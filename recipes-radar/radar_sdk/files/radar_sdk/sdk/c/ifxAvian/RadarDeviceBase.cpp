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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <algorithm> // for std::minmax and std::max_element
#include <cmath> // for std::round
#include <cstring> // for std::memset
#include <cstdlib> // std::div
#include <sstream>

#include "ifxAvian/internal/RadarDeviceBase.hpp"
#include "ifxAvian/internal/RadarDeviceErrorTranslator.hpp"
#include <ifxBase/Exception.hpp>
#include <ifxBase/internal/Util.h>
#include <ifxBase/Mem.h>

#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"

// libAvian
#include "ifxAvian_DeviceTraits.hpp"
#include "ifxAvian_Utilities.hpp"
#include "ifxAvian_Types.hpp"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

#define CHECK_AVIAN_ERR_CODE(err) do { if((err) != Avian::Driver::Error::OK) return err; } while(0)
#define CHECK_AVIAN_ERR_CODE_GOTO_FAIL(err) do { if((err) != Avian::Driver::Error::OK) goto fail; } while(0)

// Amplification factor that is applied to the IF signal before sampling; Valid range: [18-60]dB
constexpr uint32_t IFX_IF_GAIN_DB_LOWER_LIMIT = 18;
constexpr uint32_t IFX_IF_GAIN_DB_UPPER_LIMIT = 60;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

namespace
{
    const ifx_Avian_Metrics_t BGT24LTR24_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        1.f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.2f, /* frame_repetition_time_s */
        24.125e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT60TR12E_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        1, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        60.5e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT60TR13C_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        60.5e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT60UTR11_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        1, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        60.5e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT60ATR24C_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        60.5e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT60UTR13D_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        60.5e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT120UTR13E_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        120e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t BGT120UTR24_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        120e9f /* center_frequency_Hz*/
    };

    const ifx_Avian_Metrics_t UNSUPPORTED_DEVICE_Metrics =
    {
        1'000'000, /* sample_rate_Hz */
        1, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3f, /* range_resolution_m */
        9.6f, /* max_range_m */
        2.45f, /* max_speed_m_s */
        0.0765f, /* speed_resolution_m_s */
        0.1f, /* frame_repetition_time_s */
        60.5e9f /* center_frequency_Hz*/
    };

    // round from type From to Type To.
    // For example: Rounding double to uint32_t:
    // round<uint32_t>(3.141592);
    template<class To, class From>
    To round(From value)
    {
        return To(std::round(value));
    }

    /// Compute slice size from number of samples per frame, the frame repetition
    /// time (inverse of frame rate), and the FIFO size
    uint16_t calculate_slice_size(uint32_t num_samples_per_frame, float frame_repetition_time_s, uint32_t fifo_size)
    {
        // Take half the FIFO size as a hard cap to have enough buffer to
        // prevent FIFO overflows in the Avian sensor.
        // The Avian sensor will trigger an interrupt if there are at least
        // max_slice_size samples in the internal FIFO. The board (e.g., the
        // RBB MCU7 board) will then read the FIFO. As reacting to the interrupt
        // takes a bit of time and in the mean time more samples are put in the
        // FIFO, max_slice_size must be smaller than the fifo size.
        const uint32_t max_slice_size = fifo_size / 2;

        // Frame rate in Hz. There are frame_rate of frames per second.
        const float frame_rate = 1 / frame_repetition_time_s;

        // Compute the slice rate from the slice_size. The slice rate is the
        // number of slices that are generated per second for a given slice
        // size.
        auto compute_slice_rate = [num_samples_per_frame, frame_rate](uint32_t slice_size) -> float {
            return (num_samples_per_frame * frame_rate) / slice_size;
        };

        // Compute slice rate from the slice rate. With the returned slice size
        // slice_rate of slices will be generated per second.
        // Note that this value might be larger than max_slice_size or fifo_size.
        auto compute_slice_size = [num_samples_per_frame, frame_rate](float slice_rate) -> uint32_t {
            return uint32_t(num_samples_per_frame * frame_rate / slice_rate);
        };

        // Sending a lot of slices from the board to the host is not efficient.
        // If possible we try to avoid slice rates higher than 20 Hz.
        constexpr float slice_rate_threshold = 20; // 20 slices per second

        if (num_samples_per_frame <= max_slice_size)
        {
            // A complete frame fits into a single slice.

            if (compute_slice_rate(num_samples_per_frame) <= slice_rate_threshold)
            {
                // The complete frame fits into a single slice and the
                // corresponding slice rate is smaller than 20 slices per
                // second.
                // One slice corresponds to exactly one frame.
                return num_samples_per_frame;
            }
            else
            {
                // Even though the full frame fits into a single slice, the
                // resulting slice rate would be too high.
                // To avoid sending many small slices, we send k frames as
                // one slice to achieve a slice rate of about 20Hz.
                if (num_samples_per_frame == 0) {
                    throw rdk::exception::num_samples_out_of_range();
                }
                auto k = uint32_t(std::ceil(compute_slice_size(slice_rate_threshold) / num_samples_per_frame));

                return std::min(k * num_samples_per_frame, max_slice_size);
            }
        }
        else
        {
            // A full frame does not fit into a slice.

            // We need at least k slices to fit a frame.
            const auto k = uint32_t(std::ceil(float(num_samples_per_frame) / max_slice_size));

            if (compute_slice_rate(num_samples_per_frame / k) <= slice_rate_threshold)
            {
                // If the slice rate is sufficiently low, we go for
                // num_samples_per_frame/k. This has the advantage that k slices
                // are one frame (at least approximately, due to rounding it
                // might not be exactly true). That means advantage that full
                // frames arrive approximately equidistant in time at the host.
                return num_samples_per_frame / k;
            }
            else
            {
                // Slice rate would be too high. We use the maximum possible slice
                // size to reduce the slice rate as much as possible. The main
                // objective here is to use a big slice size to optimize the throughput
                // and make it less likely that we cannot transfer all samples
                // from the sensor to the host in time.
                return max_slice_size;
            }
        }
    }

    Avian::Tx_Mode tx_mask_to_tx_mode(const uint32_t tx_mask)
    {
        switch (tx_mask)
        {
        case 0:
            // disable all TX antennas
            return Avian::Tx_Mode::Off;
        case 1:
            // enable first TX antenna
            return Avian::Tx_Mode::Tx1_Only;
        case 2:
            // enable second TX antenna
            return Avian::Tx_Mode::Tx2_Only;
        case 3:
            // enable both TX antenna (TX1 and TX2)'
            // Despite the name "Alternating" means both TX antennas are activated
            return Avian::Tx_Mode::Alternating;
            break;
        default:
            throw rdk::exception::tx_antenna_mode_not_allowed();
        }
    }
} // end of anonymous namespace

ifx_Radar_Sensor_Info_t RadarDeviceBase::read_sensor_info()
{
    Avian::Device_Info avian_device_info = {};
    auto rc = m_driver->get_device_info(&avian_device_info);
    if (rc != Infineon::Avian::Driver::Error::OK)
        throw rdk::exception::exception(RadarDeviceErrorTranslator::translate_error_code(rc));

    ifx_Radar_Sensor_Info_t sensor_info;
    sensor_info.description = avian_device_info.description;
    sensor_info.min_rf_frequency_Hz = 1000 * static_cast<uint64_t>(avian_device_info.min_rf_frequency_kHz);
    sensor_info.max_rf_frequency_Hz = 1000 * static_cast<uint64_t>(avian_device_info.max_rf_frequency_kHz);
    sensor_info.num_tx_antennas = avian_device_info.num_tx_antennas;
    sensor_info.num_rx_antennas = avian_device_info.num_rx_antennas;
    sensor_info.max_tx_power = avian_device_info.max_tx_power;
    sensor_info.num_temp_sensors = avian_device_info.num_temp_sensors;
    sensor_info.interleaved_rx = avian_device_info.interleaved_rx;
    sensor_info.device_id = 0;

    return sensor_info;
}

void RadarDeviceBase::initialize()
{
    Avian::Driver::Error rc;

    /* fetch the timings */
    rc = fetch_timings();
    if (rc != Avian::Driver::Error::OK)
        throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));

    /* read the sensor information (among others number of RX and TX antennas) */
    m_sensor_info = read_sensor_info();

    ifx_Avian_Calc_Delays_t delays = {};
    delays.pre_chirp_delay = m_chirp_timing.pre_chirp_delay_100ps * 100e-12;
    delays.post_chirp_delay = m_chirp_timing.post_chirp_delay_100ps * 100e-12;
    delays.pa_delay = m_chirp_timing.pa_delay_100ps * 100e-12;
    delays.adc_delay = m_chirp_timing.adc_delay_100ps * 100e-12;
    delays.wake_up_time = m_startup_timing.wake_up_time_100ps * 100e-12;
    delays.pll_settle_time_coarse = m_startup_timing.pll_settle_time_coarse_100ps * 100e-12;
    delays.pll_settle_time_fine = m_startup_timing.pll_settle_time_fine_100ps * 100e-12;
    delays.chirp_end_delay = m_chirp_end_delay_100ps * 100e-12;

    m_calc = ifx_avian_calc_create(&delays);
    if (!m_calc)
        throw rdk::exception::memory_allocation_failed::exception(ifx_error_get());

    // read configuration from device
    rc = read_config();
    if (rc != Avian::Driver::Error::OK)
        throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
}

uint16_t RadarDeviceBase::get_slice_size() const
{
    uint16_t slice_size;
    m_driver->get_slice_size(&slice_size);

    return slice_size;
}

const char* RadarDeviceBase::get_board_uuid() const
{
    return m_board_uuid.c_str();
}

//----------------------------------------------------------------------------

const ifx_Radar_Sensor_Info_t* RadarDeviceBase::get_sensor_info() const
{
    return &m_sensor_info;
}

//----------------------------------------------------------------------------

const ifx_Firmware_Info_t* RadarDeviceBase::get_firmware_info() const
{
    return &m_firmware_info;
}

//----------------------------------------------------------------------------

uint32_t RadarDeviceBase::export_register_list(bool set_trigger_bit, uint32_t* register_list)
{
    auto registers = m_driver->get_device_configuration().get_configuration_sequence(set_trigger_bit);

    if (register_list != nullptr)
    {
        std::copy(registers.begin(), registers.end(), register_list);
    }
    return static_cast<uint32_t>(registers.size());
}

//----------------------------------------------------------------------------

char* RadarDeviceBase::get_register_list_string(bool set_trigger_bit)
{
    ifx_Avian_Config_t config = {};
    get_config(config);

    std::stringstream string_cpp;

    const uint32_t num_rx_antennas = ifx_util_popcount(config.rx_mask);
    const uint32_t num_tx_antennas = ifx_util_popcount(config.tx_mask);

    string_cpp << "#ifndef XENSIV_BGT60TRXX_CONF_H" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_H" << std::endl;
    string_cpp << std::endl;

    const char* sensor_type_str = rdk::RadarDeviceCommon::sensor_to_string(get_sensor_type());

    string_cpp << "#define XENSIV_BGT60TRXX_CONF_DEVICE (XENSIV_DEVICE_" << sensor_type_str << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_START_FREQ_HZ (" << config.start_frequency_Hz << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_END_FREQ_HZ (" << config.end_frequency_Hz << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP (" << config.num_samples_per_chirp << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_CHIRPS_PER_FRAME (" << config.num_chirps_per_frame << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_RX_ANTENNAS (" << num_rx_antennas << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_TX_ANTENNAS (" << num_tx_antennas << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_SAMPLE_RATE (" << config.sample_rate_Hz << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_CHIRP_REPETITION_TIME_S (" << config.chirp_repetition_time_s << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_FRAME_REPETITION_TIME_S (" << config.frame_repetition_time_s << ")" << std::endl;

    auto register_list = m_driver->get_device_configuration().get_configuration_sequence(set_trigger_bit);

    if (!register_list.empty()) {
        auto size = register_list.size();

        string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_REGS (" << size << ")" << std::endl;

        string_cpp << std::endl;

        string_cpp << "#if defined(XENSIV_BGT60TRXX_CONF_IMPL)" << std::endl;

        string_cpp << "const uint32_t register_list[] = { " << std::endl;
        for (uint32_t i = 0; i < (size - 1); i++)
        {
            string_cpp << "    0x" << std::hex << register_list.at(i) << "UL, " << std::endl;
        }
        string_cpp << "    0x" << std::hex << register_list.at(size - 1) << "UL" << std::endl << "};" << std::endl;

        string_cpp << "#endif /* XENSIV_BGT60TRXX_CONF_IMPL */" << std::endl;
    }

    string_cpp << std::endl;
    string_cpp << "#endif /* XENSIV_BGT60TRXX_CONF_H */" << std::endl;

    auto* outstring_c = static_cast<char*>(ifx_mem_calloc(string_cpp.str().size() + 1, sizeof(char)));
    if (!outstring_c)
        throw rdk::exception::memory_allocation_failed();

    std::memcpy(outstring_c, string_cpp.str().data(), string_cpp.str().size() + 1);

    return outstring_c;
}

//----------------------------------------------------------------------------

ifx_Radar_Sensor_t RadarDeviceBase::get_sensor_type() const
{
    return ifx_Radar_Sensor_t(m_driver->get_device_type());
}

//----------------------------------------------------------------------------

void RadarDeviceBase::configure_adc(const ifx_Avian_ADC_Config_t& config)
{
    Avian::Adc_Configuration adc_configuration = {};
    //oversampling is turned off by default as it is not officially supported
    adc_configuration.oversampling = Avian::Adc_Oversampling::Off;
    adc_configuration.sample_time = Avian::Adc_Sample_Time(config.sample_time);
    adc_configuration.tracking = Avian::Adc_Tracking(config.tracking);
    adc_configuration.double_msb_time = config.double_msb_time;
    adc_configuration.samplerate_Hz = config.samplerate_Hz;

    auto rc = m_driver->set_adc_configuration(&adc_configuration);
    RadarDeviceErrorTranslator::translate_error_code(rc);
}
//----------------------------------------------------------------------------

void RadarDeviceBase::configure_shapes(ifx_Avian_Shape_Set_t& shape_set)
{
    Avian::Frame_Definition frame_definition =
    {
        {
            { shape_set.shape[0].num_repetition, static_cast<Avian::Power_Mode>(shape_set.shape[0].following_power_mode), shape_set.shape[0].end_delay_100ps },
            { shape_set.shape[1].num_repetition, static_cast<Avian::Power_Mode>(shape_set.shape[1].following_power_mode), shape_set.shape[1].end_delay_100ps },
            { shape_set.shape[2].num_repetition, static_cast<Avian::Power_Mode>(shape_set.shape[2].following_power_mode), shape_set.shape[2].end_delay_100ps },
            { shape_set.shape[3].num_repetition, static_cast<Avian::Power_Mode>(shape_set.shape[3].following_power_mode), shape_set.shape[3].end_delay_100ps }
        },
        {
            shape_set.num_repetition, static_cast<Avian::Power_Mode>(shape_set.following_power_mode), shape_set.end_delay_100ps
        },
        0
    };

    Avian::Driver::Error rc;

    rc = m_driver->set_frame_definition(&frame_definition);
        if(rc != Avian::Driver::Error::OK){
        throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
    }
    CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

    for (uint8_t i = 0; i < 4; i++)
    {
        if (shape_set.shape[i].num_repetition == 0)
            break;

        rc = configure_shape(i, &shape_set.shape[i]);
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);
    }

    return;

fail:
    throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
}


//----------------------------------------------------------------------------

void RadarDeviceBase::get_config(ifx_Avian_Config_t& config)
{
    auto rc = read_config();
    if (rc != Avian::Driver::Error::OK)
    {
        throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
    }

    config = m_config;
}

//----------------------------------------------------------------------------

ifx_Avian_Metrics_t RadarDeviceBase::get_default_metrics() const
{
    const auto sensor_type = m_driver->get_device_type();
    switch (sensor_type)
    {
    case Avian::Device_Type::BGT24LTR24:
        return BGT24LTR24_Metrics;
    case Avian::Device_Type::BGT60TR12E:
        return BGT60TR12E_Metrics;
    case Avian::Device_Type::BGT60TR13C:
        return BGT60TR13C_Metrics;
    case Avian::Device_Type::BGT60UTR11:
        return BGT60UTR11_Metrics;
    case Avian::Device_Type::BGT60ATR24C:
        return BGT60ATR24C_Metrics;
    case Avian::Device_Type::BGT60UTR13D:
        return BGT60UTR13D_Metrics;
    case Avian::Device_Type::BGT120UTR13E:
        return BGT120UTR13E_Metrics;
    case Avian::Device_Type::BGT120UTR24:
        return BGT120UTR24_Metrics;
    default:
        return UNSUPPORTED_DEVICE_Metrics;
    }
}

void RadarDeviceBase::config_get_limits(ifx_Avian_Config_t& config_lower, ifx_Avian_Config_t& config_upper) const
{
    auto sensor_type = m_driver->get_device_type();
    const auto& hpf_cutoff_settings = Avian::Device_Traits::get(sensor_type).hpf_cutoff_settings;
    const auto& aaf_cutoff_settings = Avian::Device_Traits::get(sensor_type).aaf_cutoff_settings;
	//Not all 4 values are always used.
    //The last elements of the array may be 0, meaning "reserved" and must not be used.
    auto min_aaf_cutoff_settings = aaf_cutoff_settings.at(0);
    auto max_aaf_cutoff_settings = *std::max_element(aaf_cutoff_settings.begin(), aaf_cutoff_settings.end());

    // Not all 8 values of hpf_cutoff_settings are always used. The last
    // elements of  the array may be 0, meaning "reserved" and must not be used.
    uint16_t min_hp_cutoff_kHz = hpf_cutoff_settings[0];
    uint16_t max_hp_cutoff_kHz = *std::max_element(hpf_cutoff_settings.begin(), hpf_cutoff_settings.end());

    config_lower.sample_rate_Hz = 500'000u; // 500kHz, 80kHz could be also, because of default ADC configs (38.6MHz clock sets 80kHz)
    config_lower.rx_mask = 1; // one rx antenna activated
    config_lower.tx_mask = 1; // one tx antenna activated
    config_lower.tx_power_level = TX_POWER_LEVEL_LOWER;
    config_lower.if_gain_dB = IF_GAIN_DB_LOWER;
    config_lower.start_frequency_Hz = m_sensor_info.min_rf_frequency_Hz;
    config_lower.end_frequency_Hz = m_sensor_info.min_rf_frequency_Hz;
    config_lower.num_samples_per_chirp = 8; // from an RF point of view it doesn't make sense to configure less than 8 samples per chirp
    config_lower.num_chirps_per_frame = 1;
    config_lower.chirp_repetition_time_s = 16e-6f; // 16µs, with 8 samples per chirp and active power mode
    config_lower.frame_repetition_time_s = 0.001f; // frame rate of 1000Hz for Vital sensing use case
    config_lower.hp_cutoff_Hz = 1000 * uint32_t(min_hp_cutoff_kHz);
    config_lower.aaf_cutoff_Hz = 1000 * uint32_t(min_aaf_cutoff_settings);
    config_lower.mimo_mode = IFX_MIMO_OFF;

    config_upper.sample_rate_Hz = 2'500'000u;                        // 2.5MHz
    config_upper.rx_mask = (1 << m_sensor_info.num_rx_antennas) - 1; // all rx antennas activated
    config_upper.tx_mask = 1 << (m_sensor_info.num_tx_antennas - 1); // all tx antennas activated
    config_upper.tx_power_level = TX_POWER_LEVEL_UPPER;
    config_upper.if_gain_dB = IF_GAIN_DB_UPPER;
    config_upper.start_frequency_Hz = m_sensor_info.max_rf_frequency_Hz;
    config_upper.end_frequency_Hz = m_sensor_info.max_rf_frequency_Hz;
    config_upper.num_samples_per_chirp = 1024;
    config_upper.num_chirps_per_frame = 512;
    config_upper.chirp_repetition_time_s = 15e-3f; // 15ms, so that with max chirps of 512, it cannot go less than 0.1Hz frame rate
    config_upper.frame_repetition_time_s = 10;     // frame rate of 0.1Hz to fix Fusion GUI range doppler lower precision issues and slow update of plots
    config_upper.hp_cutoff_Hz = 1000 * uint32_t(max_hp_cutoff_kHz);
    config_upper.aaf_cutoff_Hz = 1000 * uint32_t(max_aaf_cutoff_settings);
    config_upper.mimo_mode = m_sensor_info.num_tx_antennas > 1 ? IFX_MIMO_TDM : IFX_MIMO_OFF; // IFX_MIMO_TDM if device has more than one RX antenna
}

//----------------------------------------------------------------------------

void RadarDeviceBase::set_config(const ifx_Avian_Config_t& config)
{
    // At least one RX antenna must be activated. IF no RX antennas was
    // activated the frame had 0 entries we would run into a division-by-zero
    // error.
    if (config.rx_mask == 0)
        throw rdk::exception::rx_antenna_combination_not_allowed();

    if (m_acquisition_state == Acquisition_State_t::Error)
        throw rdk::exception::communication_error();

    Avian::Driver::Error rc;

    /*
    * As a first step of initialization the device configuration provided by the user is copied into
    * the handle. Some of the parameters are needed during fetching of time domain data.
    */
    m_config = config;
    m_acquisition_state = Acquisition_State_t::Stopped;

    { /* initialization and non-shape related settings */
        /*
        * After connection it is unclear if the device is in easy mode or in normal mode. To be on the
        * safe side, switch to normal mode. This has the nice side effect that this causes a device
        * reset and stops operation. The according firmware function is known to not fail, but of
        * course communication errors could happen, so the error code must not be omitted.
        */
        rc = m_driver->enable_easy_mode(false);
        if (rc != Avian::Driver::Error::OK)
        {
            throw rdk::exception::num_samples_out_of_range();
        }

        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

        rc = m_driver->set_adc_samplerate(config.sample_rate_Hz);
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

        // true if down chirp
        const bool down_chirp = (config.start_frequency_Hz > config.end_frequency_Hz);

        const Avian::Baseband_Configuration baseband_config = initialize_baseband_config(config);
        for (uint8_t shape_num = 0; shape_num < 4; shape_num++)
        {
            rc = select_shape(shape_num, down_chirp);
            CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

            rc = m_driver->set_baseband_configuration(&baseband_config);
            CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);
        }
    }

    {
        /* read the device FIFO size in samples
         * The units used by Device_Traits correspond to pairs of samples,
         * therefore the number of samples is obtained by multiplying by two.
         */
        auto sensor_type = m_driver->get_device_type();
        const auto& device_traits = Avian::Device_Traits::get(sensor_type);
        uint32_t adc_fifo_size = static_cast<uint32_t>(device_traits.fifo_size) * 2;

        ifx_Avian_Shape_Set_t shape_set = get_shape_set_from_config(config);
        IFX_ERR_HANDLE_R(configure_shapes(shape_set), (void)0);

        uint8_t num_antennas = ifx_devconf_count_rx_antennas(&config);
        const uint32_t frame_size = config.num_chirps_per_frame * config.num_samples_per_chirp * num_antennas;

        rc = m_driver->set_slice_size(calculate_slice_size(frame_size, config.frame_repetition_time_s, adc_fifo_size));
        if(rc != Avian::Driver::Error::OK){
            throw rdk::exception::frame_size_not_supported();
        }
    }

    {
		Infineon::Avian::Anti_Alias_Filter_Settings aaf;
        aaf.frequency1_kHz = config.aaf_cutoff_Hz / 1000;
        rc = m_driver->set_anti_alias_filter_settings(&aaf);
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);
    }

    send_to_device();

    return;

fail:
    throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
}

//----------------------------------------------------------------------------

ifx_Avian_Calc_t* RadarDeviceBase::get_device_calc() const
{
    return m_calc;
};

ifx_Float_t RadarDeviceBase::get_device_calc_chirp_time(const ifx_Avian_Config_t* config) const
{
    const auto sampling_time = static_cast<ifx_Float_t>(config->num_samples_per_chirp) / config->sample_rate_Hz;
    return static_cast<ifx_Float_t>(ifx_avian_calc_chirp_time(m_calc, sampling_time));
};

std::tuple<uint64_t, uint64_t, uint8_t, uint8_t> RadarDeviceBase::compute_end_delays(const ifx_Avian_Config_t& config) const
{
    const double frame_time = config.frame_repetition_time_s;

    /* sampling_time = num_samples_per_chirp/adc_samplerate_Hz */
    const double sampling_time = ifx_avian_calc_sampling_time(m_calc, config.num_samples_per_chirp, config.sample_rate_Hz);

    /* chirp_to_chirp_time (also known as pulse_repetition_time) */
    const double chirp_repetition_time = ifx_devconf_get_chirp_repetition_time(&config);

    /*
     * With the parameters read from the device and the parameters specified by the caller,
     * calculate the time taken for a complete single chirp. To achieve the specified chirp-to-chirp
     * time, a post delay ("shape end delay" = T_SED) must be applied. Of course a negative delay is
     * not possible, and the chirp-to-chirp time cannot be smaller than the single chirp time.
     */
    ifx_avian_power_mode_t shape_end_power_mode{};
    ifx_avian_power_mode_t frame_end_power_mode{};

    const double shape_end_delay = ifx_avian_calc_shape_end_delay(m_calc, config.mimo_mode == IFX_MIMO_TDM, sampling_time, chirp_repetition_time, &shape_end_power_mode);
    const double frame_end_delay = ifx_avian_calc_frame_end_delay(m_calc, sampling_time, chirp_repetition_time, config.num_chirps_per_frame, frame_time, &frame_end_power_mode);

    /* check that shape_end_delay and frame_end_delay are valid */
    if (shape_end_delay < 0)
        throw rdk::exception::chirp_rate_out_of_range();
    if (frame_end_delay < 0)
        throw rdk::exception::frame_rate_out_of_range();

    const auto T_SED = round<uint64_t>(shape_end_delay / 100e-12f);
    const auto T_FED = round<uint64_t>(frame_end_delay / 100e-12f);

    return std::make_tuple(T_SED, T_FED, uint8_t(shape_end_power_mode), uint8_t(frame_end_power_mode));
}

ifx_Avian_Shape_Set_t RadarDeviceBase::get_shape_set_from_config(const ifx_Avian_Config_t& config) const
{
    // convert into types needed by ifx_Avian_Shape_Set_t
    const auto tx_mode = uint8_t(tx_mask_to_tx_mode(config.tx_mask));
    const auto rx_mask = uint8_t(config.rx_mask);
    const auto tx_power = uint8_t(config.tx_power_level);

    const auto [shape_end_delay_100ps, frame_end_delay_100ps, shape_end_power_mode, frame_end_power_mode]
        = compute_end_delays(config);

    const auto start_frequency_kHz = round<uint32_t>(config.start_frequency_Hz / 1000.f);
    const auto end_frequency_kHz = round<uint32_t>(config.end_frequency_Hz / 1000.f);

    const ifx_chirp_direction_t chirp_direction = (start_frequency_kHz < end_frequency_kHz)
        ? DIR_UPCHIRP_ONLY
        : DIR_DOWNCHIRP_ONLY;

    const auto [lower_frequency_kHz, upper_frequency_kHz]
        = std::minmax(start_frequency_kHz, end_frequency_kHz);

    ifx_Avian_Shape_Set_t shape_set{};
    shape_set.num_repetition = uint16_t(config.num_chirps_per_frame);
    shape_set.following_power_mode = uint8_t(frame_end_power_mode);
    shape_set.end_delay_100ps = frame_end_delay_100ps;

    // shorter name for convenience
    auto& shape0 = shape_set.shape[0];

    // take care of the Frame_Definition_t via the shape
    shape0.num_repetition = 1; // one repetition
    shape0.following_power_mode = shape_end_power_mode;
    shape0.end_delay_100ps = shape_end_delay_100ps;

    const ifx_Avian_Chirp_Config_t chirp_config =
    {
        /* .chirp_setting = */
        {
            /* .num_samples = */ config.num_samples_per_chirp,
            /* .end_delay_100ps = */ m_chirp_end_delay_100ps
        },
        /* .antenna_setup = */
        {
            /* .rx_mask = */ rx_mask,
            /* .tx_mode = */ tx_mode
        }
    };

    shape0.config =
    {
        /* .direction = */ chirp_direction,
        /* .lower_frequency_kHz = */ lower_frequency_kHz,
        /* .upper_frequency_kHz = */ upper_frequency_kHz,
        /* .tx_power = */ tx_power,

        /* .up = */ chirp_config,
        /* .down = */ chirp_config
    };

    if (config.mimo_mode == IFX_MIMO_TDM)
    {
        shape_set.shape[1] = shape_set.shape[0];

        shape_set.shape[0].following_power_mode = uint8_t(Avian::Power_Mode::Stay_Active);
        shape_set.shape[0].end_delay_100ps = 0;
        shape_set.shape[0].config.up.antenna_setup.tx_mode = uint8_t(Avian::Tx_Mode::Tx1_Only);
        shape_set.shape[0].config.down.antenna_setup.tx_mode = uint8_t(Avian::Tx_Mode::Tx1_Only);

        shape_set.shape[1].following_power_mode = shape_end_power_mode;
        shape_set.shape[1].end_delay_100ps = shape_end_delay_100ps;
        shape_set.shape[1].config.up.antenna_setup.tx_mode = uint8_t(Avian::Tx_Mode::Tx2_Only);
        shape_set.shape[1].config.down.antenna_setup.tx_mode = uint8_t(Avian::Tx_Mode::Tx2_Only);
    }

    return shape_set;
}

//----------------------------------------------------------------------------

Avian::Baseband_Configuration RadarDeviceBase::initialize_baseband_config(const ifx_Avian_Config_t& config) const
{
    constexpr uint32_t reset_timer_period_100ps = 15875; // 1.5875e-06

    // reference for convenience
    const auto& if_gain_dB = config.if_gain_dB;
    const auto hp_cutoff_kHz = round<uint16_t>(config.hp_cutoff_Hz / 1000.f);

    // check range of if_gain_dB
    if ((if_gain_dB < IFX_IF_GAIN_DB_LOWER_LIMIT) || (if_gain_dB > IFX_IF_GAIN_DB_UPPER_LIMIT))
        throw rdk::exception::error::exception(IFX_ERROR_IF_GAIN_OUT_OF_RANGE);

    // HP Gain should be chosen as high as possible and VGA gain as low as
    // possible to minimize noise.

    // Choose hp_gain as high as possible
    const auto hp_gain = (if_gain_dB >= 30)
        ? Avian::Hp_Gain::_30dB
        : Avian::Hp_Gain::_18dB;

    // VGA gain can be 0dB, 5dB, 10dB, ..., 30dB.
    // Choose vga_gain such that hp_gain + vga_gain is as close to if_gain_dB
    // as possible.
    const int gain_left_dB = if_gain_dB - ((hp_gain == Avian::Hp_Gain::_30dB) ? 30 : 18);
    const div_t div = std::div(gain_left_dB, 5); // 5 because of the steps of 5dB
    const auto vga_gain = (div.rem >= 3)
        ? Avian::Vga_Gain(div.quot + 1) // round up (hp_gain+vga_gain > if_gain_dB)
        : Avian::Vga_Gain(div.quot);    // round down (hp_gain+vga_gain <= if_gain_dB)

    Avian::Baseband_Configuration baseband_config{};
    baseband_config.hp_gain_1 = hp_gain;
    baseband_config.hp_cutoff_1_kHz = hp_cutoff_kHz;
    baseband_config.vga_gain_1 = vga_gain;
    baseband_config.hp_gain_2 = hp_gain;
    baseband_config.hp_cutoff_2_kHz = hp_cutoff_kHz;
    baseband_config.vga_gain_2 = vga_gain;
    baseband_config.hp_gain_3 = hp_gain;
    baseband_config.hp_cutoff_3_kHz = hp_cutoff_kHz;
    baseband_config.vga_gain_3 = vga_gain;
    baseband_config.hp_gain_4 = hp_gain;
    baseband_config.hp_cutoff_4_kHz = hp_cutoff_kHz;
    baseband_config.vga_gain_4 = vga_gain;
    baseband_config.reset_period_100ps = reset_timer_period_100ps;

    return baseband_config;
}


//----------------------------------------------------------------------------

void RadarDeviceBase::stop_and_reset()
{
    if (m_driver)
    {
        const auto rc = m_driver->stop_and_reset_sequence();
        const ifx_Error_t error = RadarDeviceErrorTranslator::translate_error_code(rc);
        if(error == IFX_OK)
        {
            m_acquisition_state = Acquisition_State_t::Stopped;
        }
        else
        {
            m_acquisition_state = Acquisition_State_t::Error;
            throw rdk::exception::exception(error);
        }
    }
}

//----------------------------------------------------------------------------

std::vector<uint32_t> RadarDeviceBase::get_hp_cutoff_list(const ifx_Avian_Device_t* handle) const
{
    auto sensor_type = m_driver->get_device_type();
    const auto& device_traits = Avian::Device_Traits::get(sensor_type);

    std::vector<uint32_t> vector_of_hp_cutoffs{};
    for (const auto cutoff_kHz : device_traits.hpf_cutoff_settings)
    {
        if (cutoff_kHz == 0)
            break;
        vector_of_hp_cutoffs.push_back(static_cast<uint32_t>(cutoff_kHz) * 1000u); // convert from kHz to Hz
    }

    return vector_of_hp_cutoffs;
}

//----------------------------------------------------------------------------

std::vector<uint32_t> RadarDeviceBase::get_aaf_cutoff_list(const ifx_Avian_Device_t* handle) const
{
    auto sensor_type = m_driver->get_device_type();
    const auto& device_traits = Avian::Device_Traits::get(sensor_type);

    std::vector<uint32_t> vector_of_aaf_cutoffs{};
    for (const auto cutoff_kHz : device_traits.aaf_cutoff_settings)
    {
        if (cutoff_kHz == 0)
            break;
        vector_of_aaf_cutoffs.push_back(static_cast<uint32_t>(cutoff_kHz) * 1000u); // convert from kHz to Hz
    }

    return vector_of_aaf_cutoffs;
}

ifx_Float_t RadarDeviceBase::get_min_chirp_repetition_time(const ifx_Avian_Config_t& config) const
{
    double sampling_time = ifx_avian_calc_sampling_time(m_calc, config.num_samples_per_chirp, config.sample_rate_Hz);
    return static_cast<ifx_Float_t>(ifx_avian_calc_min_chirp_repetition_time(m_calc, sampling_time));
}

ifx_Float_t RadarDeviceBase::get_min_frame_repetition_time(const ifx_Avian_Config_t& config) const
{
    bool is_mimo = (config.mimo_mode == ifx_Avian_MIMO_Mode::IFX_MIMO_TDM);
    double min_prt = config.chirp_repetition_time_s;

    if (is_mimo)
        min_prt *= 2;  // As two shapes are configured in MIMO mode

    return static_cast<ifx_Float_t>(ifx_avian_calc_min_frame_repetition_time(m_calc, min_prt, config.num_chirps_per_frame));
}

ifx_Float_t RadarDeviceBase::get_temperature()
{
    throw rdk::exception::not_supported();
}

ifx_Cube_R_t* RadarDeviceBase::get_next_frame(ifx_Cube_R_t* frame, uint16_t timeout_ms)
{
    throw rdk::exception::not_supported();
}

ifx_Float_t RadarDeviceBase::get_tx_power(uint8_t tx_antenna)
{
    throw rdk::exception::not_supported();
}

BoardInstance* RadarDeviceBase::get_strata_avian_board() const
{
    throw rdk::exception::not_supported();
}

Avian::StrataPort* RadarDeviceBase::get_strata_avian_port() const
{
    throw rdk::exception::not_supported();
}

Avian::Constant_Wave_Controller* RadarDeviceBase::get_constant_wave_controller()
{
    throw rdk::exception::not_supported();
}

void RadarDeviceBase::start_acquisition()
{
    throw rdk::exception::not_supported();
}

void RadarDeviceBase::stop_acquisition()
{
    throw rdk::exception::not_supported();
}