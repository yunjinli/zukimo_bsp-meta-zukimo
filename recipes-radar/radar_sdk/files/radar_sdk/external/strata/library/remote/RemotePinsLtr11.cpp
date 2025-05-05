/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemotePinsLtr11.hpp"

#include <platform/exception/EProtocol.hpp>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/ipins.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>


RemotePinsLtr11::RemotePinsLtr11(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_LTR11, id, COMPONENT_SUBIF_PINS}
{
}

void RemotePinsLtr11::setResetPin(bool state)
{
    const uint8_t bState = state ? 1 : 0;
    m_vendorCommands.vendorWrite(FN_PINS_SET_RESET_PIN, sizeof(bState), &bState);
}

void RemotePinsLtr11::reset()
{
    m_vendorCommands.vendorWrite(FN_PINS_RESET, 0, nullptr);
}

bool RemotePinsLtr11::getIrqPin()
{
    uint8_t state;
    m_vendorCommands.vendorRead(FN_PINS_GET_IRQ, sizeof(state), &state);
    return (state != 0);
}

uint8_t RemotePinsLtr11::getDetectionPins()
{
    uint8_t states;
    m_vendorCommands.vendorRead(FN_PINS_GET_DETECTIONS, sizeof(states), &states);
    return states;
}