/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteVendorCommands.hpp"
#include <platform/exception/EProtocol.hpp>

RemoteVendorCommands::RemoteVendorCommands(IVendorCommands *commands, uint8_t bRequest, uint8_t bType, uint8_t bImplementation, uint8_t bId, uint8_t bSubInterface) :
    m_commands {commands},
    m_bRequest {bRequest},
    m_bType {bType},
    m_bImplementation {bImplementation},
    m_bId {bId},
    m_bSubInterface {bSubInterface}
{
}

void RemoteVendorCommands::vendorWrite(uint8_t bFunction, uint16_t wLength, const uint8_t buffer[]) const
{
    m_commands->vendorWrite(m_bRequest, m_bType, m_bImplementation, m_bId, m_bSubInterface, bFunction, wLength, buffer);
}

void RemoteVendorCommands::vendorRead(uint8_t bFunction, uint16_t wLength, uint8_t buffer[]) const
{
    m_commands->vendorRead(m_bRequest, m_bType, m_bImplementation, m_bId, m_bSubInterface, bFunction, wLength, buffer);
}

void RemoteVendorCommands::vendorTransfer(uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[]) const
{
    m_commands->vendorTransfer(m_bRequest, m_bType, m_bImplementation, m_bId, m_bSubInterface, bFunction, wLengthSend, bufferSend, wLengthReceive, bufferReceive);
}

void RemoteVendorCommands::vendorTransferChecked(uint8_t bFunction, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t wLengthReceive, uint8_t bufferReceive[]) const
{
    m_commands->vendorTransferChecked(m_bRequest, m_bType, m_bImplementation, m_bId, m_bSubInterface, bFunction, wLengthSend, bufferSend, wLengthReceive, bufferReceive);
}
