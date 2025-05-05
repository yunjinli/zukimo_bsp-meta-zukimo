/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IPinsLtr11.hpp>
#include <components/interfaces/IProtocolLtr11.hpp>
#include <components/interfaces/IRadar.hpp>
#include <components/interfaces/IRegisters.hpp>

/**
 * \brief Access interface to a radar front end device of type Ltr11,
 * e.g. 
 */
class IRadarLtr11 :
    virtual public IRadar
{
public:
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_RADAR_LTR11;
    }

    virtual ~IRadarLtr11() = default;

    virtual IRegisters<uint8_t, uint16_t> *getIRegisters() = 0;
    virtual IPinsLtr11 *getIPinsLtr11()                    = 0;
    virtual IProtocolLtr11 *getIProtocolLtr11()            = 0;
};