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
 * @file Types.h
 *
 * @brief Header for Types
 */

#ifndef IFX_DOPPLER_LTR11_TYPES_H
#define IFX_DOPPLER_LTR11_TYPES_H

#include <ifxBase/Types.h>

typedef enum {
    IFX_LTR11_PULSE_WIDTH_5us   = 0,
    IFX_LTR11_PULSE_WIDTH_10us  = 1,
    IFX_LTR11_PULSE_WIDTH_3us   = 2,
    IFX_LTR11_PULSE_WIDTH_4us   = 3
} ifx_LTR11_PulseWidth_t;

typedef enum {
    IFX_LTR11_PRT_250us     = 0,
    IFX_LTR11_PRT_500us     = 1,
    IFX_LTR11_PRT_1000us    = 2,
    IFX_LTR11_PRT_2000us    = 3
} ifx_LTR11_PRT_t;

typedef enum {
    IFX_LTR11_TX_POWER_LEVEL_MINUS_34dBm    = 0,
    IFX_LTR11_TX_POWER_LEVEL_MINUS_31_5dBm  = 1,
    IFX_LTR11_TX_POWER_LEVEL_MINUS_25dBm    = 2,
    IFX_LTR11_TX_POWER_LEVEL_MINUS_18dBm    = 3,
    IFX_LTR11_TX_POWER_LEVEL_MINUS_11dBm    = 4,
    IFX_LTR11_TX_POWER_LEVEL_MINUS_5dBm     = 5,
    IFX_LTR11_TX_POWER_LEVEL_0dBm           = 6,
    IFX_LTR11_TX_POWER_LEVEL_4_5dBm         = 7
} ifx_LTR11_TxPowerLevel_t;

typedef enum {
    IFX_LTR11_RX_IF_GAIN_10dB = 0,
    IFX_LTR11_RX_IF_GAIN_15dB = 1,
    IFX_LTR11_RX_IF_GAIN_20dB = 2,
    IFX_LTR11_RX_IF_GAIN_25dB = 3,
    IFX_LTR11_RX_IF_GAIN_30dB = 4,
    IFX_LTR11_RX_IF_GAIN_35dB = 5,
    IFX_LTR11_RX_IF_GAIN_40dB = 6,
    IFX_LTR11_RX_IF_GAIN_45dB = 7,
    IFX_LTR11_RX_IF_GAIN_50dB = 8
} ifx_LTR11_RxIFGain_t;

typedef enum
{
    IFX_LTR11_HOLD_TIME_MIN   = 0,
    IFX_LTR11_HOLD_TIME_512ms = 4,
    IFX_LTR11_HOLD_TIME_1s    = 8,
    IFX_LTR11_HOLD_TIME_2s    = 16,
    IFX_LTR11_HOLD_TIME_3s    = 24,
    IFX_LTR11_HOLD_TIME_5s    = 40,
    IFX_LTR11_HOLD_TIME_10s   = 79,
    IFX_LTR11_HOLD_TIME_20s   = 235,
    IFX_LTR11_HOLD_TIME_45s   = 352,
    IFX_LTR11_HOLD_TIME_60s   = 469,
    IFX_LTR11_HOLD_TIME_90s   = 704,
    IFX_LTR11_HOLD_TIME_2min  = 938,
    IFX_LTR11_HOLD_TIME_5min  = 2345,
    IFX_LTR11_HOLD_TIME_10min = 4690,
    IFX_LTR11_HOLD_TIME_15min = 7035,
    IFX_LTR11_HOLD_TIME_30min = 14070
} ifx_LTR11_Hold_Time_t;


typedef enum {
    IFX_LTR11_APRT_FACTOR_4  = 0,
    IFX_LTR11_APRT_FACTOR_8  = 1,
    IFX_LTR11_APRT_FACTOR_16 = 2,
    IFX_LTR11_APRT_FACTOR_2  = 3,
    IFX_LTR11_APRT_FACTOR_1  = 4 /* aprt disabled */
} ifx_LTR11_APRT_Factor_t;

typedef enum {
    IFX_LTR11_RF_BAND_COMMON,
    IFX_LTR11_RF_BAND_JAPAN,
} ifx_LTR11_rf_band_selection;

typedef enum
{
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_66   = 66,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_80   = 80,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_90   = 90,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_112  = 112,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_136  = 136,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_192  = 192,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_248  = 248,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_320  = 320,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_384  = 384,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_480  = 480,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_640  = 640,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_896  = 896,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_1344 = 1344,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_1920 = 1920,
    IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_2560 = 2560
} ifx_LTR11_Internal_Detector_Threshold_t;

#endif // IFX_DOPPLER_LTR11_TYPES_H
