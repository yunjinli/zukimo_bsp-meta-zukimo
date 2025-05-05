/* ===========================================================================
** Copyright (C) 2019-2020 Infineon Technologies AG. All rights reserved.
** ===========================================================================
**
** ===========================================================================
** Infineon Technologies AG (INFINEON) is supplying this file for use
** exclusively with Infineon's sensor products. This file can be freely
** distributed within development tools and software supporting such
** products.
**
** THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
** OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
** INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON
** WHATSOEVER.
** ===========================================================================
*/

#pragma once

#include "ifxBase/Error.h"
#include "ifxRadarDeviceCommon/RadarDeviceCommon.h"
#include <components/nonvolatileMemory/NonvolatileMemoryEepromI2c.hpp>

/**
 * @brief Defines the structure for EEPROM header object.
 *        Can be used for reading header from EEPROM. For writing needs to be initialized
 *        with 8 byte string identifier, version number, shield type, NVM type and a custom or
 *        generated 16 byte uuid.
 */
typedef struct {
    uint8_t magic[8];       /**< Magic string identifier. owned by object */
    uint8_t version_major;  /**< Major part of version number (major.minor), currently 1 */
    uint8_t version_minor;  /**< Minor part of version number (major.minor), currently 0 */
    uint16_t shield_type;   /**< Shield type identifier using enum ifx_RF_Shield_Type_t */
    uint16_t nvm_type;      /**< node version manager identifier using enum ifx_RF_Shield_Eeprom_Nvm_Type_t */
    uint16_t reserved;      /**< reserved bytes, set to 0 */
    uint8_t uuid[16];       /**< 16 byte uuid */
} ifx_RF_Shield_Eeprom_Header_t;

/**
 * @brief Used to identify the shield NVM type.
 */
typedef enum {
    IFX_EEPROM_NVM_NONE = 0x0000,       /**< No NVM onboard */
    IFX_EEPROM_NVM_UNKNOWN = 0x0001,    /**< Unknown NVM type */
    IFX_EEPROM_NVM_AUTO = 0x0002,       /**< The NVM type is to be automatically determined, only useful as a parameter */
    IFX_EEPROM_NVM_MCUFLASH = 0x0100,   /**< Flash memory from the MCU */
    IFX_EEPROM_NVM_24CW128X = 0x0200,   /**< EEPROM Type 24CW128X */
} ifx_RF_Shield_Eeprom_Nvm_Type_t;

/**
 * Specific implementation of non-volatile memory for RF shield I2C EEPROMs
 */
class RfShieldEeprom :
    public NonvolatileMemoryEepromI2c
{
public:
    /**
     * @brief Construct an object
     * @param access   The I2C interface to use for communication with the EEPROM
     * @param i2cBusId The I2C bus id used by the EEPPROM to talk to as it is wired on the device
     */
    RfShieldEeprom(II2c *access, uint8_t i2cBusId);

    /**
     * @brief Reads EEPROM header structure
     *
     * In case the shield EEPROM is not present, not readable,
     * or not initialised, the function returns an error.
     *
     * @param [in]    header        EEPROM header structure
     * @retval        status        execution status
     */
    ifx_Error_e readHeader(ifx_RF_Shield_Eeprom_Header_t *header);

    /**
     * @brief Writes EEPROM header structure
     *
     * In case the shield EEPROM is not present, or not writable,
     * the function returns an error.
     *
     * @param [in]    header        EEPROM header structure
     * @retval        status        execution status
     */
    ifx_Error_e writeHeader(const ifx_RF_Shield_Eeprom_Header_t *header);

    /**
     * @brief Format EEPROM header structure
     *
     * Initialises mandatory fields of EEPROM header structure.
     *
     * @param [in]    header        shield EEPROM header structure
     * @param [in]    uuid          unique id of shield (16-byte long)
     * @param [in]    shield_type   shield type according to ifx_RF_Shield_Type_t
     * @retval        status        execution status
     */
    static void formatHeader(ifx_RF_Shield_Eeprom_Header_t *header, uint8_t* uuid, uint16_t shield_type);
};
