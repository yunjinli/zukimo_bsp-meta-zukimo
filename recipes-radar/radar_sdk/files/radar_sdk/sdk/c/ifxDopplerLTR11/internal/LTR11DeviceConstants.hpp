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
 * @file LTR11DeviceConstants.hpp
 *
 * \brief Constants for LTR11 
 *
 */

#ifndef IFX_LTR11_DEVICE_CONSTANTS_HPP
#define IFX_LTR11_DEVICE_CONSTANTS_HPP


constexpr uint64_t LTR11_FREQUENCY_DEFAULT       = 61'044'000'000ull;
constexpr uint64_t LTR11_FREQUENCY_DEFAULT_JAPAN = 60'540'000'000ull;

constexpr uint64_t LTR11_FREQUENCY_MIN       = 61'044'000'000ull;
constexpr uint64_t LTR11_FREQUENCY_MAX       = 61'452'000'000ull;
constexpr uint64_t LTR11_FREQUENCY_MIN_JAPAN = 60'540'000'000ull;
constexpr uint64_t LTR11_FREQUENCY_MAX_JAPAN = 60'948'000'000ull;

constexpr uint64_t LTR11_FREQUENCY_STEP = 2'400'000ull;

static_assert(LTR11_FREQUENCY_DEFAULT <= LTR11_FREQUENCY_MAX);
static_assert(LTR11_FREQUENCY_MIN <= LTR11_FREQUENCY_DEFAULT);

static_assert(LTR11_FREQUENCY_DEFAULT_JAPAN <= LTR11_FREQUENCY_MAX_JAPAN);
static_assert(LTR11_FREQUENCY_MIN_JAPAN <= LTR11_FREQUENCY_DEFAULT_JAPAN);

constexpr uint16_t LTR11_INTERNAL_DETECTOR_THRESHOLD_MIN     = 0;
constexpr uint16_t LTR11_INTERNAL_DETECTOR_THRESHOLD_MAX     = (1 << 12) - 1;
constexpr uint16_t LTR11_INTERNAL_DETECTOR_THRESHOLD_DEFAULT = IFX_LTR11_INTERNAL_DETECTOR_THRESHOLD_80;

static_assert(LTR11_INTERNAL_DETECTOR_THRESHOLD_DEFAULT <= LTR11_INTERNAL_DETECTOR_THRESHOLD_MAX);
static_assert(LTR11_INTERNAL_DETECTOR_THRESHOLD_MIN <= LTR11_INTERNAL_DETECTOR_THRESHOLD_DEFAULT);

// number of samples
constexpr uint16_t LTR11_NUM_OF_SAMPLES_MIN     = 16;
constexpr uint16_t LTR11_NUM_OF_SAMPLES_MAX     = 1024;
constexpr uint16_t LTR11_NUM_OF_SAMPLES_DEFAULT = 256;

static_assert(LTR11_NUM_OF_SAMPLES_DEFAULT <= LTR11_NUM_OF_SAMPLES_MAX);
static_assert(LTR11_NUM_OF_SAMPLES_MIN <= LTR11_NUM_OF_SAMPLES_DEFAULT);

#endif /* IFX_LTR11_DEVICE_CONSTANTS_HPP */
