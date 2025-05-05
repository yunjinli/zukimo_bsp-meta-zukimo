/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemotePinsAvian.hpp"
#include "RemoteProtocolAvian.hpp"
#include "RemoteRadar.hpp"
#include "RemoteRegisters.hpp"
#include <components/interfaces/IRadarAvian.hpp>


class RemoteRadarAvian :
    public IRadarAvian,
    public RemoteRadar
{
public:
    RemoteRadarAvian(IVendorCommands *commands, uint8_t id);

    IRegisters<uint8_t, uint32_t> *getIRegisters() override;
    IPinsAvian *getIPinsAvian() override;
    IProtocolAvian *getIProtocolAvian() override;

private:
    RemoteVendorCommands m_vendorCommands;

    RemoteRegisters<uint8_t, uint32_t> m_registers;
    RemotePinsAvian m_pins;
    RemoteProtocolAvian m_commands;
};
