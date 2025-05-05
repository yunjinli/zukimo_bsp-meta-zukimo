/* ===========================================================================
** Copyright (C) 2022 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include <string>
#include <vector>

#if defined(__linux__) || defined(linux) || defined(__linux)
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#endif


#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>

static std::vector<std::string> find_com_ports_win32(const char* vid, const char* pid)
{
    std::vector<std::string> ports;

    // plug and play id: USB\VID_XXXX&PID_XXXX where XXXX are hexadecimal characters
    char pnpid[32];
    snprintf(pnpid, sizeof(pnpid), "USB\\VID_%c%c%c%c&PID_%c%c%c%c",
        toupper(vid[0]), toupper(vid[1]), toupper(vid[2]), toupper(vid[3]),
        toupper(pid[0]), toupper(pid[1]), toupper(pid[2]), toupper(pid[3]));

    // SetupDiGetClassDevs returns a handle to a device information set that
    // contains requested device information elements for a local computer
    HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(nullptr, "USB", nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (DeviceInfoSet == INVALID_HANDLE_VALUE)
        return ports;

    DWORD DeviceIndex = 0;
    SP_DEVINFO_DATA DeviceInfoData { };
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // SetupDiEnumDeviceInfo returns a SP_DEVINFO_DATA structure that specifies
    // a device information element in a device information set
    while (SetupDiEnumDeviceInfo(DeviceInfoSet, DeviceIndex++, &DeviceInfoData))
    {
        BYTE buffer[256] = { 0 };
        DEVPROPTYPE ulPropertyType;
        DWORD dwSize1;

        // get specified plug and play device property
        if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, &ulPropertyType, buffer, sizeof(buffer), &dwSize1))
        {
            // check if this is the device we are looking for
            if (strncmp((char*)buffer, pnpid, strlen(pnpid)) != 0)
                continue;

            // open registry
            HKEY hDeviceRegistryKey = SetupDiOpenDevRegKey(DeviceInfoSet, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            if (hDeviceRegistryKey == INVALID_HANDLE_VALUE)
                continue;

            // Read in the name of the port
            char port_name[32] = { 0 };
            DWORD dwSize2 = sizeof(port_name);
            DWORD dwType = 0;

            // query
            LSTATUS ret = RegQueryValueEx(hDeviceRegistryKey, "PortName", nullptr, &dwType, (LPBYTE)port_name, &dwSize2);

            // close registry key
            RegCloseKey(hDeviceRegistryKey);

            if (ret == ERROR_SUCCESS && (dwType == REG_SZ))
            {
                // check if the usb device corresponds to a COM port
                if (strncmp(port_name, "COM", 3) == 0)
                {
                    ports.push_back(std::string(port_name));
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(DeviceInfoSet);

    return ports;
}
#elif defined(__linux__) || defined(linux) || defined(__linux)
static bool file_exists(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

/* check for PID and VID in uevent file */
static bool check_uevent(const char* path, const char* vid, const char* pid)
{
    char buffer[2048];
    bool ret = false;
    auto vid_ = strtol(vid, nullptr, 16);
    auto pid_ = strtol(pid, nullptr, 16);

    FILE* fh = fopen(path, "r");
    if (fh == nullptr)
        return false;

    while (fgets(buffer, sizeof(buffer), fh))
    {
        char* p = nullptr;
        if (strncmp(buffer, "PRODUCT=", 8) != 0)
            continue;

        /* PRODUCT=58b/58/1 */
        char* s_vid = buffer + 8;
        if ((p = strchr(s_vid, '/')) == nullptr)
            break;
        *p = '\0';
        char* s_pid = p + 1;

        if ((p = strchr(s_pid, '/')) == nullptr)
            break;
        *p = '\0';

        if ((strtol(s_vid, nullptr, 16) == vid_) && (strtol(s_pid, nullptr, 16) == pid_))
        {
            ret = true;
            break;
        }
    }

    fclose(fh);

    return ret;
}

std::vector<std::string> find_com_ports_linux(const char* vid, const char* pid)
{
    std::vector<std::string> ports;
    const char* sysdir = "/sys/class/tty/";
    DIR* dir_handle = opendir(sysdir);
    if (dir_handle == nullptr)
        return ports;

    struct dirent* dir_entry;
    while ((dir_entry = readdir(dir_handle)) != nullptr)
    {
        /* search for /sys/class/tty/ttyACMXXX where XXX is a number */
        const char* fname = dir_entry->d_name;
        if (strncmp(fname, "ttyACM", 6) != 0)
            continue;

        /* check for correct vendor and product id in /sys/class/tty/ttyACMXXX/device/uevent */
        char path[PATH_MAX] = { 0 };
        snprintf(path, sizeof(path) - 1, "%s/%s/device/uevent", sysdir, fname);
        if (check_uevent(path, vid, pid))
        {
            std::string port = std::string("/dev/") + fname;
            if (file_exists(port))
                ports.push_back(port);

        }
    }

    closedir(dir_handle);
    return ports;
}
#elif defined(__APPLE__)
#include <sys/param.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/serial/ioss.h>
#include <IOKit/IOBSD.h>

static int hex2int(const char* hex)
{
	return (int)strtol(hex, NULL, 16);
}

std::vector<std::string> find_com_ports_mac(const char* vid, const char* pid)
{
	std::vector<std::string> ports;

    CFMutableDictionaryRef classes = IOServiceMatching(kIOSerialBSDServiceValue);
    if (classes == NULL)
        return ports;

    // Look for devices that claim to be modems.
    CFDictionarySetValue(classes, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDAllTypes));

    // Get an iterator across all matching devices.
    io_iterator_t iterator;
    if (IOServiceGetMatchingServices(kIOMasterPortDefault, classes, &iterator) != KERN_SUCCESS)
        return ports;

    // Iterate across all modems found. In this example, we bail after finding the first modem.
    io_object_t it;
    while ((it = IOIteratorNext(iterator)))
    {
        char port_name[MAXPATHLEN] = { 0 };

        auto bsdPathAsCFString = static_cast<CFStringRef>(IORegistryEntryCreateCFProperty(it, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0));
        if (!bsdPathAsCFString)
            continue;

        CFStringGetCString(bsdPathAsCFString, port_name, sizeof(port_name), kCFStringEncodingUTF8);
        CFRelease(bsdPathAsCFString);

        if (strlen(port_name) == 0)
            continue;

        auto cf_vid = static_cast<CFNumberRef>(IORegistryEntrySearchCFProperty(it, kIOServicePlane, CFSTR("idVendor"), kCFAllocatorDefault, kIORegistryIterateRecursively | kIORegistryIterateParents));
        auto cf_pid = static_cast<CFNumberRef>(IORegistryEntrySearchCFProperty(it, kIOServicePlane, CFSTR("idProduct"), kCFAllocatorDefault, kIORegistryIterateRecursively | kIORegistryIterateParents));

        int pid_found = 0;
		int vid_found = 0;
        if (cf_vid && cf_pid &&
            CFNumberGetValue(cf_vid, kCFNumberIntType, &vid_found) &&
            CFNumberGetValue(cf_pid, kCFNumberIntType, &pid_found))
        {
			if(pid_found == hex2int(pid) && vid_found == hex2int(vid))
                ports.push_back(port_name);
        }

        // Release CFTypeRef
        if (cf_vid)
            CFRelease(cf_vid);
        if (cf_pid)
            CFRelease(cf_pid);

        // Release the io_service_t now that we are done with it.
        IOObjectRelease(it);
    }

    IOObjectRelease(iterator);

    return ports;
}
#endif



/**
 * \brief Check if vendor id and product id are valid
 *
 * The vendor id (VID) and product id (pid) are valid if the string consists
 * of exactly 4 hexadecimal characters, so each of the 4 characters must be
 * one of [0-9abcdefABCDEF].
 *
 * \param [in] vid  vendor id
 * \param [in] pid  product id
 * \retval true     if vendor and product id are valid
 * \retval false    if vendor and product id are invalid
 */
static bool valid_vid_pid(const char* vid, const char* pid)
{
    if (strlen(vid) != 4 || strlen(pid) != 4)
        return false;

    for (int i = 0; i < 4; i++)
    {
        if (!isxdigit(vid[i]) || !isxdigit(pid[i]))
            return false;
    }

    return true;
}

std::vector<std::string> find_com_ports(const char* vid, const char* pid)
{
    std::vector<std::string> ports;

    if (!valid_vid_pid(vid, pid))
        return ports;

#ifdef _WIN32
    return find_com_ports_win32(vid, pid);
#elif defined(__linux__) || defined(linux) || defined(__linux)
    return find_com_ports_linux(vid, pid);
#elif defined(__APPLE__)
    return find_com_ports_mac(vid, pid);
#else
    #error "Platform not supported (only Linux, Windows, and Mac OS are supported)"
#endif
}
