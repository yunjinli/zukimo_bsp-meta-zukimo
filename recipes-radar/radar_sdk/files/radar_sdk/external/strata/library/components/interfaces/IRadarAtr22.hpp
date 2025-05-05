/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IProtocolAtr22.hpp>
#include <components/interfaces/IRadar.hpp>
#include <components/interfaces/IRegisters.hpp>

/**
 * \brief Access interface to a radar front end device of the Atr22 family,
 * e.g. 
 */
class IRadarAtr22 :
    virtual public IRadar
{
public:
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_RADAR_ATR22;
    }

    virtual ~IRadarAtr22() = default;

    virtual IRegisters<uint16_t, uint16_t> *getIRegisters() = 0;
    virtual IProtocolAtr22 *getIProtocolAtr22()             = 0;
};
