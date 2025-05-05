/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "IComponent.hpp"
#include "IRegisters.hpp"

class IPowerSupply : public IComponent
{
public:
    static constexpr uint8_t getType()
    {
        return COMPONENT_TYPE_POWER_SUPPLY;
    }
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_DEFAULT;
    }

    virtual ~IPowerSupply() = default;
};
