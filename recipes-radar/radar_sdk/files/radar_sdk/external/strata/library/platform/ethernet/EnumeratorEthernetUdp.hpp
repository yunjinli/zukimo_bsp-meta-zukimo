/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/ethernet/SocketUdp.hpp>
#include <platform/interfaces/IEnumerator.hpp>


class EnumeratorEthernetUdp :
    public IEnumerator
{
protected:
    void enumerate(IEnumerationListener &listener, BoardData::const_iterator begin, BoardData::const_iterator end) override;

private:
    inline void sendBroadcast(const std::vector<remoteInfo_t> &broadcastAddresses);
    inline void getResponses(IEnumerationListener &listener, BoardData::const_iterator begin, BoardData::const_iterator end);

    SocketUdp m_socket;
};
