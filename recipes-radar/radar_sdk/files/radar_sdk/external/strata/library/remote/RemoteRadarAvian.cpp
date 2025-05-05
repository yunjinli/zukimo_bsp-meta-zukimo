/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRadarAvian.hpp"

#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/protocol/protocol_definitions.h>


RemoteRadarAvian::RemoteRadarAvian(IVendorCommands *commands, uint8_t id) :
    RemoteRadar(commands, id),
    m_vendorCommands {commands, CMD_COMPONENT, getType(), getImplementation(), id, COMPONENT_SUBIF_DEFAULT},
    m_registers(sizeof(uint8_t), commands, getType(), getImplementation(), id),
    m_pins(commands, id),
    m_commands(commands, id)
{
}

IRegisters<uint8_t, uint32_t> *RemoteRadarAvian::getIRegisters()
{
    return &m_registers;
}

IPinsAvian *RemoteRadarAvian::getIPinsAvian()
{
    return &m_pins;
}

IProtocolAvian *RemoteRadarAvian::getIProtocolAvian()
{
    return &m_commands;
}
