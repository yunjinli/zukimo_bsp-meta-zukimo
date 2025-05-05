/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BridgeProtocol.hpp"
#include <common/Logger.hpp>
#include <common/Serialization.hpp>
#include <platform/exception/EProtocol.hpp>
#include <platform/exception/EProtocolFunction.hpp>
#include <universal/protocol/protocol_definitions.h>


BridgeProtocol::BridgeProtocol(IVendorCommands *commands) :
    m_commands(commands),
    m_I2c(m_commands),
    m_Gpio(m_commands),
    m_Spi(m_commands),
    m_Memory(m_commands),
    m_Flash(m_commands),
    m_Data(m_commands)
{
}

IVendorCommands *BridgeProtocol::getIVendorCommands()
{
    return m_commands;
}

IGpio *BridgeProtocol::getIGpio()
{
    return &m_Gpio;
}

II2c *BridgeProtocol::getII2c()
{
    return &m_I2c;
}

IData *BridgeProtocol::getIData()
{
    return &m_Data;
}

ISpi *BridgeProtocol::getISpi()
{
    return &m_Spi;
}

IFlash *BridgeProtocol::getIFlash()
{
    return &m_Flash;
}

IMemory<uint32_t> *BridgeProtocol::getIMemory()
{
    return &m_Memory;
}

void BridgeProtocol::setDefaultTimeout()
{
    m_commands->setDefaultTimeout();
}

uint16_t BridgeProtocol::getMaxTransfer() const
{
    return m_commands->getMaxTransfer();
}

IVendorCommands *BridgeProtocol::getIVendorCommands() const
{
    return m_commands;
}

void BridgeProtocol::readVersionInfo()
{
    constexpr uint16_t wValue  = 1;
    constexpr uint16_t wLength = sizeof(m_versionInfo);
    m_commands->vendorRead(REQ_BOARD_INFO, wValue, 0, wLength, m_versionInfo.data());
    littleToHost(m_versionInfo.begin(), m_versionInfo.end());
}

void BridgeProtocol::readExtendedVersion()
{
    constexpr uint16_t maxLength = 256;
    uint8_t buffer[maxLength];

    constexpr uint16_t wValue = 3;
    uint16_t wLength          = maxLength;
    m_commands->vendorTransfer(REQ_BOARD_INFO, wValue, 0, 0, nullptr, wLength, buffer);
    m_extendedVersionString.assign(reinterpret_cast<const char *>(buffer), wLength);
}

void BridgeProtocol::checkVersion()
{
    readVersionInfo();

    LOG(DEBUG) << "Board protocol version = " << m_versionInfo[4] << "." << m_versionInfo[5];
    LOG(DEBUG) << "Board flash image version = " << m_versionInfo[0] << "." << m_versionInfo[1] << "." << m_versionInfo[2] << "." << m_versionInfo[3];

    const uint32_t boardVer = static_cast<uint32_t>(m_versionInfo[4] << 16) | m_versionInfo[5];
    const uint32_t hostVer  = ((PROTOCOL_VERSION_MAJOR << 16) | PROTOCOL_VERSION_MINOR);

    if (boardVer < hostVer)
    {
        std::string msg("Protocol version mismatch! Please update board with Flash Image using protocol version ");
        msg.append(std::to_string(PROTOCOL_VERSION_MAJOR));
        msg.append(".");
        msg.append(std::to_string(PROTOCOL_VERSION_MINOR));
        msg.append(" (it has ");
        msg.append(std::to_string(m_versionInfo[4]));
        msg.append(".");
        msg.append(std::to_string(m_versionInfo[5]));
        msg.append(")");
        throw EProtocol(msg.c_str());
    }
    else if (boardVer > hostVer)
    {
        std::string msg("Protcol version mismatch! Please use Host Software with protocol version ");
        msg.append(std::to_string(m_versionInfo[4]));
        msg.append(".");
        msg.append(std::to_string(m_versionInfo[5]));
        msg.append(" (this is ");
        msg.append(std::to_string(PROTOCOL_VERSION_MAJOR));
        msg.append(".");
        msg.append(std::to_string(PROTOCOL_VERSION_MINOR));
        msg.append(")");
        throw EProtocol(msg.c_str());
    }
}

void BridgeProtocol::getBoardInfo(BoardInfo_t &buffer)
{
    constexpr uint16_t wValue = 0;

    // For backwards compatiblity we first read a fixed size of 32,
    // since some firmwares have a check for that size.
    uint16_t wLength = 32;

    // 1. Ensure that the buffer is set to 0 at the position of the last fixed size element
    //    This allows to detect if the board info is longer.
    buffer[wLength - 1] = 0;

    // 2. Read the fixed size info.
    m_commands->vendorRead(REQ_BOARD_INFO, wValue, 0, wLength, buffer.data());

    // 3. The buffer for backwards compatiblity was completely filled with data.
    //    This indicates that the board info is longer.
    //    Now use a transfer for the firmware to decide how much to send, up to the size of buffer
    if (buffer[wLength - 1] != 0)
    {
        wLength = sizeof(buffer);
        m_commands->vendorTransfer(REQ_BOARD_INFO, wValue, 0, 0, nullptr, wLength, buffer.data());
    }

    // 4. make sure the string is terminated to avoid memory leak
    buffer[wLength - 1] = 0;
}

void BridgeProtocol::getLastError(uint8_t buffer[8])
{
    constexpr uint16_t wValue = 0;
    m_commands->vendorRead(REQ_ERROR_INFO, wValue, 0, sizeof(*buffer) * 8, buffer);
}

void BridgeProtocol::getDetailedError(int32_t errors[4])
{
    constexpr uint16_t wValue = 1;
    constexpr uint16_t count  = 4;
    m_commands->vendorRead(REQ_ERROR_INFO, wValue, 0, sizeof(*errors) * count, errors);
    littleToHost(errors, errors + count);
}

const std::string &BridgeProtocol::getExtendedVersionString()
{
    if (m_extendedVersionString.empty())
    {
        readExtendedVersion();
    }

    return m_extendedVersionString;
}

void BridgeProtocol::readUuid()
{
    constexpr uint16_t wValue = 2;
    try
    {
        m_commands->vendorRead(REQ_BOARD_INFO, wValue, 0, static_cast<uint16_t>(m_uuid.size()), m_uuid.data());
    }
    catch (const EProtocolFunction &e)
    {
        if ((e.code() == E_NOT_IMPLEMENTED) || (e.code() == STATUS_REQUEST_WVALUE_INVALID))
        {
            // the connected board does not support the UUID request, call the default implementation
            BridgeControl::readUuid();
        }
        else
        {
            throw;
        }
    }
}
