/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/BoardDescriptor.hpp>
#include <platform/interfaces/link/ISocket.hpp>  // ipAddress_t


class BoardDescriptorUdp :
    public BoardDescriptor
{
public:
    BoardDescriptorUdp(const BoardData &data, const char name[], ipAddress_t ipAddr) :
        BoardDescriptor(data, name),
        m_identifier {ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]}
    {}

    std::shared_ptr<IBridge> createBridge() override;

private:
    ipAddress_t m_identifier;
};


class BoardEthernetUdp
{
public:
    static std::unique_ptr<BoardDescriptor> searchBoard(ipAddress_t ipAddr, BoardData::const_iterator begin, BoardData::const_iterator end);
    static std::unique_ptr<BoardInstance> createBoardInstance(ipAddress_t ipAddr);
};
