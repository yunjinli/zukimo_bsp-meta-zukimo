/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/bridge/VendorImpl.hpp>
#include <platform/interfaces/link/ISocket.hpp>

#include <mutex>


class BridgeEthernetControl :
    public VendorImpl
{
    friend class BoardDescriptorEthernet;

public:
    BridgeEthernetControl(ISocket &socket, ipAddress_t ipAddr);
    ~BridgeEthernetControl() override;

    void openConnection();
    void closeConnection();

    // IVendorCommands implementation
    void setDefaultTimeout() override;
    uint16_t getMaxTransfer() const override;

    using VendorImpl::vendorRead;
    using VendorImpl::vendorTransfer;
    using VendorImpl::vendorWrite;
    void vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[]) override;
    void vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t buffer[]) override;
    void vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[]) override;

protected:
    uint8_t m_ipAddr[4];

private:
    void sendRequest(uint8_t *packet, uint16_t sendSize, uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[]);
    void receiveResponse(uint8_t *packet, uint16_t receiveSize, uint8_t bRequestType, uint8_t bRequest, uint16_t &wLength, uint8_t buffer[]);

    ISocket &m_socket;

    std::mutex m_lock;
    bool m_commandError;
};
