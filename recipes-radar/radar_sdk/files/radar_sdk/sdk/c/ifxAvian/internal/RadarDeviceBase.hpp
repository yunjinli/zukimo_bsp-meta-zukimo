/* ===========================================================================
** Copyright (C) 2021 - 2022 Infineon Technologies AG
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
 * @file RadarDeviceBase.hpp
 *
 * @brief Defines the structure for the Radar Device Controller Module.
*/

#ifndef IFX_RADAR_INTERNAL_RADAR_DEVICE_BASE_HPP
#define IFX_RADAR_INTERNAL_RADAR_DEVICE_BASE_HPP

#include <memory>
#include <atomic>
#include <iterator>
#include <tuple>

#include "ifxBase/internal/NonCopyable.hpp"
#include "ifxAvian/internal/DeviceControlAvian.hpp"

#include "ifxBase/Error.h"
#include "ifxBase/Cube.h"
#include "ifxAvian/DeviceConfig.h"
#include "ifxAvian/Metrics.h"

#include "ifxRadarDeviceCommon/internal/AcquisitionState.hpp"

#include "platform/BoardInstance.hpp"
#include <ifxAvian_CwController.hpp>

// libAvian
#include "ifxAvian_Driver.hpp"
#include "ports/ifxAvian_StrataPort.hpp"

using namespace Infineon;

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


/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/
struct RadarDeviceBase: public DeviceControlAvian
{
    NONCOPYABLE(RadarDeviceBase);

    static constexpr auto UUID_INVALID_STR = "00000000-0000-0000-0000-000000000000"; /**< uuid of shield; if all bytes are 0 then the uuid is invalid */

    virtual ~RadarDeviceBase() { ifx_avian_calc_destroy(m_calc);  };

    ifx_Float_t get_device_calc_chirp_time(const ifx_Avian_Config_t* config) const;
    ifx_Avian_Calc_t* get_device_calc() const;

    virtual void set_config(const ifx_Avian_Config_t& config);
    virtual void get_config(ifx_Avian_Config_t& config);

    ifx_Avian_Metrics_t get_default_metrics() const;

    void config_get_limits(ifx_Avian_Config_t& config_lower, ifx_Avian_Config_t& config_upper) const;

    void configure_shapes(ifx_Avian_Shape_Set_t& shape_set);

    void configure_adc(const ifx_Avian_ADC_Config_t& config);

    uint32_t export_register_list(bool set_trigger_bit, uint32_t* register_list);  

    char * get_register_list_string(bool set_trigger_bit);

    uint16_t get_slice_size() const;

    const ifx_Radar_Sensor_Info_t* get_sensor_info() const;

    ifx_Radar_Sensor_t get_sensor_type() const;

    const char* get_board_uuid() const;

    const ifx_Firmware_Info_t* get_firmware_info() const;

    // Returns a list of available hp cutoff frequencies for given device handle in [Hz]
    std::vector<uint32_t> get_hp_cutoff_list(const ifx_Avian_Device_t* handle) const;

    // Returns a list of available aaf cutoff frequencies for given device handle in [Hz]
    std::vector<uint32_t> get_aaf_cutoff_list(const ifx_Avian_Device_t* handle) const;

    ifx_Float_t get_min_chirp_repetition_time(const ifx_Avian_Config_t& config) const;

    ifx_Float_t get_min_frame_repetition_time(const ifx_Avian_Config_t& config) const;

public:
    /* These abstract virtual members must be implemented by a derived class */
    virtual ifx_Float_t get_temperature();

    virtual ifx_Cube_R_t* get_next_frame(ifx_Cube_R_t* frame, uint16_t timeout_ms);

    virtual ifx_Float_t get_tx_power(uint8_t tx_antenna);

    virtual BoardInstance* get_strata_avian_board() const;

    virtual Avian::StrataPort* get_strata_avian_port() const;

    virtual void start_acquisition();
    virtual void stop_acquisition();

    virtual Avian::Constant_Wave_Controller* get_constant_wave_controller();

protected:
    RadarDeviceBase() = default;

    virtual void initialize();
    ifx_Radar_Sensor_Info_t read_sensor_info();
    void stop_and_reset();

private:
    // private methods
    virtual void send_to_device() { };

    /**
     * @brief Compute end delays and end power modes
     *
     * The function computes the shape end delay, frame end delay, shape end power
     * mode, and frame end power mode from the configuration config.
     *
     * The function returns:
     * - shape_end_delay_100ps (shape end delay in units of 100ps)
     * - frame_end_delay_100ps (frame end delay in units of 100ps)
     * - shape_end_power_mode (shape end power mode as uint8_t)
     * - frame_end_power_mode (frame end power mode as uint8_t)
     */
    std::tuple<uint64_t, uint64_t, uint8_t, uint8_t> compute_end_delays(const ifx_Avian_Config_t& config) const;

    /// Convert device configuration into a shape set
    ifx_Avian_Shape_Set_t get_shape_set_from_config(const ifx_Avian_Config_t& config) const;

protected:
    Avian::Baseband_Configuration initialize_baseband_config(const ifx_Avian_Config_t& config) const;

    // attributes
    std::string m_board_uuid = UUID_INVALID_STR; /**< UUID of board */
    ifx_Radar_Sensor_Info_t m_sensor_info = {};  /**< Sensor information */
    ifx_Firmware_Info_t m_firmware_info = {};    /**< Firmware information */
    Atomic_Acquisition_State_t m_acquisition_state{ Acquisition_State_t::Stopped };
};

#endif /* IFX_RADAR_INTERNAL_RADAR_DEVICE_BASE_HPP */
