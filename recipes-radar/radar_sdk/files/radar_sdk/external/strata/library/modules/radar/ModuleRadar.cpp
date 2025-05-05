/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "ModuleRadar.hpp"

#include <common/BinUtils.hpp>
#include <common/exception/EGenericException.hpp>
#include <cstring>


#define M_C0 299792458.0  //Speed of light in vacuum (m/s)

#define IfxRsp_Modulation_Tdm     0
#define IfxRsp_Modulation_Cdm     1
#define IfxRsp_Modulation_Unknown 2

namespace
{
    constexpr const uint32_t frameSize = 4 * 1024 * 1024;
}

//ModuleRadarBase::ModuleRadarBase(IBridge *bridge) :
//    m_bridgeData {bridge->getIBridgeData()}
//{
//    m_bridgeData->setBufferPool(frameSize, framePoolCount);
//}

void ModuleRadar::setCalibration(const IfxRsp_AntennaCalibration *c)
{
    memcpy(m_antennaCalibration, c, sizeof(m_antennaCalibration));
}

const IfxRsp_AntennaCalibration *ModuleRadar::getCalibration()
{
    return m_antennaCalibration;
}

void ModuleRadar::setConfiguration(const IfxRfe_MmicConfig *c)
{
    memcpy(&m_config, c, sizeof(m_config));
}

const IfxRfe_MmicConfig *ModuleRadar::getConfiguration()
{
    return &m_config;
}

void ModuleRadar::setSequence(const IfxRfe_Sequence *s)
{
    memcpy(&m_sequence, s, sizeof(m_sequence));
}

const IfxRfe_Sequence *ModuleRadar::getSequence()
{
    return &m_sequence;
}

void ModuleRadar::setProcessingStages(const IfxRsp_Stages *s)
{
    memcpy(&m_processingStages, s, sizeof(m_processingStages));
}

const IfxRsp_Stages *ModuleRadar::getProcessingStages()
{
    return &m_processingStages;
}


static uint8_t getModulation(const IfxRfe_Sequence *s)
{
    if (s->rampCount > 0)
    {
        const unsigned int count = getBitCount(s->ramps[0].txMask);
        uint_fast8_t i;
        for (i = 1; i < s->rampCount; i++)
        {
            if (count != getBitCount(s->ramps[i].txMask))
            {
                break;
            }
        }
        if (i == s->rampCount)  // for loop did not break
        {
            if (count == 1)
            {
                return IfxRsp_Modulation_Tdm;
            }
            else
            {
                return IfxRsp_Modulation_Cdm;
            }
        }
    }

    return IfxRsp_Modulation_Unknown;
}

void ModuleRadar::getDataProperties(IDataProperties_t *info)
{
    const IfxRfe_MmicConfig *c = this->getConfiguration();
    const IfxRfe_Sequence *s   = this->getSequence();

    info->format = DataFormat_Q15;

    info->bitWidth = c->sampleWidth;
    info->samples  = static_cast<uint16_t>((s->tRamp - s->tRampStartDelay + s->tRampStopDelay) * c->sampleRate);

    uint16_t ramps = 0;
    for (uint_fast8_t i = 0; i < s->rampCount; i++)
    {
        ramps += s->ramps[i].loops;
    }
    info->ramps = s->loops * ramps;

    info->rxChannels = static_cast<uint8_t>(getBitCount(s->rxMask));
}

void ModuleRadar::getRadarInfo(IProcessingRadarInput_t *info, const IDataProperties_t *dataProperties)
{
    const IfxRfe_MmicConfig *c = this->getConfiguration();
    const IfxRfe_Sequence *s   = this->getSequence();

    uint8_t rxChannels;
    uint16_t ramps;
    if (dataProperties)
    {
        rxChannels = dataProperties->rxChannels;
        ramps      = dataProperties->ramps;
    }
    else
    {
        IDataProperties_t dp;
        getDataProperties(&dp);
        rxChannels = dp.rxChannels;
        ramps      = dp.ramps;
    }

    uint16_t combinedTxMask = 0x0000;
    for (uint_fast8_t i = 0; i < s->rampCount; i++)
    {
        combinedTxMask |= s->ramps[i].txMask;
    }
    info->txChannels = getBitCount(combinedTxMask);

    double tRepetition = s->tRamp + s->tWait + s->tJump;

    uint8_t modulation = getModulation(s);
    if ((modulation == IfxRsp_Modulation_Tdm) && (info->txChannels > 0))
    {
        tRepetition *= info->txChannels;
        info->virtualAnt = rxChannels * info->txChannels;
        info->rampsPerTx = ramps / info->txChannels;
    }
    else
    {
        info->virtualAnt = rxChannels;
        info->rampsPerTx = ramps;
    }

    double tRamp  = s->tRamp;
    double fDelta = s->ramps[0].fDelta;
    if (fDelta < 0)
    {
        fDelta = -fDelta;
    }
    info->maxRange    = static_cast<float>(M_C0 / 4 * c->sampleRate * tRamp / fDelta);
    double fCenter    = s->ramps[0].fStart + s->ramps[0].fDelta / 2;
    info->maxVelocity = static_cast<float>(M_C0 / (4 * fCenter * tRepetition));
}

uint32_t ModuleRadar::calculateFrameSize()
{
    //ToDo: Calculate the actual frame size out of the configured data
    return frameSize;
}
