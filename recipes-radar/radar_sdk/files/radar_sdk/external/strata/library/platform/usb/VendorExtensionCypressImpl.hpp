/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <mutex>

#include <common/Buffer.hpp>
#include <platform/bridge/VendorImpl.hpp>
#include <platform/interfaces/link/IUvcExtension.hpp>

#include <universal/uvc_definitions.h>


class VendorExtensionCypressImpl :
    public VendorImpl
{
private:
    constexpr static const uint16_t m_maxPayload = 4096;

public:
    VendorExtensionCypressImpl(IUvcExtension *uvcExtension);

    void setDefaultTimeout() override;
    uint16_t getMaxTransfer() const override;

    // IBridgeProtocol implementation
    using VendorImpl::vendorRead;
    using VendorImpl::vendorTransfer;
    using VendorImpl::vendorWrite;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t buffer[]) override;
    void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[]) override;

    // BridgeProtocol override
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex) override;

private:
    void sendRequest(uint8_t bmReqType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[]);
    uint16_t checkStatus();

private:
    // Vendor extension access
    IUvcExtension *m_uvcExtension;

    void getDataProperty(uint16_t length, uint8_t buffer[]);
    bool m_variableSizeSupport;
    stdext::buffer<uint8_t> m_fixedBuffer;
};
