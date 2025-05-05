#include "PowerSupplyMax20430Pec.hpp"
#include "common/exception/EGenericException.hpp"

PowerSupplyMax20430Pec::PowerSupplyMax20430Pec(II2c *accessI2c, uint16_t devAddrI2c) :
    m_registers(accessI2c, devAddrI2c)
{
}

IRegisters<uint8_t> *PowerSupplyMax20430Pec::getIRegisters()
{
    return &m_registers;
}
