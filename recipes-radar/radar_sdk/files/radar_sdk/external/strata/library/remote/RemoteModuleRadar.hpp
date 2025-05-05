/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <modules/radar/ModuleRadar.hpp>

#include "RemoteVendorCommands.hpp"
#include <atomic>
#include <platform/interfaces/IBridge.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class RemoteModuleRadar :
    public ModuleRadar
{
public:
    RemoteModuleRadar(IBridge *bridge, uint8_t id);
    ~RemoteModuleRadar();

    //IModuleRadar
    void setCalibration(const IfxRsp_AntennaCalibration *c) override;
    const IfxRsp_AntennaCalibration *getCalibration() override;
    void setConfiguration(const IfxRfe_MmicConfig *c) override;
    const IfxRfe_MmicConfig *getConfiguration() override;
    void setSequence(const IfxRfe_Sequence *s) override;
    IfxRfe_Sequence *getSequence() override;
    void setProcessingStages(const IfxRsp_Stages *s) override;
    const IfxRsp_Stages *getProcessingStages() override;
    void getDataProperties(IDataProperties_t *props) override;
    void getRadarInfo(IProcessingRadarInput_t *info, const IDataProperties_t *dataProperties = nullptr) override;

    void startMeasurements(double measurementCycle) override;
    void stopMeasurements() override;
    void doMeasurement() override;
    void reset() override;
    void configure() override;

    void registerListener(IFrameListener<> *listener);

protected:
    IBridgeData *m_bridgeData;

private:
    /**
     * Read all data from the remote side to have it up-to-date
     */
    void readRemoteSetup();

    RemoteVendorCommands m_vendorCommands;

    std::atomic<bool> m_measurementStarted;
};
