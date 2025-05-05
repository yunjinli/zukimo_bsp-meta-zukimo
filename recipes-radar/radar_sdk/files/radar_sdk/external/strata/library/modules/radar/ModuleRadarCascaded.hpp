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

class ModuleRadarCascaded :
    public ModuleRadar
{
public:
    const IRadarConfig_t ModuleRadarCascadedConfig[2] =
        {
            {OPERATION_MODE_MASTER_LOOUT_TX1, 0, 0},  // Master
            {OPERATION_MODE_SLAVE_LOIN_TX3, 2, 4},    // Slave
        };

    ModuleRadarCascaded(IRadar *radar0, IRadar *radar1, IProcessingRadar *processing0, IProcessingRadar *processing1);

    IProcessingRadar *getIProcessingRadar(uint8_t index);
    IRadar *getIRadar(uint8_t index);

    void reset() override;
    void configure() override;
    void startMeasurements(double measurementCycle) override;
    void stopMeasurements() override;
    void doMeasurement() override;
    void run();

private:
    IRadar *m_radar[2];
    IProcessingRadar *m_processing[2];
};
