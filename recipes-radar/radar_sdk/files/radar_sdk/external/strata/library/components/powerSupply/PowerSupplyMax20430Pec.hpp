#pragma once

#include <components/Registers8bitPec.hpp>
#include <components/interfaces/IPowerSupplyMax20430Pec.hpp>


///
/// IPowerSupply implementation to support on-board PMICs (Power-Management-IC) which can be
/// configured via I2C
///
class PowerSupplyMax20430Pec :
    public IPowerSupplyMax20430Pec
{
public:
    PowerSupplyMax20430Pec(II2c *accessI2c, uint16_t devAddrI2c);

    ///
    /// @copydoc IPowerSupply::getIRegisters
    ///
    IRegisters<uint8_t> *getIRegisters() override;

private:
    Registers8bitPec m_registers;
};
