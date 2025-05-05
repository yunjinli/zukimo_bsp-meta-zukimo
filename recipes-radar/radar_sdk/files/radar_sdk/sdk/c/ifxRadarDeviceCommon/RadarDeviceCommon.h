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

/**
 * @file RadarDeviceCommon.h
 *
 * @brief Header for ifxRadarDeviceCommon
 *
 * Includes all ifxRadarDeviceCommon headers.
 *
 * \defgroup gr_cat_Radar_Device_Common        Radar Device Common (ifxDeviceCommon)
 */

#ifndef IFX_RADAR_DEVICE_COMMON_H
#define IFX_RADAR_DEVICE_COMMON_H

#include "ifxBase/Types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** @addtogroup gr_cat_Radar_Device_Common
  * @{
  */

    /* When adding/changing values also make sure to update the function
     * board_type_to_string in rdk::RadarDeviceCommon.
     */
    typedef enum {
        IFX_BOARD_UNKNOWN = 0,         /**< Unknown board */
        IFX_BOARD_BASEBOARD_AURIX = 1, /**< Aurix board */
        IFX_BOARD_BASEBOARD_MCU7 = 2,  /**< Radar Baseboard MCU7 */
        IFX_BOARD_BASEBOARD_V9 = 3     /**< V9 board */
    } ifx_Board_Type_t;

    /* The values for the Avian devices must be identical to the values in
     * Infineon::Avian::Device_Type. As this is a C header and Infineon::Avian::Device_Type
     * is an enum class, we unfortunately cannot use the values of the original values
     * directly.
     *
     * When adding/changing values also make sure to update the functions
     * sensor_is_avian, sensor_is_mimose, sensor_to_string, and sensor_is_ltr11
     * in rdk::RadarDeviceCommon.
     */
     /** Types of different radar sensors */
    typedef enum
    {
        IFX_AVIAN_BGT60TR13C = 0,   /**< BGT60TR13C */
        IFX_AVIAN_BGT60ATR24C = 1,  /**< BGT60ATR24C */
        IFX_AVIAN_BGT60UTR13D = 2,  /**< BGT60UTR13D */
        IFX_AVIAN_BGT60TR12E = 3,   /**< BGT60TR12E */
        IFX_AVIAN_BGT60UTR11 = 4,   /**< BGT60UTR11 */
        IFX_AVIAN_BGT120UTR13E = 5, /**< BGT120UTR13E */
        IFX_AVIAN_BGT24LTR24 = 6,   /**< BGT24LTR24 */
        IFX_AVIAN_BGT120UTR24 = 7,  /**< BGT120UTR24 */
        IFX_AVIAN_UNKNOWN = 8,      /**< Unknown Avian device */

        IFX_MIMOSE_BGT24ATR22 = 128, /**< BGT24ATR22 */

        IFX_BGT60LTR11AIP = 256,  /**< BGT60LTR11 AIP (Antenna in Package) */

        IFX_RADAR_SENSOR_UNKNOWN = 0xfff /**< Unknown sensor */
    } ifx_Radar_Sensor_t;

    /** Types of different shields */
    typedef enum {
        IFX_SHIELD_MISSING = 0x0000,             /**< Shield not connected */
        IFX_SHIELD_UNKNOWN = 0x0001,             /**< Unknown shield type */
        IFX_SHIELD_BGT60TR13AIP = 0x0200,        /**< Shield for BGT60TR13AIP */
        IFX_SHIELD_BGT60ATR24AIP = 0x0201,       /**< Shield for BGT60ATR24AIP */
        IFX_SHIELD_BGT60UTR11 = 0x0202,          /**< Shield for BGT60UTR11 */
        IFX_SHIELD_BGT60UTR13D = 0x0203,         /**< Shield for BGT60UTR13D */
        IFX_SHIELD_BGT60LTR11 = 0x0300,          /**< Shield for BGT60LTR11 */
        IFX_SHIELD_BGT60LTR11_MONOSTAT = 0x0301, /**< Shield for BGT60LTR11 */
        IFX_SHIELD_BGT60LTR11_B11 = 0x0302,      /**< Shield for BGT60LTR11_B11 */
        IFX_SHIELD_BGT24ATR22_ES = 0x0400,       /**< Shield for BGT24ATR22 Engineering Sample */
        IFX_SHIELD_BGT24ATR22_PROD = 0x0401,     /**< Shield for BGT24ATR22 Production Sample */
    } ifx_RF_Shield_Type_t;

    typedef struct
    {
        const char* description; /**< A pointer to a zero-terminated string
                                      containing a firmware description. */
        uint16_t version_major; /**< The firmware version major number. */
        uint16_t version_minor; /**< The firmware version minor number. */
        uint16_t version_build; /**< The firmware version build number. */

        const char* extendedVersion; /**< Extended firmware version with additional version information */
    } ifx_Firmware_Info_t;

    typedef struct {
        ifx_Radar_Sensor_t sensor_type;       /**< Type of radar sensor. */
        ifx_Board_Type_t board_type;          /**< Type of board. */
        uint8_t uuid[16];                     /**< Unique id of board. */
    } ifx_Radar_Sensor_List_Entry_t;

    typedef struct
    {
        const char* description;               /**< A pointer to a null terminated
                                                    string holding a human
                                                    readable description of the
                                                    sensor. */
        uint64_t         min_rf_frequency_Hz;  /**< The minimum RF frequency the
                                                    the sensor can emit. */
        uint64_t         max_rf_frequency_Hz;  /**< The maximum RF frequency the
                                                    sensor can emit. */
        uint8_t          num_tx_antennas;      /**< The number of RF antennas used
                                                    for transmission. */
        uint8_t          num_rx_antennas;      /**< The number of RF antennas used
                                                    for reception. */
        uint8_t          max_tx_power;         /**< The amount of RF transmission
                                                    power can be controlled in the
                                                    range of 0 ... max_tx_power.
                                                    */
        uint8_t          num_temp_sensors;     /**< The number of available
                                                    temperature sensors. */
        uint8_t          interleaved_rx;       /**< If this is 0, the radar data
                                                    of multiple RX antennas is
                                                    stored in consecutive data
                                                    blocks, where each block holds
                                                    data of one antenna. If this
                                                    is non-zero, the radar data of
                                                    multiple RX antennas is stored
                                                    in one data block, where for
                                                    each point in time the samples
                                                    from all RX antennas are
                                                    stored consecutively before
                                                    the data of the next point in
                                                    time follows. */
        uint64_t device_id;                    /**< Sensor specific device id.

                                                    For Avian: If supported, a unique device id which is
                                                    programmed during production. If the Avian sensor does
                                                    not have a unique device id the value is 0.

                                                    For radar sensors other than Avian the value is currently 0. */
    } ifx_Radar_Sensor_Info_t;

    typedef struct {
        ifx_RF_Shield_Type_t type;  /**< Type of RF shield */
    } ifx_RF_Shield_Info_t;


/**
  * @}
  */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_DEVICE_COMMON_H */
