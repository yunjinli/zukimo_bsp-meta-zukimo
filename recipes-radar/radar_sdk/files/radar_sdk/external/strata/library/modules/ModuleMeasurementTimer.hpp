/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <modules/interfaces/IModuleMeasurementTimer.hpp>


class ModuleMeasurementTimer :
    public IModuleMeasurementTimer
{
public:
    ModuleMeasurementTimer();
    ~ModuleMeasurementTimer();

    void startMeasurementTimer(double interval) override;
    void stopMeasurementTimer() override;
};
