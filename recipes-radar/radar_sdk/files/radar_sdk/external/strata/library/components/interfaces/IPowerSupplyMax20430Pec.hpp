/**
 * @copyright 2022 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "IPowerSupply.hpp"

///
/// Interface for Max20430Pec power supply
///
class IPowerSupplyMax20430Pec : public IPowerSupply
{
public:
    virtual ~IPowerSupplyMax20430Pec() = default;

    virtual IRegisters<uint8_t> *getIRegisters() = 0;
};
