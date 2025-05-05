/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "ModuleRadarSingle.hpp"
#include <stdexcept>


const IRadarConfig_t ModuleRadarSingle::ModuleRadarSingleConfig[1] = {{OPERATION_MODE_SINGLE, 0, 0}};


ModuleRadarSingle::ModuleRadarSingle(IRadar *radar, IProcessingRadar *processing) :
    m_radar {radar},
    m_processing {processing}
{
}

IProcessingRadar *ModuleRadarSingle::getIProcessingRadar(uint8_t index)
{
    if (index > 0)
    {
        throw std::out_of_range("index out of range");
    }
    return m_processing;
}

IRadar *ModuleRadarSingle::getIRadar(uint8_t index)
{
    if (index > 0)
    {
        throw std::out_of_range("index out of range");
    }
    return m_radar;
}

void ModuleRadarSingle::reset()
{
}

void ModuleRadarSingle::configure()
{
    //    const IfxRfe_MmicConfig *c = &this->m_config;
    //    IfxRfe_Sequence *s         = &this->m_sequence;
    //    IfxRsp_Stages *p           = &this->m_processingStages;

    /*
        m_radar.initialize();
        m_radar.loadSequence(&m_sequence);
        m_radar.configure(&m_config);

        const double dataRate = c->sampleRate * c->sampleWidth;
        if (dataRate >= 200.)
        {
            m_radar.LVDS_calib_start();
            try
            {
                m_data->calibrate(0);
            }
            catch (std::exception &)
            {
                // in case of exception, stop LVDS calibration from MMIC!
                m_radar.LVDS_calib_stop();
                throw;
            }
            m_radar.LVDS_calib_stop();
        }
        else
        {
            // just set state machine to LVDS calib done
    //        m_radar.getIRadar()->writeRadarRegister(0x420, 0x7);
        }

        m_radar.calibrate();

        m_radar.enableDataCrc(false);

        IDataSettings_t dataConfig;

    //    if (c->rxMask & ((1u << 3) | (1u << 2)))
    //    {
    //        dataConfig.EnabledChannels = 4;
    //    }
    //    else if (c->rxMask & (1u << 1))
    //    {
    //        dataConfig.EnabledChannels = 2;
    //    }
    //    else if (c->rxMask & (1u << 0))
    //    {
    //        dataConfig.EnabledChannels = 1;
    //    }
    //    else
    //    {
    //        dataConfig.EnabledChannels = 0;
    //    }

        dataConfig.lsbFirst = false;
        dataConfig.crcEnabled = false;
        dataConfig.crcMswFirst = false;
        //m_data->initializeData(radar->m_interface.m_config->dataId, &dataConfig);

        IDataProperties_t dataProperties;
        getDataProperties(&dataProperties);

        //m_accessData->configure(radar->m_interface.m_config->dataId, &dataProperties);

        IProcessingRadar_Descriptor pd;
        pd.dataSource = 0;
        pd.radarDataProperties = &dataProperties;
        pd.stages = p;

        m_processing->configure(&pd, 1);

        //tbd: check interfaces for necessary values
        */
}


void ModuleRadarSingle::run()
{
    m_processing->start();
    m_radar->startSequence();
}

void ModuleRadarSingle::startMeasurements(double measurementCycle)
{
    //    m_bridgeData->startStreaming();
    m_radar->startData();
    startMeasurementTimer(measurementCycle);
}

void ModuleRadarSingle::doMeasurement()
{
}

void ModuleRadarSingle::stopMeasurements()
{
    //    this->m_bridgeData->stopStreaming();
    m_radar->stopData();
    stopMeasurementTimer();
}
