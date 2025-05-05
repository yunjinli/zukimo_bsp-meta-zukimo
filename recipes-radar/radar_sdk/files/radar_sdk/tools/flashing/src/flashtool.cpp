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

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "FlashRadar.h"
#include "argparse.h"

/* ========= Macros ========== */

// status definitions
constexpr char STATUS_BOOTLOADER[] = "BOOTLOADER";
constexpr char STATUS_ERASE[] = "ERASE";
constexpr char STATUS_WRITE[] = "WRITE";
constexpr char STATUS_VERIFY[] = "VERIFY";
constexpr char STATUS_RESET[] = "RESET";
constexpr char STATUS_EXIT[] = "EXIT";

// error codes
enum class Error : int
{
    ERROR_MISSING_PARAMETER = 1,
    ERROR_READING_FIRMWARE,
    ERROR_ACTIVATING_BOOTLOADER,
    ERROR_DEVICE_NOT_FOUND,
    ERROR_CONNECTING_FAILED,
    ERROR_FLASHING,
    ERROR_UNKNOWN
};

// Return true if file is readable otherwise false
static bool file_readable(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

std::string step_;
std::string message_;

// Print status message
static void status_message(const char *step, int progress, const std::string &message)
{
    fprintf(stdout, "%s: %d %s", step, progress, message.c_str());
    if (!message.empty())
    {
        if (message.back() != '\n')
            fprintf(stdout, "\n");
    }
}

static void status_message(const std::string &message)
{
    fprintf(stdout, "%s", message.c_str());
    if (!message.empty())
    {
        if (message.back() != '\n')
            fprintf(stdout, "\n");
    }
}

// example callback
void StatusCallbackFunc(const std::string &message)
{
    message_ = message;

    if (message.compare(0, 5, "Erase") == 0)
    {
        step_ = std::string(STATUS_ERASE);
        status_message(step_.c_str(), 0, message_);
    }
    else if (message.compare(0, 5, "Write") == 0)
    {
        step_ = std::string(STATUS_WRITE);
        status_message(step_.c_str(), 0, message_);
    }
    else if (message.compare(0, 6, "Verify") == 0)
    {
        step_ = std::string(STATUS_VERIFY);
        status_message(step_.c_str(), 0, message_);
    }
    else if (message.compare(0, 9, "Resetting") == 0)
    {
        step_ = std::string(STATUS_RESET);
        status_message(step_ + std::string(": ") + message_);
    }
    else if (message == "Activating the bootloader")
    {
        step_ = std::string(STATUS_BOOTLOADER);
        message_ = "Activating the bootloader";
        status_message(step_.c_str(), 0, message_);
    }
    else if (message.find("bootloader activated") != std::string::npos)
    {
        step_ = std::string(STATUS_BOOTLOADER);
        status_message(step_.c_str(), 100, message_);
    }
    else
    {
        status_message(message_);
    }
}

void ProgressCallbackFunc(int num, int div)
{
    status_message(step_.c_str(), (num * 100) / div, message_);
}

void print_devices_list(Infineon::Flash::DeviceList &device_list)
{
    for (const auto &device : device_list)
    {
        std::cout << device.getDescription() << '\n';
    }
}

bool is_devices_empty(Infineon::Flash::DeviceList &device_list)
{
    if (device_list.empty())
    {
        status_message(STATUS_EXIT, int(Error::ERROR_DEVICE_NOT_FOUND), "No boards found.");
        return true;
    }
    return false;
}

bool is_file_empty(const std::string &file_name)
{
    if (!file_readable(file_name))
    {
        status_message(STATUS_EXIT, int(Error::ERROR_READING_FIRMWARE), "Cannot open '" + std::string(file_name) + "' for reading");
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    bool show_connected_boards = false;
    bool flash_all_boards = false;
    char *port_string = nullptr;
    char *file_string = nullptr;

    const char *const usage_str[] = {
        "flash tool [OPTIONS] [FILE]",
        nullptr};

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Options"),
        OPT_BOOLEAN('l', "list", &show_connected_boards, "show connected boards", nullptr, 0, 0),
        OPT_BOOLEAN('a', "all", &flash_all_boards, "flash all connected boards", nullptr, 0, 0),
        OPT_STRING('p', "port", &port_string, "select port to flash", nullptr, 0, 0),
        OPT_GROUP("Examples:\n\
    flashtool_rbb7 -a [FILE]          Flash firmware to all connected boards\n\
    flashtool_rbb7 -p [PORT] [FILE]   Flash firmware to board connected at PORT\n\
    flashtool_rbb7 [FILE]             Flash firmware to first board found\n\
    flashtool_rbb7 -l                 List all connected devices"),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usage_str, 0);
    argparse_describe(&argparse, "Tool for flashing firmware to RadarBaseboardMCU7 boards", nullptr);

    // argv[0] to argv[argc-1] contain the command line parameters not processed yet.
    argc = argparse_parse(&argparse, argc, argv);

    if (argc > 0)
        file_string = argv[0];

    using Infineon::Flash::DeviceEntry;
    using Infineon::Flash::Flasher;

    Flasher flash;

    // register own callbacks to get messages.
    flash.registerProgressCallback(ProgressCallbackFunc);
    flash.registerStatusCallback(StatusCallbackFunc);

    if (show_connected_boards)
    {
        /**
         * Show all connected boards.
         * Parameters: [--list] or [-l]
         */
        auto devicesList = flash.getDeviceList();
        if (is_devices_empty(devicesList))
        {
            return 1;
        }
        print_devices_list(devicesList);

        return 0;
    }

    // Check that path to firmware is given and the firmware can be read.
    if (!file_string)
    {
        std::cout << "Path to firmware is missing.\n";
        return 1;
    }
    else if (is_file_empty(file_string))
    {
        std::cout << "Cannot read firmware file " << file_string << "\n";
        return 1;
    }

    if (flash_all_boards)
    {
        /**
        * Flash all connected boards with a specified binary file.
        * Pameters: [--all] or [-a]
        *
        */
        std::cout << "Flashing All boards started... \n";
        auto devices_list = flash.getDeviceList();
        if (is_devices_empty(devices_list))
        {
            return 1;
        }

        print_devices_list(devices_list);

        size_t boards_found = devices_list.size();
        size_t boards_flashed = 0;
        for (auto& device : devices_list)
        {
            std::cout << device.getComPort() << " Flashing started...\n";

            try
            {
                flash.flash(device, file_string);
                std::cout << device.getComPort() << " Flashed.\n";
                boards_flashed++;
            }
            catch (Infineon::Flash::FlashException& e)
            {
                std::cout << "FlashToolException : " << e.what() << '\n';
            }
        }
        std::cout << "[" << boards_flashed << "/" << boards_found << ']' << " boards have been flashed. Actual status:\n";

        // need time to reload.
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        devices_list = flash.getDeviceList(); // refresh.
        print_devices_list(devices_list);
        return 0;
    }
    else
    {
        // either flash first board found or board connected to port given by port_string
        auto devicesList = flash.getDeviceList();

        if (is_devices_empty(devicesList))
        {
            std::cout << "No board found\n";
            return 1;
        }

        for (auto &device : devicesList)
        {
            if (port_string == nullptr || port_string == device.getComPort())
            {
                std::cout << device.getComPort() << " Start flashing...\n";

                try
                {
                    flash.flash(device, file_string);
                    std::cout << device.getComPort() << " board has been flashed.\n";
                    return 0;
                }
                catch (Infineon::Flash::FlashException &e)
                {
                    std::cout << "FlashToolException : " << e.what() << '\n';
                    return 1;
                }
            }
        }
    }

    return 0;
}
