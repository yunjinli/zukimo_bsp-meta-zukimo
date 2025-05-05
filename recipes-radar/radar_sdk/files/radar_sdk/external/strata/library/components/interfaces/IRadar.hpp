/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "IComponent.hpp"
#include <universal/components/implementations/radar.h>
#include <universal/types/IfxRfe_Types.h>

#define OPERATION_MODE_SINGLE               0x00
#define OPERATION_MODE_SLAVE_LOIN_TX1       0x02  // use TX2 and TX3
#define OPERATION_MODE_SLAVE_LOIN_TX3       0x03  // use TX1 and TX2
#define OPERATION_MODE_MASTER_LOOUT_TX1     0x10  // use TX2 and TX3
#define OPERATION_MODE_MASTER_LOOUT_TX2     0x20  // use TX1 and TX3
#define OPERATION_MODE_MASTER_LOOUT_TX3     0x30  // use TX1 and TX2
#define OPERATION_MODE_MASTER_LOOUT_TX1_TX3 0x40  // disable TX2

typedef struct
{
    uint8_t devId;            ///< control interface device id
    uint8_t dataIndex;        ///< data interface index
    uint8_t channelSwapping;  ///< swapping mode for receive channels
} BoardRadarConfig_t;

typedef struct
{
    uint8_t operationMode;    // see defines
    uint8_t txChannelOffset;  // Logical offset for TX1
    uint8_t rxChannelOffset;  // Logical offset for RX1
} IRadarConfig_t;


class IRadar :
    public IComponent
{
public:
    static constexpr uint8_t getType()
    {
        return COMPONENT_TYPE_RADAR;
    }
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_DEFAULT;
    }

    virtual ~IRadar() = default;

    virtual void initialize()                                                = 0;
    virtual void reset(bool softReset = true)                                = 0;
    virtual void configure(const IfxRfe_MmicConfig *c)                       = 0;
    virtual void loadSequence(IfxRfe_Sequence *s)                            = 0;
    virtual void calibrate()                                                 = 0;
    virtual void startSequence()                                             = 0;
    virtual void startData()                                                 = 0;
    virtual void stopData()                                                  = 0;
    virtual uint8_t getDataIndex()                                           = 0;
    virtual void enableConstantFrequencyMode(uint16_t txMask, float txPower) = 0;
    virtual void setConstantFrequency(double frequency)                      = 0;
};
