/* ===========================================================================
** Copyright (C) 2022 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include "RfShieldEeprom.hpp"
#include <common/endian/General.hpp>
#include <common/endian/Little.hpp>
#include <cstring>

#define EE24CW128X_BUS_ADDRESS 0x50   // i2c bus address used by EEPROM
#define EE24CW128X_ADDRESS_BYTES 2    // number of address bytes used for EEPROM i2c transfers

#define EEPROM_HEADER_START_ADDR 0  // start address of header structure within EEPROM
#define EEPROM_HEADER_VERSION_MAJOR 1
#define EEPROM_HEADER_VERSION_MINOR 0

namespace
{
    constexpr NonvolatileMemoryConfig_t config_eeprom = {
        16 * 1024,  // total size
        0U,
        32,  // page size
        0U,
        0U,
    };

    /** Magic string identifier used by EEPROM header */
    constexpr uint8_t eeprom_magic_sequence[8] = { 'I', 'f', 'x', 'B', 'o', 'a', 'r', 'd' };

    /** Return true if EEPROM header contents are valid */
    bool isHeaderValid(const ifx_RF_Shield_Eeprom_Header_t *header)
    {
        return (memcmp(header->magic, eeprom_magic_sequence, sizeof(eeprom_magic_sequence)) == 0);
    }
}

RfShieldEeprom::RfShieldEeprom(II2c *access, uint8_t i2cBusId) :
    NonvolatileMemoryEepromI2c(access, I2C_ADDR(i2cBusId, EE24CW128X_BUS_ADDRESS), EE24CW128X_ADDRESS_BYTES, config_eeprom)
{}

ifx_Error_e RfShieldEeprom::readHeader(ifx_RF_Shield_Eeprom_Header_t *header)
{
    try
    {
        readRandom(EEPROM_HEADER_START_ADDR, sizeof(ifx_RF_Shield_Eeprom_Header_t), reinterpret_cast<uint8_t*>(header));
        header->shield_type = littleToHost(header->shield_type);
        header->nvm_type = littleToHost(header->nvm_type);
        header->reserved = littleToHost(header->reserved);
    }
    catch (...)
    {
        // eeprom is not available or accessible
        return IFX_ERROR_EEPROM;
    }

    if (!isHeaderValid(header))
    {
        // eeprom is available but the content is not initialized
        return IFX_ERROR_EEPROM;
    }
    return IFX_OK;
}

ifx_Error_e RfShieldEeprom::writeHeader(const ifx_RF_Shield_Eeprom_Header_t *header)
{
    try
    {
        ifx_RF_Shield_Eeprom_Header_t header_copy = *header;
        header_copy.shield_type = hostToLittle(header_copy.shield_type);
        header_copy.nvm_type = hostToLittle(header_copy.nvm_type);
        header_copy.reserved = hostToLittle(header_copy.reserved);
        writeRandom(EEPROM_HEADER_START_ADDR, sizeof(ifx_RF_Shield_Eeprom_Header_t), reinterpret_cast<uint8_t*>(&header_copy));
    }
    catch (...)
    {
        // eeprom is not available or accessible
        return IFX_ERROR_COMMUNICATION_ERROR;
    }
    return IFX_OK;
}

void RfShieldEeprom::formatHeader(ifx_RF_Shield_Eeprom_Header_t *header, uint8_t* uuid, uint16_t shield_type)
{
    memcpy(header->magic, eeprom_magic_sequence, sizeof(eeprom_magic_sequence));
    header->version_major = EEPROM_HEADER_VERSION_MAJOR;
    header->version_minor = EEPROM_HEADER_VERSION_MINOR;
    header->shield_type = shield_type;           // ifx_RF_Shield_Type_t
    header->nvm_type = IFX_EEPROM_NVM_24CW128X;  // ifx_RF_Shield_Eeprom_Nvm_Type_t
    header->reserved = 0;
    memcpy(header->uuid, uuid, sizeof(header->uuid));  // 16 byte uuid
}
