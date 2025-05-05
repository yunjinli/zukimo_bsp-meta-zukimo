/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
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
 * @internal
 * @file DeviceControlAvian.hpp
 *
 * @brief Defines the class for the Device Controller Module using Avian library.
*/

#ifndef IFX_AVIAN_INTERNAL_DEVICE_CONTROL_AVIAN_HPP
#define IFX_AVIAN_INTERNAL_DEVICE_CONTROL_AVIAN_HPP

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <memory>

#include "ifxAvian/Shapes.h"
#include "ifxAvian/DeviceConfig.h"
#include "ifxAvian/internal/DeviceCalc.h"

// libAvian
#include <ifxAvian_Driver.hpp>

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. TYPES
==============================================================================
*/
using namespace Infineon;

class DeviceControlAvian {
public:

    Avian::Driver* get_avian_driver() const
    {
        return m_driver.get();
    };

    Avian::Driver::Error read_chirp_timing();
    Avian::Driver::Error read_startup_timing();
    Avian::Driver::Error read_chirp_end_delay();
    Avian::Driver::Error read_config();
    Avian::Driver::Error fetch_timings();
    Avian::Driver::Error set_frame_format(uint32_t num_samples_per_chirp, uint32_t num_chirps_per_frame, uint8_t rx_mask);
    Avian::Driver::Error configure_chirp(uint8_t shape_num, uint8_t downChirp, ifx_Avian_Chirp_Config_t* chirp);
    Avian::Driver::Error configure_shape(uint8_t shape_num, ifx_Avian_Shape_t* shape);
    Avian::Driver::Error select_shape(uint8_t shape_num, bool down_chirp);
    Avian::Driver::Error set_shape_fmcw_config(uint8_t shape_num, uint8_t down_chirp, ifx_chirp_direction_t direction, uint32_t lower_frequency_kHz, uint32_t upper_frequency_kHz, uint8_t tx_power);

    Avian::Chirp_Timing get_chirp_timing() const;

protected:
    std::unique_ptr<Avian::Driver> m_driver;
    Avian::Chirp_Timing m_chirp_timing = {};     /**< Chirp timings */
    Avian::Startup_Timing m_startup_timing = {}; /**< Startup timings */
    uint32_t m_chirp_end_delay_100ps = 0; /**< either T_EDU or T_EDD depending if chirp is up or down (for sawtooth: T_EDU) */
    ifx_Avian_Config_t m_config = {};  /**< Device configuration */
    ifx_Avian_Calc_t* m_calc = nullptr; /**< module for all device related computations */
};

#endif  /* IFX_AVIAN_INTERNAL_DEVICE_CONTROL_AVIAN_HPP */