/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <platform/interfaces/IVendorCommands.hpp>

class RemoteVendorCommands
{
public:
    RemoteVendorCommands(IVendorCommands *commands, uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface);

    void vendorWrite(uint8_t bFunction, uint16_t wLength, const uint8_t buffer[]) const;
    void vendorRead(uint8_t bFunction, uint16_t wLength, uint8_t buffer[]) const;
    void vendorTransfer(uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[]) const;
    void vendorTransferChecked(uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[]) const;

    /*
     * This function optimizes reading serial data from the communication interface to a struct
     * by not needing an additional buffer for receiving the data.
     * It directly reads the data to the end of the struct and then deserializes it
     * into the same memory.
     */
    template <typename T>
    void vendorReadToStruct(uint8_t bFunction, T *targetStruct, uint8_t count = 1)
    {
        constexpr size_t structSerialSize = serialized_sizeof(T());
        const uint16_t readSize           = static_cast<uint16_t>(structSerialSize * count);
        const uint16_t offset             = static_cast<uint16_t>((sizeof(T) * count) - readSize);
        uint8_t *buf                      = reinterpret_cast<uint8_t *>(targetStruct) + offset;
        vendorRead(bFunction, readSize, buf);
        const uint8_t *it = buf;
        for (uint8_t i = 0; i < count; i++)
        {
            it = serialToHost(it, &targetStruct[i]);
        }
    }

    inline uint16_t getMaxTransfer() const
    {
        return m_commands->getMaxTransfer();
    }

private:
    IVendorCommands *m_commands;
    uint8_t m_bRequest;
    uint8_t m_bType;
    uint8_t m_bImplementation;
    uint8_t m_bId;
    uint8_t m_bSubInterface;
};
