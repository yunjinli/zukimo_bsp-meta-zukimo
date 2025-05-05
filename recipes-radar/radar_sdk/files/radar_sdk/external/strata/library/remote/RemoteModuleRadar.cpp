/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteModuleRadar.hpp"

#include <common/Buffer.hpp>
#include <components/radar/TypeSerialization.hpp>
#include <platform/bridge/BridgeProtocol.hpp>
#include <platform/exception/EProtocol.hpp>
#include <universal/modules/subinterfaces.h>
#include <universal/modules/types.h>
#include <universal/modules/types/imoduleradar.h>
#include <universal/protocol/protocol_definitions.h>


namespace
{
    constexpr const uint32_t frameQueueCount = 50;
}

RemoteModuleRadar::RemoteModuleRadar(IBridge *bridge, uint8_t id) :
    m_bridgeData {bridge->getIBridgeData()},
    m_vendorCommands {bridge->getIBridgeControl()->getIVendorCommands(), CMD_MODULE, getType(), MODULE_IMPL_DEFAULT, id, MODULE_SUBIF_DEFAULT},
    m_measurementStarted {false}
{
    if (bridge->getIBridgeControl()->getIVendorCommands() == nullptr)
    {
        throw EProtocol("IVendorCommands not implemented");
    }
}

RemoteModuleRadar::~RemoteModuleRadar()
{
    if (m_measurementStarted)
    {
        try
        {
            RemoteModuleRadar::stopMeasurements();
        }
        catch (...)
        {
        }
    }
}

void RemoteModuleRadar::setCalibration(const IfxRsp_AntennaCalibration *c)
{
    constexpr size_t size     = serialized_sizeof(IfxRsp_AntennaCalibration());
    constexpr uint16_t length = 2 * size;
    uint8_t buf[length];

    uint8_t *it = buf;
    it          = hostToSerial(it, &c[0]);
    it          = hostToSerial(it, &c[1]);

    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_SET_CALIBRATION, length, buf);

    ModuleRadar::setCalibration(c);
}

const IfxRsp_AntennaCalibration *RemoteModuleRadar::getCalibration()
{
    m_vendorCommands.vendorReadToStruct(FN_MODULE_RADAR_GET_CALIBRATION, m_antennaCalibration, 2);
    return m_antennaCalibration;
}

void RemoteModuleRadar::setConfiguration(const IfxRfe_MmicConfig *c)
{
    constexpr size_t size = serialized_sizeof(IfxRfe_MmicConfig());
    uint8_t buf[size];

    hostToSerial(buf, c);

    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_SET_CONFIG, size, buf);

    ModuleRadar::setConfiguration(c);
}

const IfxRfe_MmicConfig *RemoteModuleRadar::getConfiguration()
{
    m_vendorCommands.vendorReadToStruct(FN_MODULE_RADAR_GET_CONFIG, &m_config);
    return &m_config;
}

void RemoteModuleRadar::setSequence(const IfxRfe_Sequence *s)
{
    const size_t size = serialized_sizeof(s, s->rampCount);
    stdext::buffer<uint8_t> buf(size);

    uint8_t *it = buf.data();
    it          = hostToSerial(it, s);
    it          = hostToSerial(it, s->ramps, s->rampCount);

    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_SET_SEQUENCE, static_cast<uint16_t>(size), buf.data());

    ModuleRadar::setSequence(s);
}

IfxRfe_Sequence *RemoteModuleRadar::getSequence()
{
    constexpr uint8_t maxRamps = 8;
    constexpr size_t size      = serialized_sizeof(IfxRfe_Sequence(), maxRamps);
    uint8_t buf[size];

    uint16_t wLengthReceive = size;
    m_vendorCommands.vendorTransfer(FN_MODULE_RADAR_GET_SEQUENCE, 0, nullptr, wLengthReceive, buf);

    const uint8_t *it = serialToHost(buf, &m_sequence);

    const auto correctLength = serialized_sizeof(IfxRfe_Sequence(), m_sequence.rampCount);
    if (wLengthReceive != correctLength)
    {
        throw EProtocol("RemoteModuleRadar - rampCount of sequence does not match received data length");
    }

    m_ramps.resize(m_sequence.rampCount);
    m_sequence.ramps = m_ramps.data();
    serialToHost(it, m_sequence.ramps, m_sequence.rampCount);

    return &m_sequence;
}

void RemoteModuleRadar::setProcessingStages(const IfxRsp_Stages *s)
{
    constexpr size_t size = serialized_sizeof(IfxRsp_Stages());
    uint8_t buf[size];

    hostToSerial(buf, s);

    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_SET_PROCESSING_STAGES, size, buf);

    ModuleRadar::setProcessingStages(s);
}

const IfxRsp_Stages *RemoteModuleRadar::getProcessingStages()
{
    m_vendorCommands.vendorReadToStruct(FN_MODULE_RADAR_GET_PROCESSING_STAGES, &m_processingStages);
    return &m_processingStages;
}

void RemoteModuleRadar::getDataProperties(IDataProperties_t *props)
{
    m_vendorCommands.vendorReadToStruct(FN_MODULE_RADAR_GET_DATA_PROPERTIES, props);
}

void RemoteModuleRadar::getRadarInfo(IProcessingRadarInput_t *info, const IDataProperties_t *dataProperties)
{
    if (dataProperties)
    {
        throw EProtocol("RemoteModuleRadar does not implement optional dataProperties parameter");
    }
    m_vendorCommands.vendorReadToStruct(FN_MODULE_RADAR_GET_RADAR_INFO, info);
}

void RemoteModuleRadar::reset()
{
    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_RESET, 0, nullptr);
}

void RemoteModuleRadar::configure()
{
    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_CONFIGURE, 0, nullptr);

    readRemoteSetup();  //read all data from remote side to use it for the following frame size calculation

    m_bridgeData->setFrameBufferSize(calculateFrameSize());
    m_bridgeData->setFrameQueueSize(frameQueueCount);
}

void RemoteModuleRadar::registerListener(IFrameListener<> *listener)
{
    m_bridgeData->registerListener(listener);
}

void RemoteModuleRadar::startMeasurements(double measurementCycle)
{
    m_bridgeData->startStreaming();

    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_START_MEASUREMENT, sizeof(measurementCycle), reinterpret_cast<uint8_t *>(&measurementCycle));

    m_measurementStarted = true;
}

void RemoteModuleRadar::doMeasurement()
{
    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_DO_MEASUREMENT, 0, nullptr);
}

void RemoteModuleRadar::stopMeasurements()
{
    m_bridgeData->stopStreaming();
    m_vendorCommands.vendorWrite(FN_MODULE_RADAR_STOP_MEASUREMENT, 0, nullptr);

    m_measurementStarted = false;
}

void RemoteModuleRadar::readRemoteSetup()
{
    getConfiguration();
    getSequence();
    getProcessingStages();
}
