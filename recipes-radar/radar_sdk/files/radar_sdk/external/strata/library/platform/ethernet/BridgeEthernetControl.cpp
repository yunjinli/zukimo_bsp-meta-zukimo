/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BridgeEthernetControl.hpp"

#include <common/Buffer.hpp>
#include <common/Logger.hpp>
#include <common/Serialization.hpp>
#include <common/crc/Crc16.hpp>
#include <platform/exception/EProtocol.hpp>
#include <platform/exception/EProtocolFunction.hpp>


namespace
{
    constexpr const uint16_t controlPort = 55055;

    constexpr const uint16_t defaultTimeout = 1000;
}


BridgeEthernetControl::BridgeEthernetControl(ISocket &socket, ipAddress_t ipAddr) :
    m_ipAddr {ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]},
    m_socket(socket)
{
    openConnection();
}

BridgeEthernetControl::~BridgeEthernetControl()
{
    closeConnection();
}

void BridgeEthernetControl::openConnection()
{
    m_socket.open(0, controlPort, m_ipAddr, defaultTimeout);
    m_commandError = false;
}

void BridgeEthernetControl::closeConnection()
{
    m_socket.close();
}

void BridgeEthernetControl::setDefaultTimeout()
{
    m_socket.setTimeout(defaultTimeout);
}

uint16_t BridgeEthernetControl::getMaxTransfer() const
{
    return m_socket.maxPayload() - m_commandHeaderSize;
}

void BridgeEthernetControl::sendRequest(uint8_t *packet, uint16_t sendSize, uint8_t bmReqType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[])
{
    switch (bmReqType)
    {
        case VENDOR_REQ_READ:
            static const auto receiveSize = m_responseHeaderSize + wLength;
            if (receiveSize > m_socket.maxPayload())
            {
                throw EProtocol("Read request length is too long, the maximum is ", getMaxTransfer());
            }
            break;
        case VENDOR_REQ_TRANSFER:
        case VENDOR_REQ_WRITE:
            if (sendSize > m_socket.maxPayload())
            {
                throw EProtocol("Request length is too long, the maximum is ", getMaxTransfer());
            }
            std::copy(buffer, buffer + wLength, &packet[8]);
            break;
        default:
            throw EProtocol("Invalid bmReqType", bmReqType);
    }

    packet[0] = bmReqType;
    packet[1] = bRequest;
    packet[2] = static_cast<uint8_t>(wValue);
    packet[3] = static_cast<uint8_t>(wValue >> 8);
    packet[4] = static_cast<uint8_t>(wIndex);
    packet[5] = static_cast<uint8_t>(wIndex >> 8);
    packet[6] = static_cast<uint8_t>(wLength);
    packet[7] = static_cast<uint8_t>(wLength >> 8);

    if (m_commandError)
    {
        m_socket.dumpPacket();
        m_commandError = false;
    }

    m_socket.send(packet, sendSize);
}

void BridgeEthernetControl::receiveResponse(uint8_t *packet, uint16_t receiveSize, uint8_t bmReqType, uint8_t bRequest, uint16_t &wLength, uint8_t buffer[])
{
    const uint16_t returnedLength = m_socket.receive(packet, receiveSize);
    const uint8_t &bmResType      = packet[0];
    const uint8_t &bStatus        = packet[1];

    if (!returnedLength)
    {
        m_commandError = true;
        throw EProtocol("No request response received", bRequest);
    }
    if (bmResType != bmReqType)
    {
        throw EProtocol("Request response type error", (bmResType << 8) | bmReqType);
    }
    if (bStatus)
    {
        throw EProtocolFunction(bStatus);
    }

    const auto maxLength = wLength;
    wLength              = serialToHost<uint16_t>(&packet[2]);
    if (bmReqType == VENDOR_REQ_READ)
    {
        if (wLength != maxLength)
        {
            throw EProtocol("Request response length error", (wLength << 16) | (bmReqType << 8) | bRequest);
        }
    }
    if (wLength > maxLength)
    {
        throw EProtocol("Request response too long for buffer", (wLength << 16) | (bmReqType << 8) | bRequest);
    }
    const auto payload = &packet[4];
    std::copy(payload, payload + wLength, buffer);
}

void BridgeEthernetControl::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[])
{
    std::lock_guard<std::mutex> lock(m_lock);

    const uint16_t sendSize    = m_commandHeaderSize + wLength;
    const uint16_t receiveSize = m_responseHeaderSize;
    stdext::buffer<uint8_t> packet(std::max(sendSize, receiveSize));

    sendRequest(packet.data(), sendSize, VENDOR_REQ_WRITE, bRequest, wValue, wIndex, wLength, buffer);
    wLength = 0;
    receiveResponse(packet.data(), receiveSize, VENDOR_REQ_WRITE, bRequest, wLength, nullptr);
}

void BridgeEthernetControl::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t buffer[])
{
    std::lock_guard<std::mutex> lock(m_lock);

    const uint16_t sendSize    = m_commandHeaderSize;
    const uint16_t receiveSize = m_responseHeaderSize + wLength;
    stdext::buffer<uint8_t> packet(std::max(sendSize, receiveSize));

    sendRequest(packet.data(), sendSize, VENDOR_REQ_READ, bRequest, wValue, wIndex, wLength, nullptr);
    receiveResponse(packet.data(), receiveSize, VENDOR_REQ_READ, bRequest, wLength, buffer);
}

void BridgeEthernetControl::vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[])
{
    std::lock_guard<std::mutex> lock(m_lock);

    const uint16_t sendSize    = m_commandHeaderSize + wLengthSend;
    const uint16_t receiveSize = m_responseHeaderSize + wLengthReceive;
    stdext::buffer<uint8_t> packet(std::max(sendSize, receiveSize));

    sendRequest(packet.data(), sendSize, VENDOR_REQ_TRANSFER, bRequest, wValue, wIndex, wLengthSend, bufferSend);
    receiveResponse(packet.data(), receiveSize, VENDOR_REQ_TRANSFER, bRequest, wLengthReceive, bufferReceive);
}
