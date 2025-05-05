/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <modules/ModuleMeasurementTimer.hpp>
#include <modules/interfaces/IModuleRadar.hpp>
#include <platform/interfaces/IBridge.hpp>

#include <vector>

class ModuleRadar :
    public IModuleRadar,
    public ModuleMeasurementTimer
{
public:
    //IModuleRadar
    void setCalibration(const IfxRsp_AntennaCalibration *c) override;
    const IfxRsp_AntennaCalibration *getCalibration() override;
    void setConfiguration(const IfxRfe_MmicConfig *c) override;
    const IfxRfe_MmicConfig *getConfiguration() override;
    void setSequence(const IfxRfe_Sequence *s) override;
    const IfxRfe_Sequence *getSequence() override;
    void setProcessingStages(const IfxRsp_Stages *s) override;
    const IfxRsp_Stages *getProcessingStages() override;

    void getDataProperties(IDataProperties_t *props) override;
    void getRadarInfo(IProcessingRadarInput_t *info, const IDataProperties_t *dataProperties = nullptr) override;

protected:
    uint32_t calculateFrameSize();

    IfxRfe_MmicConfig m_config;
    IfxRfe_Sequence m_sequence;
    std::vector<IfxRfe_Ramp> m_ramps;
    IfxRsp_Stages m_processingStages;
    IfxRsp_AntennaCalibration m_antennaCalibration[2];
};
