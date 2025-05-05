/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteProtocolLtr11.hpp"

#include <algorithm>
#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/iprotocol.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>


RemoteProtocolLtr11::RemoteProtocolLtr11(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_LTR11, id, COMPONENT_SUBIF_PROTOCOL}
{
}

void RemoteProtocolLtr11::executeWrite(const Write &command)
{
    m_vendorCommands.vendorWrite(FN_PROTOCOL_EXECUTE, sizeof(command), command);
}

void RemoteProtocolLtr11::executeRead(const Read &command, uint16_t &value)
{
    const uint8_t &cmd = command;

    m_vendorCommands.vendorTransferChecked(FN_PROTOCOL_EXECUTE, sizeof(cmd), &cmd, sizeof(value), reinterpret_cast<uint8_t *>(&value));
}

void RemoteProtocolLtr11::setBits(uint8_t address, uint16_t bitMask)
{
    constexpr uint16_t wLength = sizeof(address) + sizeof(bitMask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, address);
    it          = hostToSerial(it, bitMask);

    m_vendorCommands.vendorWrite(FN_PROTOCOL_SET_BITS, wLength, payload);
}

void RemoteProtocolLtr11::executeWriteBatch(const Write commands[], uint16_t count)
{
    m_vendorCommands.vendorWrite(FN_PROTOCOL_EXECUTE_HELPER, count * sizeof(*commands), *commands);
}

void RemoteProtocolLtr11::executeWriteBurst(const WriteBurst &command, uint16_t count, const WriteValue values[])
{
    WriteBurst cmd = command;  // create a local copy of the command to be able to increment it for multi-part transfers

    constexpr uint16_t elemSize    = sizeof(values[0]);
    constexpr uint16_t argSize     = sizeof(cmd);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;
    const decltype(count) wCount   = std::min(count, maxCount);

    while (count > 0)
    {
        const uint16_t length  = wCount * elemSize;
        const uint16_t wLength = argSize + length;
        stdext::buffer<uint8_t> payload(wLength);
        uint8_t *it = payload.data();
        it          = hostToSerial(it, cmd);
        it          = hostToSerial(it, values, values + wCount);

        m_vendorCommands.vendorWrite(FN_PROTOCOL_EXECUTE, wLength, payload.data());
        cmd.increment(wCount);
        values += wCount;
        count -= wCount;
    }
}

void RemoteProtocolLtr11::executeReadBurst(const ReadBurst &command, uint16_t count, uint16_t values[])
{
    ReadBurst cmd = command;  // create a local copy of the command to be able to increment it for multi-part transfers

    constexpr uint16_t elemSize    = sizeof(values[0]);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;
    const decltype(count) wCount   = std::min(count, maxCount);

    while (count > 0)
    {
        const uint16_t wLengthReceive = wCount * elemSize;
        const uint16_t wLengthSend    = sizeof(cmd) + sizeof(wCount);
        uint8_t payload[wLengthSend];
        uint8_t *it = payload;
        it          = hostToSerial(it, cmd);
        it          = hostToSerial(it, wCount);

        m_vendorCommands.vendorTransferChecked(FN_PROTOCOL_EXECUTE, wLengthSend, payload, wLengthReceive, reinterpret_cast<uint8_t *>(values));

        cmd.increment(wCount);
        values += wCount;
        count -= wCount;
    }
}

void RemoteProtocolLtr11::setMisoArbitration(uint16_t prt)
{
    const uint16_t wLength = sizeof(prt);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, prt);

    m_vendorCommands.vendorWrite(FN_PROTOCOL_SETTING, wLength, payload);
}
