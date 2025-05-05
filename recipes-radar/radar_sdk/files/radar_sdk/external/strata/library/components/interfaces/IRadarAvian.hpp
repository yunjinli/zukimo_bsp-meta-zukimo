/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IPinsAvian.hpp>
#include <components/interfaces/IProtocolAvian.hpp>
#include <components/interfaces/IRadar.hpp>
#include <components/interfaces/IRegisters.hpp>


/**
 * \brief Access interface to a radar front end device of the Avian family,
 * e.g. BGT60TR13C, BGT60ATR24C, BGT60TR13D and BGT60TR12E.
 */
class IRadarAvian :
    virtual public IRadar
{
public:
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_RADAR_AVIAN;
    }

    virtual ~IRadarAvian() = default;

    virtual IRegisters<uint8_t, uint32_t> *getIRegisters() = 0;
    virtual IPinsAvian *getIPinsAvian()                    = 0;
    virtual IProtocolAvian *getIProtocolAvian()            = 0;
};
