/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteProtocolAvian.hpp"

#include <algorithm>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/iprotocol.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>


RemoteProtocolAvian::RemoteProtocolAvian(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_AVIAN, id, COMPONENT_SUBIF_PROTOCOL}
{
}

void RemoteProtocolAvian::execute(const Command commands[], uint32_t count, uint32_t results[])
{
    constexpr uint16_t elemSize    = sizeof(*commands);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;

    while (count > 0)
    {
        const decltype(count) wCount = std::min(count, maxCount);
        const uint16_t wLength       = wCount * elemSize;

        if (results)
        {
            m_vendorCommands.vendorTransferChecked(FN_PROTOCOL_EXECUTE, wLength, *commands, wLength, reinterpret_cast<uint8_t *>(results));
            results += wCount;
        }
        else
        {
            m_vendorCommands.vendorWrite(FN_PROTOCOL_EXECUTE, wLength, *commands);
        }

        commands += wCount;
        count -= wCount;
    }
}

void RemoteProtocolAvian::setBits(uint8_t address, uint32_t bitMask)
{
    const uint32_t payload = (address << 24) | (bitMask & 0xFFFFFF);
    m_vendorCommands.vendorWrite(FN_PROTOCOL_SET_BITS, sizeof(payload), reinterpret_cast<const uint8_t *>(&payload));
}
