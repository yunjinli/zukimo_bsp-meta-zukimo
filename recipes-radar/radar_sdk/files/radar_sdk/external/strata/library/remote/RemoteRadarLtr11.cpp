/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRadarLtr11.hpp"

#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/protocol/protocol_definitions.h>


RemoteRadarLtr11::RemoteRadarLtr11(IVendorCommands *commands, uint8_t id) :
    RemoteRadar(commands, id),
    m_vendorCommands {commands, CMD_COMPONENT, getType(), getImplementation(), id, COMPONENT_SUBIF_DEFAULT},
    m_registers(1, commands, getType(), getImplementation(), id),
    m_pins(commands, id),
    m_commands(commands, id)
{
}

IRegisters<uint8_t, uint16_t> *RemoteRadarLtr11::getIRegisters()
{
    return &m_registers;
}

IPinsLtr11 *RemoteRadarLtr11::getIPinsLtr11()
{
    return &m_pins;
}

IProtocolLtr11 *RemoteRadarLtr11::getIProtocolLtr11()
{
    return &m_commands;
}
