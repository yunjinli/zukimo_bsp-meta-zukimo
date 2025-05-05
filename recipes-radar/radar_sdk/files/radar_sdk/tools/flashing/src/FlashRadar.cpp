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

#include "FlashRadar.h"

#include <algorithm>
#include <cstdarg>
#include <chrono>
#include <string>
#include <utility>
#include <thread>

#include "findComPort.h"

// Bossa headers
#include "Samba.h"
#include "PortFactory.h"
#include "Device.h"
#include "Flasher.h"

namespace Infineon {
    namespace Flash {
        Flasher::Flasher(std::vector<const char*> productIds /*= supportedProductIds*/) : m_productIds(std::move(productIds)) {}

        /**
         * \brief Find all com ports corresponding to USB devices with given vendor and product id
         *
         * Return a vector with all COM ports that correspond to a USB device with
         * USB vendor id vendorId and USB product id productId.
         *
         * \param [in] vendorId     USB vendor id (e.g. "12AB")
         * \param [in] productId    USB product id (e.g. "12AB")
         * \retval comPorts         vector with COM ports
         */
        std::vector<std::string> Flasher::findComPorts(const char* vendorId, const char* productId) const
        {
            return find_com_ports(vendorId, productId);
        }

        DeviceList Flasher::getDeviceList()
        {
            m_deviceList.clear();
            updateDeviceListNormalMode();
            updateDeviceListBootloaderMode();
            return m_deviceList;
        }

        void Flasher::flash(const DeviceEntry& device, const std::string& filename)
        {
            if (!device.isBootloaderMode()) {
                constexpr auto vid = FLASHRADARBASEBOARDMCU7_BOOTLOADER_VENDOR_ID;
                constexpr auto pid = FLASHRADARBASEBOARDMCU7_BOOTLOADER_PRODUCT_ID;

                if (!isDeviceInDevicesList(m_deviceList, device)) {
                    throw FlashException("Selected USB com port not exist.\n");
                }

                // List of ports that have been in bootloader mode before we
                // activate the bootloader mode of device.
                const portList alreadyInBootloaderModePorts = findComPorts(vid, pid);

                onStatus("Activating the bootloader");
                activateBootloader(device);

                // Wait at most 8 seconds (40*200ms = 8s).
                // If one more device is in bootloader mode we can stop waiting.
                for (int i = 0; i < 40; i++)
                {
                    auto comPorts = findComPorts(vid, pid);
                    if (comPorts.size() > alreadyInBootloaderModePorts.size()) {
                        // One more device is in bootloader mode
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

                auto flashPort = getNewAddedBootloaderPort(alreadyInBootloaderModePorts);

                write_firmware(flashPort, filename);
            }
            else {
                write_firmware(device.getComPort(), filename);
            }
        }

        void Flasher::registerStatusCallback(StatusCallback cb)
        {
            m_statusCallback = std::move(cb);
        }

        void Flasher::registerProgressCallback(ProgressCallback cb)
        {
            m_progressCallback = std::move(cb);
        }

        std::string Flasher::getNewAddedBootloaderPort(const portList& alreadyInBootloaderModePorts) const
        {
            constexpr auto vid = FLASHRADARBASEBOARDMCU7_BOOTLOADER_VENDOR_ID;
            constexpr auto pid = FLASHRADARBASEBOARDMCU7_BOOTLOADER_PRODUCT_ID;

            // List of ports with devices in bootloader mode that have not
            // been in bootloader moder before.
            portList candidates;

            for (const std::string& port : findComPorts(vid, pid))
            {
                if (std::find(alreadyInBootloaderModePorts.begin(), alreadyInBootloaderModePorts.end(), port) != alreadyInBootloaderModePorts.end())
                {
                    // This device has already been in bootloader mode before
                    // we enabled the bootloader mode for the current board.
                    // This is not the device we are looking for.
                    continue;
                }

                candidates.push_back(port);
            }

            if (candidates.size() > 1) {
                throw FlashException("Too many ports discovered in bootloader status\n");
            }
            else if (candidates.size() == 1) {
                return *candidates.begin();
            }
            else {
                throw FlashException("No port discovered in bootloader status\n");
            }
        }

        /**
         * \brief onStatus method
         *
         * This method corresponding with registered callbacks, override disable status callback.
         *
         * \param [in]  message      StatusMessage.
         */
        void Flasher::onStatus(const std::string& message)
        {
            if (m_statusCallback != nullptr) {
                m_statusCallback(message);
            }

        }

        /**
        * \brief onProgress method
        *
        * This method corresponding with registered callbacks, override disable progress callback.
        * onProgres is called on progress updates.
        *
        * The progress in percent is: num*100/div
        *
        * \param [in] num      current step
        * \param [in] div      total number of steps
        */
        void Flasher::onProgress(int num, int div)
        {
            if (m_statusCallback != nullptr) {
                m_progressCallback(num, div);
            }

        }

        void Flasher::updateDeviceListNormalMode()
        {
            for (const auto& productId : m_productIds)
            {
                const portList comPortsNormalMode = findComPorts(FLASHRADARBASEBOARDMCU7_IFX_VENDOR_ID, productId);

                for (const auto& comPort : comPortsNormalMode)
                    m_deviceList.emplace_back(DeviceEntry(comPort, FLASHRADARBASEBOARDMCU7_IFX_VENDOR_ID, productId));
            }
        }

        void Flasher::updateDeviceListBootloaderMode()
        {
            const portList comPortsBootloaderMode = findComPorts(FLASHRADARBASEBOARDMCU7_BOOTLOADER_VENDOR_ID, FLASHRADARBASEBOARDMCU7_BOOTLOADER_PRODUCT_ID);

            for (const auto& comPort : comPortsBootloaderMode)
                m_deviceList.emplace_back(DeviceEntry(comPort, FLASHRADARBASEBOARDMCU7_BOOTLOADER_VENDOR_ID, FLASHRADARBASEBOARDMCU7_BOOTLOADER_PRODUCT_ID));
        }

        bool Flasher::isDeviceInDevicesList(const DeviceList& deviceList, const DeviceEntry& deviceEntry) const
        {
            for (const auto& device : deviceList)
            {
                if (device.getComPort() == deviceEntry.getComPort()) {
                    return true;
                }
            }
            return false;
        }

        /**
         * \brief Write firmware to device
         *
         * Write the firmware saved in the file corresponding to filename to the device
         * associated with the COM port given by comPort.
         *
         * If an error occurs an exception of class FlashException is thrown.
         *
         * \param [in]  comPort     COM port associated to device
         * \param [in]  filename    filename of firmware
         * \retval true             if successful
         * \retval false            if an error occurred (see message for description)
         */
        void Flasher::write_firmware(const std::string& comPort, const std::string& filename)
        {
            try
            {
                const uint32_t offset = 0;
                Samba samba;
                PortFactory portFactory;

                class BossaObserver : public FlasherObserver
                {
                private:
                    Flasher* m_ptr = nullptr;
                public:
                    BossaObserver(Flasher* ptr = nullptr)
                    {
                        m_ptr = ptr;
                    }

                    void onStatus(const char* format, ...) override
                    {
                        if (m_ptr)
                        {
                            char message[512];
                            memset(message, 0, sizeof(message));

                            va_list args;
                            va_start(args, format);
                            vsnprintf(message, sizeof(message) - 1, format, args);
                            va_end(args);

                            m_ptr->onStatus(message);
                        }
                    };

                    void onProgress(int num, int div) override
                    {
                        if (m_ptr)
                            m_ptr->onProgress(num, div);
                    };
                };

                // connect to device
                bool res = samba.connect(portFactory.create(comPort));
                if (!res)
                    throw FlashException("Cannot connect to device");

                Device device(samba);
                device.create();

                Device::FlashPtr& flash = device.getFlash();

                BossaObserver observer(this);
                ::Flasher flasher(samba, device, observer);

                // erase flash
                flasher.erase(offset);

                // write firmware to flash
                flasher.write(filename.c_str(), offset);

                // verify
                uint32_t pageErrors;
                uint32_t totalErrors;
                if (!flasher.verify(filename.c_str(), pageErrors, totalErrors, offset))
                    throw FlashException("Verifying firmware failed");

                // boot
                flash->setBootFlash(true);
                flash->writeOptions();

                // reset
                onStatus("Resetting device");
                device.reset();
            }
            catch (const FlashException&)
            {
                throw;
            }
            catch (std::exception& e)
            {
                throw FlashException("An error occurred: " + std::string(e.what()));
            }
            catch (...)
            {
                throw FlashException("An unknown error occurred");
            }
        }
    }
}

namespace Infineon {
    namespace Flash {

        DeviceEntry::DeviceEntry(std::string comPort, std::string vid, std::string pid)
            : m_comPort(std::move(comPort))
            , m_vid(std::move(vid))
            , m_pid(std::move(pid))
        {}

        std::string DeviceEntry::getDescription() const
        {
            std::string board;

            if (m_pid == FLASHRADARBASEBOARDMCU7_PRODUCT_ID)
                board = "RadarBaseboardMCU7: Firmware 2.x";
            else if (m_pid == FLASHRADARBASEBOARDMCU7LEGACY_PRODUCT_ID)
                board = "RadarBaseboardMCU7Legacy: Firmware 1.x";
            else if (m_pid == FLASHRADARBASEBOARDFF_PRODUCT_ID)
                board = "RadarBaseboardMCU7: Form-Factor Model";
            else if (m_pid == FLASHRADARBASEBOARDMCU7_BOOTLOADER_PRODUCT_ID)
                board = "RadarBaseboardMCU7: Device in bootloader mode";
            else
                board = "Unknown Device:";

            return board + " (port " + m_comPort + ", product id 0x" + m_pid + ", vendor id 0x" + m_vid + ')';
        }

        std::string DeviceEntry::getComPort() const
        {
            return m_comPort;
        }

        std::string DeviceEntry::getPid() const
        {
            return m_pid;
        }

        bool DeviceEntry::isBootloaderMode() const
        {
            return (m_vid == FLASHRADARBASEBOARDMCU7_BOOTLOADER_VENDOR_ID);
        }
    }
}
