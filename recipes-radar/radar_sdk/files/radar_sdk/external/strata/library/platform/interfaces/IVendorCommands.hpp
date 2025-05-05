/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <Definitions.hpp>
#include <stdint.h>

class IVendorCommands
{
public:
    STRATA_API virtual ~IVendorCommands() = default;

    virtual void setDefaultTimeout()        = 0;
    virtual uint16_t getMaxTransfer() const = 0;

    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex)                                            = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[])  = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint16_t buffer[]) = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint32_t buffer[]) = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int8_t buffer[])   = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int16_t buffer[])  = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const int32_t buffer[])  = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const float buffer[])    = 0;
    virtual void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const double buffer[])   = 0;

    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t buffer[])  = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint16_t buffer[]) = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint32_t buffer[]) = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int8_t buffer[])   = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int16_t buffer[])  = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, int32_t buffer[])  = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, float buffer[])    = 0;
    virtual void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, double buffer[])   = 0;

    // The transfer function is used to write and read data with one single transaction.
    // There are 2 variants:
    // - Variable: wLengthReceive specifies the maximum length to be received and returnes the length actually received.
    // - Fixed: wLengthReceive specifies the exact length to receive. If a different length is received, it throws an error.
    virtual void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[])   = 0;
    virtual void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint16_t bufferSend[], uint16_t &wLengthReceive, uint16_t bufferReceive[]) = 0;
    virtual void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint32_t bufferSend[], uint16_t &wLengthReceive, uint32_t bufferReceive[]) = 0;

    virtual void vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[])   = 0;
    virtual void vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint16_t bufferSend[], uint16_t wLengthReceive, uint16_t bufferReceive[]) = 0;
    virtual void vendorTransferChecked(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint32_t bufferSend[], uint16_t wLengthReceive, uint32_t bufferReceive[]) = 0;

    // Interface for the component / module commands
    virtual void vendorWrite(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLength, const uint8_t buffer[])                                                                     = 0;
    virtual void vendorRead(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLength, uint8_t buffer[])                                                                            = 0;
    virtual void vendorTransfer(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[])       = 0;
    virtual void vendorTransferChecked(uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface, uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[]) = 0;
};
