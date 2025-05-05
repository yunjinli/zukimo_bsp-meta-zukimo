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

#include <cctype>
#include <cstdarg>
#include <cstring>
#include <thread>
#include <chrono>
#include <string>

#include "FlashRadar.h"

#include "PortFactory.h"

// for strata library definitions
#include <universal/protocol/protocol_definitions.h>
// firmware definitions for target platform custom request handler
#include <universal/protocol/protocol_definitions_rdk.h>

namespace {
    // Local Data
    uint16_t crcTable[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0  // clang-format
    };

    // Local defines
    constexpr uint16_t CRC16_CCITT_FALSE_SEED = 0xFFFF;

    // Local functions
    uint16_t Crc16CcittFalse(const uint8_t buf[], unsigned int len, uint16_t crc = CRC16_CCITT_FALSE_SEED)
    {
        while (len--)
        {
            const uint8_t x = (crc >> 8) ^ *buf++;
            crc = static_cast<uint16_t>(crc << 8) ^ crcTable[x];
        }
        return crc;
    }
}

namespace {
    // replicated types from firmware code:
    struct VendorProtocol_RequestHeader
    {
        uint8_t bmReqType;
        uint8_t bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    };

    struct VendorProtocol_ResponseHeader
    {
        uint8_t bmReqType;
        uint8_t bStatus;
        uint16_t wLength;
    };

    // Typedefs for Strata message
    struct StrataPacket
    {
        VendorProtocol_RequestHeader header;
        uint8_t crc[2];
    };

    // case insensitive string equal check
    bool iequalstr(const std::string& a, const std::string& b)
    {
        const auto size = a.size();
        if (b.size() != size)
            return false;
        for (size_t i = 0; i < size; ++i)
        {
            if (std::tolower(a[i]) != std::tolower(b[i]))
                return false;
        }
        return true;
    }
}

namespace Infineon {
    namespace Flash {

        /**
         * \brief Activate bootloader mode for RadarbaseboardMCU7 board running legacy communication protocol
         *
         * Activate the bootloader of a RadarbaseboardMCU7 board that is connected to the
         * COM port given by comPort.
         *
         * On success the function returns true. If an error occurs false is returned and
         * an error description is written to message.
         *
         * \param [in]  comPort     COM port of RadarbaseboardMCU7 board
         * \param [out] message     error description
         * \param [in]  timeout     timeout for communication in miliseconds
         * \retval true             if successful
         * \retval false            if an error occurred (see message for description)
         */
        void Flasher::activateBootloaderRadarBaseboardMCU7Legacy(const std::string& comPort, int timeout) const
        {
            // write a message to explain following output
            auto handle = PortFactory().create(comPort, true);

            if (!handle->open())
                throw FlashException("Cannot open port " + comPort);

            // set timeout
            handle->timeout(timeout);

            // endpoint based
            // query endpoint information
            // --------------------------
            uint8_t uQueryEndpointInfoMessage[] = { 0x5A, 0x00, 0x01, 0x00, 0x00, 0xDBu, 0xE0u };
            uint8_t pEndpointInfoBuffer[256]; // should be enough for almost 40 endpoints
            size_t uNumResponseBytes = sizeof(pEndpointInfoBuffer);

            handle->write(uQueryEndpointInfoMessage, sizeof(uQueryEndpointInfoMessage));
            uNumResponseBytes = handle->read(pEndpointInfoBuffer, sizeof(pEndpointInfoBuffer));

            // check status message (at end of received data block)
            if ((pEndpointInfoBuffer[uNumResponseBytes - 4] != 0x5B) ||   // 0x5B start byte for status message
                (pEndpointInfoBuffer[uNumResponseBytes - 3] != 0x00) ||   // 0x00 sending endpoint
                (pEndpointInfoBuffer[uNumResponseBytes - 1] != 0x00) ||   // 0x00 low byte error code ERR_OK
                (pEndpointInfoBuffer[uNumResponseBytes - 2] != 0x00))     // 0x00 high byte error code ERR_OK
            {
                throw FlashException("OK message not detected -> Communication scheme of the connected device is unknown");
            }

            // check data message frame
            uNumResponseBytes -= 4;
            uint8_t uStartByte = pEndpointInfoBuffer[0];
            uint8_t uSendingEndpoint = pEndpointInfoBuffer[1];
            uint16_t uPayloadSize = *reinterpret_cast<uint16_t*>(&pEndpointInfoBuffer[2]);
            uint16_t uEndMarker = *reinterpret_cast<uint16_t*>(&pEndpointInfoBuffer[uNumResponseBytes - 2]);

            if ((uNumResponseBytes != size_t(uPayloadSize + 6)) ||
                (uSendingEndpoint != 0) ||
                (uStartByte != 0x5Au) ||
                (uEndMarker != 0xE0DBu))
            {
                throw FlashException("Broken Frame -> Communication scheme of the connected device is unknown");
            }

            // check payload header
            uint8_t uMessageType = pEndpointInfoBuffer[4];
            uint8_t uNumEndpoints = pEndpointInfoBuffer[5];
            if ((uMessageType != 0) || (uNumEndpoints * 6 + 2 != uPayloadSize))
            {
                throw FlashException("Unexpected message type -> Communication scheme of the connected device is unknown");
            }

            // search for BSL endpoint
            uint8_t uBSLEndpoint = 0xFFu;
            uint8_t* pEndpointInfoTable = pEndpointInfoBuffer + 6;
            for (uint8_t ep = 0; ep < uNumEndpoints; ++ep)
            {
                if (*reinterpret_cast<uint32_t*>(pEndpointInfoTable + static_cast<uint32_t>(ep) * 6) == 0x5542534C)
                {
                    uBSLEndpoint = ep + 1;
                    break;
                }
            }
            if (uBSLEndpoint == 0xFF)
            {
                throw FlashException("Unexpected message type -> Communication scheme of the connected device is unknown");
            }

            // send reset command to endpoint
            // ------------------------------
            uint8_t pRebootMsg[] = { 0x5A, uBSLEndpoint, 0x01, 0x00, 0x00, 0xDBu, 0xE0u };
            uNumResponseBytes = 0;
            handle->write(pRebootMsg, sizeof(pRebootMsg));
        }

        /**
         * \brief Activate bootloader mode for RadarbaseboardMCU7 board
         *
         * Activate the bootloader of a RadarbaseboardMCU7 board that is connected to the
         * COM port given by comPort.
         *
         * On success the function returns true. If an error occurs false is returned and
         * an error description is written to message.
         *
         * \param [in]  comPort     COM port of RadarbaseboardMCU7 board
         * \param [out] message     error description
         * \param [in]  timeout     timeout for communication in miliseconds
         * \retval true             if successful
         * \retval false            if an error occurred (see message for description)
         */
        void Flasher::activateBootloaderRadarBaseboardMCU7(const std::string& comPort, int timeout) const
        {
            // Create a COM port handle
            auto handle = PortFactory().create(comPort, true);

            if (!handle->open())
                throw FlashException("Cannot open port " + comPort);

            // set timeout
            handle->timeout(timeout);

            // construct Strata Message
            // --------------------------
            StrataPacket strata_msg;
            VendorProtocol_ResponseHeader* strata_resp;
            uint8_t crc_8[2];

            strata_msg.header.bmReqType = VENDOR_REQ_READ;
            strata_msg.header.bRequest = REQ_CUSTOM;
            strata_msg.header.wValue = REQ_CUSTOM_BOOT_LOADER;
            strata_msg.header.wIndex = REQ_CUSTOM_BOOT_LOADER_FUNCTION_CHK;
            strata_msg.header.wLength = 0;

            uint16_t crc = Crc16CcittFalse(reinterpret_cast<uint8_t*>(&strata_msg), sizeof(strata_msg.header));

            strata_msg.crc[0] = static_cast<uint8_t>(crc >> 8);
            strata_msg.crc[1] = static_cast<uint8_t>(crc);

            uint8_t pResponseBuffer[16]; // large enough for a strata response message
            size_t uNumResponseBytes = sizeof(pResponseBuffer);

            handle->write(reinterpret_cast<uint8_t*>(&strata_msg), sizeof(strata_msg));
            uNumResponseBytes = handle->read(pResponseBuffer, sizeof(pResponseBuffer));

            strata_resp = reinterpret_cast<VendorProtocol_ResponseHeader*>(pResponseBuffer);
            crc = Crc16CcittFalse(reinterpret_cast<uint8_t*>(strata_resp), sizeof(VendorProtocol_ResponseHeader));
            crc_8[0] = static_cast<uint8_t>(crc >> 8);
            crc_8[1] = static_cast<uint8_t>(crc);
            // check status message (at end of received data block)
            if ((uNumResponseBytes != 6) ||   // false length of response
                (strata_resp->bmReqType != VENDOR_REQ_READ) ||
                (strata_resp->bStatus != STATUS_SUCCESS) ||
                (crc_8[0] != *(pResponseBuffer + sizeof(VendorProtocol_ResponseHeader))) ||
                (crc_8[1] != *(pResponseBuffer + sizeof(VendorProtocol_ResponseHeader) + 1)))
            {
                throw FlashException("OK message not detected -> Communication scheme of the connected device is unknown");
            }

            // Protocol is recognized
            // prepare activate and reset command
            // ------------------------------
            strata_msg.header.bmReqType = VENDOR_REQ_WRITE;
            strata_msg.header.bRequest = REQ_CUSTOM;
            strata_msg.header.wValue = REQ_CUSTOM_BOOT_LOADER;
            strata_msg.header.wIndex = REQ_CUSTOM_BOOT_LOADER_ACTIVATE;
            strata_msg.header.wLength = 0;

            crc = Crc16CcittFalse(reinterpret_cast<uint8_t*>(&strata_msg), sizeof(strata_msg.header));
            strata_msg.crc[0] = static_cast<uint8_t>(crc >> 8);
            strata_msg.crc[1] = static_cast<uint8_t>(crc);

            // send activate and reset command
            // ------------------------------
            handle->write(reinterpret_cast<uint8_t*>(&strata_msg), sizeof(strata_msg));
        }

        /**
         * \brief Activate bootloader mode for FF board
         *
         * Activate the bootloader of an FF board that is connected to the
         * COM port given by comPort.
         *
         * On success the function returns true. If an error occurs false is returned and
         * an error description is written to message.
         *
         * \param [in]  comPort     COM port of FF board
         * \param [out] message     error description
         * \param [in]  timeout     timeout for communication in miliseconds
         * \retval true             if successful
         * \retval false            if an error occurred (see message for description)
         */
        void Flasher::activateBootloaderFF(const std::string& comPort, int timeout) const
        {
            // write a message to explain following output
            auto handle = PortFactory().create(comPort, true);

            if (!handle->open())
                throw FlashException("Cannot open port " + comPort);

            // set timeout
            handle->timeout(timeout);

            // endpoint based
            // query endpoint information
            // --------------------------
            char enterBootloaderResponse[128];
            // read everything in stream first
            while (handle->read(reinterpret_cast<uint8_t*>(enterBootloaderResponse), sizeof(enterBootloaderResponse)) > 0) {}
            // clear response bytes
            memset(enterBootloaderResponse, '\0', sizeof(enterBootloaderResponse));
            // write enter bootloader command
            uint8_t enterBootloaderCmd[] = "enter_bootloader\n";
            handle->write(enterBootloaderCmd, sizeof(enterBootloaderCmd));
            handle->flush();

            // sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // read response
            handle->read(reinterpret_cast<uint8_t*>(enterBootloaderResponse), sizeof(enterBootloaderResponse));
            enterBootloaderResponse[sizeof(enterBootloaderResponse) - 1] = '\0';
            // check if response ends with the expected OK with CRLF
            if (strcmp(enterBootloaderResponse + strlen(enterBootloaderResponse) - strlen("OK\r\n"), "OK\r\n") != 0)
            {
                throw FlashException("OK message not detected -> Communication scheme of the connected device is unknown");
            }
        }

        /**
         * \brief Calls board specific bootloader activation
         *
         * Selects the appropriate bootloader mode activation depending on the input product ID
         * of the board detected on the COM port given by comPort.
         *
         * On success the function returns true. If an error occurs false is returned and
         * an error description is written to message.
         *
         * \param [in]  productID   4 character Product ID of the detected board
         * \param [in]  comPort     COM port of detected board
         * \param [out] message     error description
         * \retval true             if successful
         * \retval false            if an error occurred (see message for description)
         */
        void Flasher::activateBootloader(const DeviceEntry& device) const
        {
            const auto productId = device.getPid();
            auto comPort = device.getComPort();

            if (iequalstr(productId, FLASHRADARBASEBOARDMCU7LEGACY_PRODUCT_ID))
                activateBootloaderRadarBaseboardMCU7Legacy(comPort);
            else if (iequalstr(productId, FLASHRADARBASEBOARDMCU7_PRODUCT_ID))
                activateBootloaderRadarBaseboardMCU7(comPort);
            else if (iequalstr(productId, FLASHRADARBASEBOARDFF_PRODUCT_ID))
                activateBootloaderFF(comPort);
            else
                throw FlashException("Board product ID " + productId + " not supported for bootloader activation");
        }
    }
}