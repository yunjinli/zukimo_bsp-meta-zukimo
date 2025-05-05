/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IRadar.hpp>
#include <modules/radar/ModuleRadar.hpp>

class ModuleRadarSingle :
    public ModuleRadar
{
public:
    static const IRadarConfig_t ModuleRadarSingleConfig[1];

    ModuleRadarSingle(IRadar *radar, IProcessingRadar *processing);

    IProcessingRadar *getIProcessingRadar(uint8_t index);
    IRadar *getIRadar(uint8_t index);

    void reset() override;
    void configure() override;
    void startMeasurements(double measurementCycle) override;
    void stopMeasurements() override;
    void doMeasurement() override;
    void run();

private:
    IRadar *m_radar;
    IProcessingRadar *m_processing;
};
