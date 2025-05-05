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

#ifndef FLASH_RADAR_H
#define FLASH_RADAR_H

/* On Windows symbols (functions) in a DLL (shared library) are not exported by
 * default. You have to tell the linker to export the symbol when you build the
 * library and you have to tell the linker (?) to import the symbol when using
 * the library. This is done with __declspec:
 *     Compiling the library: __declspec(dllexport) void foo(void);
 *     Using the library:     __declspec(dllimport) void foo(void);
 * More information can be found here:
 * https://stackoverflow.com/questions/33062728/cmake-link-shared-library-on-windows/41618677
 *
 * In contrast, on Linux the default is to export all symbols. For gcc
 * (probably also clang), see https://gcc.gnu.org/wiki/Visibility.
 *
 * The build system has to set correct preprocessor defines (cmake works).
 * - If the library is build as a static library, no preprocessor name is
 *   needed.
 * - If the library is build as a dynamic library:
 *      - Windows: flash_rbb_mcu7_EXPORTS must be set when compiling the library
 *      - Windows: flash_rbb_mcu7_EXPORTS must not be set when linking
 *      - Linux: No preprocessor defines are needed
 */
#ifdef FLASHLIB_BUILD_STATIC
 // build as static library; no visibility
#define IFX_DLL_PUBLIC
#define IFX_DLL_HIDDEN
#elif defined(_MSC_VER) || defined(__MINGW64__) || defined(__WIN32__)
 // default visibility is hidden, so IFX_DLL_HIDDEN is a noop
#define IFX_DLL_HIDDEN

#ifndef IFX_DLL_PUBLIC
#ifdef flash_rbb_mcu7_EXPORTS
    // We are building this library
#define IFX_DLL_PUBLIC __declspec(dllexport)
#else
    // We are using this library
#define IFX_DLL_PUBLIC __declspec(dllimport)
#endif
#endif
#elif (__GNUC__ >= 4) || (__clang_major__ >= 5)
 // see https://gcc.gnu.org/wiki/Visibility
#define IFX_DLL_PUBLIC __attribute__ ((visibility ("default")))
#define IFX_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
#else
#define IFX_DLL_PUBLIC
#define IFX_DLL_HIDDEN
#endif

#include <string>
#include <stdexcept>
#include <functional>
#include <vector>

/// USB vendor id of Infineon products
#define FLASHRADARBASEBOARDMCU7_IFX_VENDOR_ID "058B"

/// USB vendor id of RadarBaseboardMCU7 in bootloader mode
#define FLASHRADARBASEBOARDMCU7_BOOTLOADER_VENDOR_ID "03EB"

/// USB product id of RadarBaseboardMCU7 in bootloader mode
#define FLASHRADARBASEBOARDMCU7_BOOTLOADER_PRODUCT_ID "6124"

/// USB product id of RadarBaseboardMCU7 (firmware version 2.0 or higher)
#define FLASHRADARBASEBOARDMCU7_PRODUCT_ID "0251"

/// USB vendor id of RadarBaseboardMCU7
#define FLASHRADARBASEBOARDMCU7_VENDOR_ID FLASHRADARBASEBOARDMCU7_IFX_VENDOR_ID

/// USB product id of RadarBaseboardMCU7 with legacy firmware (before version 2.0)
#define FLASHRADARBASEBOARDMCU7LEGACY_PRODUCT_ID "0058"

/// USB product id of RadarBaseboard Form Factor model
#define FLASHRADARBASEBOARDFF_PRODUCT_ID "1058"

/* namespaces */
namespace Infineon
{
    namespace Flash
    {
        /// Exception thrown by Flasher class
        class FlashException final : public std::exception
        {
        public:
            explicit FlashException(std::string what) : m_what(std::move(what)) {}

            const char* what() const throw() override
            {
                return m_what.c_str();
            }

        private:
            std::string m_what;
        };

        
        /** 
         * \brief Class for keeping device information.
         *
         * This class describe the HW boards and keeps the information about vendor Id, product Id and USB com port.
         * It simply uses to passed by a parameter in the flash device class.
         */
        class DeviceEntry
        {
        public:
            /** 
            * \brief Device Entry class
            *
            * \param [in] comPort  current port number
            * \param [in] vid      vendor id (four hexadecimal characters)
            * \param [in] pid      product id (four hexadecimar characters)
            */
            IFX_DLL_PUBLIC DeviceEntry(std::string comPort, std::string vid, std::string pid);

            /**
            * \brief Get Description about Device.
            * 
            * Human readable device description.
            *
            * \retval description    device description.
            */
            IFX_DLL_PUBLIC std::string getDescription() const;

            /**
            * \brief Get USB device port.
            *
            * \retval port  Port of USB device.
            */
            IFX_DLL_PUBLIC std::string getComPort() const;

            /**
            * \brief Get Device product ID.
            *
            * \retval pid   Device product ID (four hexadecimal characters)
            */
            IFX_DLL_PUBLIC std::string getPid() const;

            /**
             * \brief Return if device is in bootloader mode.
             *
             * \retval true     Device is in bootloader mode.
             * \retval false    Device is not in bootloader mode.
             */
            IFX_DLL_PUBLIC bool isBootloaderMode() const;
            
        private:
            friend class Flasher;

            const std::string m_comPort;
            const std::string m_vid;
            const std::string m_pid;
        };

        const std::vector<const char*> supportedProductIds{ FLASHRADARBASEBOARDMCU7_PRODUCT_ID,
                                                         FLASHRADARBASEBOARDMCU7LEGACY_PRODUCT_ID,
                                                         FLASHRADARBASEBOARDFF_PRODUCT_ID };
        /** Status callback.
         * 
         * \param [in] message  Description of status.
         */
        using StatusCallback = std::function<void(const std::string& message)>;

        /** Progress callback
         *
         * The progress in percent is: num*100.0/div.
         *
         * \param [in] num      Current step.
         * \param [in] div      Total number of steps.
         */
        using ProgressCallback = std::function<void(int num, int div)>;

        /// Device list
        using DeviceList = std::vector<DeviceEntry>;

        /// USB com ports list
        using portList = std::vector<std::string>;

        /**
        * \brief Class for flashing RadarBaseboardMCU7 boards.
        *
        * This class allows to update the firmware on RadarBaseboardMCU7 boards.
        *
        * Typical steps to update the firmware:
        * 1. Search for connected RadarBaseboardMCU7 devices using \ref getDeviceList.
        *    The method lists all connected boards (bootloader mode or normal mode).
        * 2. Choose one device from list and use the method \ref flash
        *    to upload new firmware.
        * 3. Do not connect/disconnect boards during these steps.
        */
        class Flasher
        {
        public:
            /**
            * \brief Create new FlashRadarBaseboardMCU7 object.
            * 
            * productIds is set by default for all supported products version 1.x and 2.x.
            * 
            * \param [in] productIds    List of USB product ids (e.g. "12AB").
            */
            IFX_DLL_PUBLIC explicit Flasher(std::vector<const char*> productIds = supportedProductIds);

            /**
            * \brief Return list of all connected USB boards.
            *
            * Return a vector of DeviceEntry corresponding to all connected USB boards.
            *
            * \retval DeviceList       vector of DeviceEntry.
            */
            IFX_DLL_PUBLIC DeviceList getDeviceList();

            /**
             * \brief Write firmware to device.
             *
             * Write the firmware saved in the file corresponding to filename to the device.
             * If necessary the bootloader will be activated before flashing.
             *
             * If an error occurs an exception of type FlashException is thrown.
             *
             * Note that if the device was put in bootloader mode automatically
             * by putting test point TP1 to 3.3V while connecting the board,
             * the device will remain in bootloader mode after flashing. In this
             * case the user has to manually unplug and plug the USB board.
             *
             * \param [in]  device      DeviceEntry corresponds with one device to flash.
             * \param [in]  filename    Filename of firmware.
             */
            IFX_DLL_PUBLIC void flash(const DeviceEntry& device, const std::string& filename);

            /**
             * \brief register callback status
             *
             * This methods registers a callback function for the status.
             *
             * The callback function cb is called whenever the status changes.
             * Status changes might be activating the bootloader, writing the firmware,
             * verifying the written firmware, or erasing the flash.
             *
             * To unregister a status callback, pass nullptr for cb.
             * 
             * \param [in]  cb      Status callback.
             */
            IFX_DLL_PUBLIC void registerStatusCallback(StatusCallback cb);

            /**
             * \brief register callback progress
             *
             * This methods registers a callback function for the progress.
             *
             * The callback is function cb is called whenever the progress changes.
             *
             * Note that a progress indication is only available for writing and
             * verifying the firmware.
             *
             * To unregister a progress callback, pass nullptr for cb.
             * 
             * \param [in]  cb      Progress callback.
             */
            IFX_DLL_PUBLIC void registerProgressCallback(ProgressCallback cb);

        private:
            // private variables
            StatusCallback m_statusCallback = nullptr;
            ProgressCallback m_progressCallback = nullptr;
            const std::vector<const char*> m_productIds;
            DeviceList m_deviceList;

            // private methods
            void onStatus(const std::string& message);
            void onProgress(int num, int div);

            std::vector<std::string> findComPorts(const char* vendorId, const char* productId) const;

            void write_firmware(const std::string& comPort, const std::string& filename);

            void activateBootloaderRadarBaseboardMCU7Legacy(const std::string& comPort, int timeout = 2000) const;
            void activateBootloaderRadarBaseboardMCU7(const std::string& comPort, int timeout = 2000) const;
            void activateBootloaderFF(const std::string& comPort, int timeout = 2000) const;
            void activateBootloader(const DeviceEntry& device) const;

            void updateDeviceListNormalMode();
            void updateDeviceListBootloaderMode();
            
            bool isDeviceInDevicesList(const DeviceList& deviceList, const DeviceEntry& deviceEntry) const;
            std::string getNewAddedBootloaderPort(const portList& alreadyInBootloaderModePorts) const;
        };
    } // namespace Flash
} // namespace Infineon

#endif /* FLASH_RADAR_H */
