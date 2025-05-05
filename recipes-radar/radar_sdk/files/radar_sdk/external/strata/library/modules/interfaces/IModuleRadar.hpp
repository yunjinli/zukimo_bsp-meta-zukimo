/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IProcessingRadar.hpp>
#include <modules/interfaces/IModule.hpp>
#include <universal/data_definitions.h>
#include <universal/types/IfxRfe_Types.h>
#include <universal/types/IfxRsp_Types.h>


class IModuleRadar :
    public IModule
{
public:
    static constexpr uint8_t getType()
    {
        return MODULE_TYPE_RADAR;
    }
    static constexpr uint8_t getImplementation()
    {
        return MODULE_IMPL_DEFAULT;
    }

    virtual ~IModuleRadar() = default;

    /**
     * @brief startMeasurements
     * @param measurementCycle time in seconds between measurements.
     * If 0 , measurements shall be manually trigger by calling doMeasurement().
     */
    virtual void startMeasurements(double measurementCycle) = 0;
    virtual void stopMeasurements()                         = 0;
    virtual void doMeasurement()                            = 0;
    virtual void reset()                                    = 0;
    virtual void configure()                                = 0;
    //  virtual void run() = 0;


    virtual void setCalibration(const IfxRsp_AntennaCalibration *c) = 0;
    virtual const IfxRsp_AntennaCalibration *getCalibration()       = 0;
    virtual void setConfiguration(const IfxRfe_MmicConfig *c)       = 0;
    virtual const IfxRfe_MmicConfig *getConfiguration()             = 0;
    virtual void setSequence(const IfxRfe_Sequence *s)              = 0;
    virtual const IfxRfe_Sequence *getSequence()                    = 0;
    virtual void setProcessingStages(const IfxRsp_Stages *s)        = 0;
    virtual const IfxRsp_Stages *getProcessingStages()              = 0;

    virtual void getDataProperties(IDataProperties_t *props)                                                    = 0;
    virtual void getRadarInfo(IProcessingRadarInput_t *info, const IDataProperties_t *dataProperties = nullptr) = 0;
};
