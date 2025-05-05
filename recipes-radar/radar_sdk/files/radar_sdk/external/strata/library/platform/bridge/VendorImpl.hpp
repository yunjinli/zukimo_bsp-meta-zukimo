/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <common/EndianConversion.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class VendorImpl :
    public IVendorCommands
{
protected:
    constexpr static const uint16_t m_commandHeaderSize  = 8;
    constexpr static const uint16_t m_responseHeaderSize = 4;

public:
    // IVendorCommands implementation
    using IVendorCommands::vendorRead;
    using IVendorCommands::vendorTransfer;
    using IVendorCommands::vendorWrite;

    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint16_t buffer[]) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint32_t buffer[]) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int8_t buffer[]) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int16_t buffer[]) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int32_t buffer[]) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const float buffer[]) override;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const double buffer[]) override;

    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint16_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint32_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int8_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int16_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int32_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, float buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, double buffer[]) override;

    void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const uint16_t bufferIn[], uint16_t &wLengthOut, uint16_t bufferOut[]) override;
    void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const uint32_t bufferIn[], uint16_t &wLengthOut, uint32_t bufferOut[]) override;

    void vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[]) override;
    void vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint16_t bufferSend[], uint16_t wLengthReceive, uint16_t bufferReceive[]) override;
    void vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint32_t bufferSend[], uint16_t wLengthReceive, uint32_t bufferReceive[]) override;

    void vendorWrite(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLength, const uint8_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLength, uint8_t buffer[]) override;
    void vendorTransfer(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[]) override;
    void vendorTransferChecked(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[]) override;

private:
    template <typename T>
    typename std::enable_if<is_little_endian<T>::value>::type
    vendorWriteImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const T buffer[]);
    template <typename T>
    typename std::enable_if<is_big_endian<T>::value>::type
    vendorWriteImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const T buffer[]);

    template <typename T>
    void vendorReadImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, T buffer[]);

    template <typename T>
    typename std::enable_if<is_little_endian<T>::value>::type
    vendorTransferImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const T bufferIn[], uint16_t &wLengthOut, T bufferOut[]);

    template <typename T>
    typename std::enable_if<is_big_endian<T>::value>::type
    vendorTransferImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const T bufferIn[], uint16_t &wLengthOut, T bufferOut[]);
};
