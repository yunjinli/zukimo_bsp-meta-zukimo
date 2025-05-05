/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "ModuleRadarCascaded.hpp"
#include <stdexcept>


ModuleRadarCascaded::ModuleRadarCascaded(IRadar *radar0, IRadar *radar1, IProcessingRadar *processing0, IProcessingRadar *processing1) :
    m_radar {radar0, radar1},
    m_processing {processing0, processing1}
{
}

IProcessingRadar *ModuleRadarCascaded::getIProcessingRadar(uint8_t index)
{
    if (index > 0)
    {
        throw std::out_of_range("index out of range");
    }
    return m_processing[index];
}

IRadar *ModuleRadarCascaded::getIRadar(uint8_t index)
{
    if (index > 0)
    {
        throw std::out_of_range("index out of range");
    }
    return m_radar[index];
}

void ModuleRadarCascaded::configure()
{
}

void ModuleRadarCascaded::reset()
{
}

void ModuleRadarCascaded::run()
{
}

void ModuleRadarCascaded::startMeasurements(double /*measurementCycle*/)
{
}

void ModuleRadarCascaded::stopMeasurements()
{
}

void ModuleRadarCascaded::doMeasurement()
{
}
