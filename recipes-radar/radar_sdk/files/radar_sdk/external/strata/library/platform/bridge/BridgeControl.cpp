/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BridgeControl.hpp"

#include <common/exception/ENotImplemented.hpp>


BridgeControl::BridgeControl() :
    m_uuidRead {false}
{
}

IVendorCommands *BridgeControl::getIVendorCommands()
{
    return nullptr;
}

void BridgeControl::checkVersion()
{
}

void BridgeControl::setDefaultTimeout()
{
}

uint16_t BridgeControl::getMaxTransfer() const
{
    return UINT16_MAX;
}

IData *BridgeControl::getIData()
{
    throw ENotImplemented("getIData() not implemented for this Bridge");
}

IGpio *BridgeControl::getIGpio()
{
    throw ENotImplemented("getIGpio() not implemented for this Bridge");
}

II2c *BridgeControl::getII2c()
{
    throw ENotImplemented("getII2c() not implemented for this Bridge");
}

ISpi *BridgeControl::getISpi()
{
    throw ENotImplemented("getISpi() not implemented for this Bridge");
}

IFlash *BridgeControl::getIFlash()
{
    throw ENotImplemented("getIFlash() not implemented for this Bridge");
}

IMemory<uint32_t> *BridgeControl::getIMemory()
{
    throw ENotImplemented("getIMemory() not implemented for this Bridge");
}

const std::array<uint16_t, 8> &BridgeControl::getVersionInfo()
{
    return m_versionInfo;
}

const std::string &BridgeControl::getVersionString()
{
    if (m_versionString.empty())
    {
        m_versionString = std::to_string(m_versionInfo[0]) + '.' +
                          std::to_string(m_versionInfo[1]) + '.' +
                          std::to_string(m_versionInfo[2]);
        if (m_versionInfo[3])
        {
            m_versionString += '+';
            m_versionString += std::to_string(m_versionInfo[3]);
        }
    }
    return m_versionString;
}

const std::string &BridgeControl::getExtendedVersionString()
{
    return m_extendedVersionString;
}

const IBridgeControl::Uuid_t &BridgeControl::getUuid()
{
    if (!m_uuidRead)
    {
        readUuid();
        m_uuidRead = true;
    }

    return m_uuid;
}

void BridgeControl::readUuid()
{
    // default implementation
    m_uuid.fill(0);
}
