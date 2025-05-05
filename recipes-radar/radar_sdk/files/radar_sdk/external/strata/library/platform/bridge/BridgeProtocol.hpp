/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "BridgeControl.hpp"
#include "BridgeProtocolData.hpp"
#include "BridgeProtocolFlash.hpp"
#include "BridgeProtocolGpio.hpp"
#include "BridgeProtocolI2c.hpp"
#include "BridgeProtocolMemory.hpp"
#include "BridgeProtocolSpi.hpp"
#include <platform/interfaces/IVendorCommands.hpp>


class BridgeProtocol :
    public BridgeControl
{

public:
    BridgeProtocol(IVendorCommands *commands);
    /**
     * This function is used to get the BridgeProtocol pointer if one only has the BridgeControl
     * It is introduced to avoid dynamic_cast in some places.
     */
    IVendorCommands *getIVendorCommands() override;
    void checkVersion() override;

    void getBoardInfo(BoardInfo_t &buffer) override;
    void getLastError(uint8_t buffer[8]);
    void getDetailedError(int32_t errors[4]);

    const std::string &getExtendedVersionString() override;

    // IBridgeControl implementation
    IData *getIData() override;
    II2c *getII2c() override;
    ISpi *getISpi() override;
    IGpio *getIGpio() override;
    IFlash *getIFlash() override;
    IMemory<uint32_t> *getIMemory() override;
    void setDefaultTimeout() override;
    uint16_t getMaxTransfer() const override;

    IVendorCommands *getIVendorCommands() const;

protected:
    void readUuid() override;

private:
    void readVersionInfo();
    void readExtendedVersion();

    IVendorCommands *m_commands;

    BridgeProtocolI2c m_I2c;
    BridgeProtocolGpio m_Gpio;
    BridgeProtocolSpi m_Spi;
    BridgeProtocolMemory m_Memory;
    BridgeProtocolFlash m_Flash;
    BridgeProtocolData m_Data;
};
