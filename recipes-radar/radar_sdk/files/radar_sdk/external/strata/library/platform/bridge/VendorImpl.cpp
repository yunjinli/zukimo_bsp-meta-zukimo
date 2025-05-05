/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "VendorImpl.hpp"
#include <common/Buffer.hpp>
#include <platform/exception/EProtocol.hpp>
#include <universal/protocol/protocol_definitions.h>


void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex)
{
    vendorWrite(bRequest, wValue, wIndex, 0, static_cast<uint8_t *>(nullptr));
}

template <typename T>
typename std::enable_if<is_little_endian<T>::value>::type
VendorImpl::vendorWriteImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const T buffer[])
{
    vendorWrite(bRequest, wValue, wIndex, wLength, reinterpret_cast<const uint8_t *>(buffer));
}

template <typename T>
typename std::enable_if<is_big_endian<T>::value>::type
VendorImpl::vendorWriteImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const T buffer[])
{
    const auto count = wLength / sizeof(T);
    stdext::buffer<T> tmpBuf(count);
    hostToLittle(buffer, buffer + count, tmpBuf.data());
    vendorWrite(bRequest, wValue, wIndex, wLength, reinterpret_cast<const uint8_t *>(tmpBuf.data()));
}

template <typename T>
void VendorImpl::vendorReadImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, T buffer[])
{
    vendorRead(bRequest, wValue, wIndex, wLength, reinterpret_cast<uint8_t *>(buffer));

    // the following should be optimized away by the compiler if target architecture is little endian
    const auto count = wLength / sizeof(T);
    littleToHost(buffer, buffer + count);
}

template <typename T>
typename std::enable_if<is_little_endian<T>::value>::type
VendorImpl::vendorTransferImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const T bufferIn[], uint16_t &wLengthOut, T bufferOut[])
{
    vendorTransfer(bRequest, wValue, wIndex, wLengthIn, reinterpret_cast<const uint8_t *>(bufferIn), wLengthOut, reinterpret_cast<uint8_t *>(bufferOut));
}

template <typename T>
typename std::enable_if<is_big_endian<T>::value>::type
VendorImpl::vendorTransferImpl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const T bufferIn[], uint16_t &wLengthOut, T bufferOut[])
{
    const auto countIn = wLengthIn / sizeof(T);
    stdext::buffer<T> tmpBuf(countIn);
    hostToLittle(bufferIn, bufferIn + countIn, tmpBuf.data());
    vendorTransfer(bRequest, wValue, wIndex, wLengthIn, reinterpret_cast<const uint8_t *>(tmpBuf.data()), wLengthOut, reinterpret_cast<uint8_t *>(bufferOut));
    const auto countOut = wLengthOut / sizeof(T);
    littleToHost(bufferOut, bufferOut + countOut);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint16_t buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint32_t buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int8_t buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int16_t buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int32_t buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const float buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const double buffer[])
{
    vendorWriteImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint16_t buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint32_t buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int8_t buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int16_t buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int32_t buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, float buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, double buffer[])
{
    vendorReadImpl(bRequest, wValue, wIndex, wLength, buffer);
}

void VendorImpl::vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const uint16_t bufferIn[], uint16_t &wLengthOut, uint16_t bufferOut[])
{
    vendorTransferImpl(bRequest, wValue, wIndex, wLengthIn, bufferIn, wLengthOut, bufferOut);
}

void VendorImpl::vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthIn, const uint32_t bufferIn[], uint16_t &wLengthOut, uint32_t bufferOut[])
{
    vendorTransferImpl(bRequest, wValue, wIndex, wLengthIn, bufferIn, wLengthOut, bufferOut);
}

void VendorImpl::vendorWrite(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLength, const uint8_t buffer[])
{
    vendorWrite(bRequest, CMD_W_VALUE(bType, bImplementation), CMD_W_INDEX(bId, bSubInterface, bFunction), wLength, buffer);
}

void VendorImpl::vendorRead(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLength, uint8_t buffer[])
{
    vendorRead(bRequest, CMD_W_VALUE(bType, bImplementation), CMD_W_INDEX(bId, bSubInterface, bFunction), wLength, buffer);
}

void VendorImpl::vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[])
{
    const uint16_t expectedLength = wLengthReceive;
    vendorTransfer(bRequest, wValue, wIndex, wLengthSend, bufferSend, wLengthReceive, bufferReceive);
    if (wLengthReceive != expectedLength)
    {
        throw EProtocol("Unexpected transfer request response length", (expectedLength << 16) | wLengthReceive);
    }
}

void VendorImpl::vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint16_t bufferSend[], uint16_t wLengthReceive, uint16_t bufferReceive[])
{
    vendorTransferChecked(bRequest, wValue, wIndex, wLengthSend, reinterpret_cast<const uint8_t *>(bufferSend), wLengthReceive, reinterpret_cast<uint8_t *>(bufferReceive));
}

void VendorImpl::vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint32_t bufferSend[], uint16_t wLengthReceive, uint32_t bufferReceive[])
{
    vendorTransferChecked(bRequest, wValue, wIndex, wLengthSend, reinterpret_cast<const uint8_t *>(bufferSend), wLengthReceive, reinterpret_cast<uint8_t *>(bufferReceive));
}

void VendorImpl::vendorTransfer(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[])
{
    vendorTransfer(bRequest, CMD_W_VALUE(bType, bImplementation), CMD_W_INDEX(bId, bSubInterface, bFunction), wLengthSend, bufferSend, wLengthReceive, bufferReceive);
}

void VendorImpl::vendorTransferChecked(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[])
{
    vendorTransferChecked(bRequest, CMD_W_VALUE(bType, bImplementation), CMD_W_INDEX(bId, bSubInterface, bFunction), wLengthSend, bufferSend, wLengthReceive, bufferReceive);
}
