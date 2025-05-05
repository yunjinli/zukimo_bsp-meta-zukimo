/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRadar.hpp"

#include <common/Buffer.hpp>
#include <common/EndianConversion.hpp>
#include <common/Serialization.hpp>
#include <components/exception/ERadar.hpp>
#include <components/radar/TypeSerialization.hpp>
#include <platform/exception/EProtocol.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/components/types.h>
#include <universal/components/types/iradar.h>
#include <universal/protocol/protocol_definitions.h>


RemoteRadar::RemoteRadar(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_DEFAULT, id, COMPONENT_SUBIF_DEFAULT}
{
}

void RemoteRadar::initialize()
{
    m_vendorCommands.vendorWrite(FN_RADAR_INITIALIZE, 0, nullptr);
}

void RemoteRadar::reset(bool softReset)
{
    const uint8_t mode = softReset ? 1 : 0;
    m_vendorCommands.vendorWrite(FN_RADAR_RESET, sizeof(mode), &mode);
}

void RemoteRadar::configure(const IfxRfe_MmicConfig *c)
{
    constexpr size_t size = serialized_sizeof(IfxRfe_MmicConfig());
    uint8_t buf[size];

    hostToSerial(buf, c);

    m_vendorCommands.vendorWrite(FN_RADAR_CONFIGURE, size, buf);
}

void RemoteRadar::loadSequence(IfxRfe_Sequence *s)
{
    const size_t size = serialized_sizeof(s, s->rampCount);
    stdext::buffer<uint8_t> buf(size);

    uint8_t *it = buf.data();
    it          = hostToSerial(it, s);
    it          = hostToSerial(it, s->ramps, s->rampCount);

    m_vendorCommands.vendorWrite(FN_RADAR_LOAD_SEQUENCE, static_cast<uint16_t>(size), buf.data());
}

void RemoteRadar::calibrate()
{
    m_vendorCommands.vendorWrite(FN_RADAR_CALIBRATE, 0, nullptr);
}

void RemoteRadar::startSequence()
{
    m_vendorCommands.vendorWrite(FN_RADAR_START_SEQUENCE, 0, nullptr);
}

uint8_t RemoteRadar::getDataIndex()
{
    uint8_t idx;
    m_vendorCommands.vendorRead(FN_RADAR_GET_DATA_INDEX, sizeof(idx), &idx);
    return idx;
}

void RemoteRadar::startData()
{
    m_vendorCommands.vendorWrite(FN_RADAR_START_DATA, 0, nullptr);
}

void RemoteRadar::stopData()
{
    m_vendorCommands.vendorWrite(FN_RADAR_STOP_DATA, 0, nullptr);
}

void RemoteRadar::enableConstantFrequencyMode(uint16_t txMask, float txPower)
{
    constexpr uint16_t wLength = sizeof(txMask) + sizeof(txPower);
    uint8_t buf[wLength];
    uint8_t *it = buf;
    it          = hostToSerial(it, txMask);
    it          = hostToSerial(it, txPower);
    m_vendorCommands.vendorWrite(FN_RADAR_ENABLE_CONSTANT_FREQ, wLength, buf);
}

void RemoteRadar::setConstantFrequency(double frequency)
{
    m_vendorCommands.vendorWrite(FN_RADAR_SET_CONSTANT_FREQ, sizeof(frequency), reinterpret_cast<uint8_t *>(&frequency));
}
