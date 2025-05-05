/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once


class IModuleMeasurementTimer
{
public:
    virtual ~IModuleMeasurementTimer() = default;

protected:
    //    virtual void run() = 0;

    virtual void startMeasurementTimer(double interval) = 0;
    virtual void stopMeasurementTimer()                 = 0;
};
