/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "EnumeratorEthernetUdp.hpp"

#include <common/Logger.hpp>
#include <common/Serialization.hpp>
#include <platform/ethernet/BoardEthernetUdp.hpp>
#include <platform/exception/EConnection.hpp>
#include <platform/exception/EProtocol.hpp>
#include <platform/templates/identifyBoardFunction.hpp>
#include <universal/protocol/protocol_definitions.h>


namespace
{
    const std::vector<remoteInfo_t> defaultBroadcastAddresses {
        {{169, 254, 255, 255}, 55055},
        {{169, 254, 1, 255}, 55055},
    };

    constexpr const uint16_t timeout     = 200;
    constexpr const uint16_t timeoutStep = 100;

    constexpr const uint16_t m_commandHeaderSize  = 8;
    constexpr const uint16_t m_responseHeaderSize = 4;

    constexpr const uint8_t bRequest = REQ_BOARD_INFO;
    constexpr const uint16_t wValue  = 0;
    constexpr const uint16_t wIndex  = 0;
    constexpr const uint16_t wLength = 32;
}


void EnumeratorEthernetUdp::enumerate(IEnumerationListener &listener, BoardData::const_iterator begin, BoardData::const_iterator end)
{
    std::vector<remoteInfo_t> broadcastAddresses;
    m_socket.getBroadcastAddresses(broadcastAddresses);

    m_socket.open(0, 0, nullptr, timeoutStep);
    m_socket.setBroadcast(true);

    if (broadcastAddresses.empty())
    {
        LOG(DEBUG) << "Could not get broadcast addresses, using default";
        sendBroadcast(defaultBroadcastAddresses);
    }
    else
    {
        sendBroadcast(broadcastAddresses);
    }

    getResponses(listener, begin, end);

    m_socket.close();
}

inline void EnumeratorEthernetUdp::sendBroadcast(const std::vector<remoteInfo_t> &broadcastAddresses)
{
    uint8_t packet[m_commandHeaderSize];
    packet[0] = VENDOR_REQ_READ;
    packet[1] = bRequest;
    packet[2] = static_cast<uint8_t>(wValue);
    packet[3] = static_cast<uint8_t>(wValue >> 8);
    packet[4] = static_cast<uint8_t>(wIndex);
    packet[5] = static_cast<uint8_t>(wIndex >> 8);
    packet[6] = static_cast<uint8_t>(wLength);
    packet[7] = static_cast<uint8_t>(wLength >> 8);

    for (auto &b : broadcastAddresses)
    {
        LOG(DEBUG) << "Looking for boards under " << std::dec
                   << static_cast<int>(b.ip[0]) << "."
                   << static_cast<int>(b.ip[1]) << "."
                   << static_cast<int>(b.ip[2]) << "."
                   << static_cast<int>(b.ip[3]) << " over UDP ...";
        try
        {
            m_socket.sendTo(&packet[0], m_commandHeaderSize, &b);
        }
        catch (...)
        {}
    }
}

inline void EnumeratorEthernetUdp::getResponses(IEnumerationListener &listener, BoardData::const_iterator begin, BoardData::const_iterator end)
{
    const auto expiry = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);

    while (std::chrono::steady_clock::now() < expiry)
    {
        constexpr const uint16_t receiveSize = m_responseHeaderSize + wLength;
        uint8_t packet[receiveSize];

        uint16_t ids[2];
        remoteInfo_t remote;

        try
        {
            uint16_t returnedLength  = m_socket.receiveFrom(&packet[0], receiveSize, &remote);
            const uint8_t &bmResType = packet[0];
            const uint8_t &bStatus   = packet[1];

            if (!returnedLength)
            {
                continue;
            }
            if ((bmResType != VENDOR_REQ_READ) || bStatus)
            {
                throw EProtocol("Read command response status error", (bmResType << 8) | bStatus);
            }
            if (returnedLength != receiveSize)
            {
                throw EProtocol("Read command response incomplete", (returnedLength << 16) | (bmResType << 8) | bStatus);
            }
            const uint16_t responseLength = static_cast<uint16_t>(packet[3] << 8) | packet[2];
            if (responseLength != wLength)
            {
                throw EProtocol("Read command response length error", packet[1]);
            }

            const uint8_t *it       = packet + m_responseHeaderSize;
            it                      = serialToHost(it, ids);
            const char *cName       = reinterpret_cast<const char *>(it);
            packet[receiveSize - 1] = 0;  // make sure string is terminated to avoid memory leak

            LOG(DEBUG) << "... found board at " << std::dec
                       << static_cast<int>(remote.ip[0]) << "."
                       << static_cast<int>(remote.ip[1]) << "."
                       << static_cast<int>(remote.ip[2]) << "."
                       << static_cast<int>(remote.ip[3]);

            const uint16_t &VID = ids[0], &PID = ids[1];
            LOG(DEBUG) << "... VID = " << std::hex << VID << " ; PID = " << PID << " ; name = \"" << cName << "\"";

            auto descriptor = identifyBoardFunction<BoardDescriptorUdp>(ids, cName, begin, end, remote.ip);
            if (listener.onEnumerate(std::move(descriptor)))
            {
                break;
            }
        }
        catch (const EException &e)
        {
            LOG(DEBUG) << "... handled " << e.what();
        }
    }
}
