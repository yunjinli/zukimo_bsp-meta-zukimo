/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemotePinsLtr11.hpp"
#include "RemoteProtocolLtr11.hpp"
#include "RemoteRadar.hpp"
#include "RemoteRegisters.hpp"
#include <components/interfaces/IRadarLtr11.hpp>


class RemoteRadarLtr11 :
    public IRadarLtr11,
    public RemoteRadar
{
public:
    RemoteRadarLtr11(IVendorCommands *commands, uint8_t id);

    IRegisters<uint8_t, uint16_t> *getIRegisters() override;
    IPinsLtr11 *getIPinsLtr11() override;
    IProtocolLtr11 *getIProtocolLtr11() override;

private:
    RemoteVendorCommands m_vendorCommands;

    RemoteRegisters<uint8_t, uint16_t> m_registers;
    RemotePinsLtr11 m_pins;
    RemoteProtocolLtr11 m_commands;
};
