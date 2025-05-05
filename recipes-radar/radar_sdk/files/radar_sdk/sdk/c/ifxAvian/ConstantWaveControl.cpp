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

#include "ifxBase/Types.h"
#include "ifxBase/Error.h"
#include "ifxBase/List.h"
#include "ifxBase/Vector.h"

#include "ifxAvian/DeviceConfig.h"

#include "ifxAvian/DeviceControlHelper.hpp"

#include "ifxAvian_CwController.hpp"
#include "ifxAvian_Utilities.hpp"
#include "ConstantWaveControl.h"
#include <stdexcept>
#include <platform/exception/EConnection.hpp>

/*
==============================================================================
2. LOCAL DEFINITIONS
==============================================================================
*/

/*
==============================================================================
3. LOCAL TYPES
==============================================================================
*/

struct ifx_Radar_CW_Controller
{
    ifx_Radar_CW_Controller(Infineon::Avian::Constant_Wave_Controller* cw);
    ~ifx_Radar_CW_Controller();
    Infineon::Avian::Constant_Wave_Controller* m_cw;

    ifx_Avian_Baseband_Config_t baseband_configs;
    ifx_Avian_ADC_Config_t adc_configs;
    ifx_Avian_Test_Signal_Generator_t test_signal_configs;
    ifx_Avian_CW_Config_t default_configs;
};

/*
==============================================================================
5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

static uint32_t get_tx_antenna_mask(Infineon::Avian::Constant_Wave_Controller* cw_handle);
static uint32_t get_rx_antenna_mask(Infineon::Avian::Constant_Wave_Controller* cw_handle);
static uint32_t get_tx_antenna_enabled_count(Infineon::Avian::Constant_Wave_Controller* cw_handle);
static uint32_t get_rx_antenna_enabled_count(Infineon::Avian::Constant_Wave_Controller* cw_handle);

/*
==============================================================================
6. LOCAL FUNCTIONS
==============================================================================
*/


static uint32_t get_tx_antenna_mask(Infineon::Avian::Constant_Wave_Controller* cw_handle)
{
    IFX_ERR_BRV_NULL(cw_handle, 0);

    uint32_t tx_mask = 0;

    const uint32_t NUM_TX_ANTENNAS = cw_handle->get_number_of_tx_antennas();

    for (uint32_t antenna = 0; antenna < NUM_TX_ANTENNAS; antenna++)
    {
        if (cw_handle->is_tx_antenna_enabled(antenna))
            tx_mask |= (0x1 << antenna);
    }

    return tx_mask;
}

static uint32_t get_rx_antenna_mask(Infineon::Avian::Constant_Wave_Controller* cw_handle)
{
    IFX_ERR_BRV_NULL(cw_handle, 0);

    uint32_t rx_mask = 0;

    const uint32_t NUM_RX_ANTENNAS = cw_handle->get_number_of_rx_antennas();

    for (uint32_t antenna = 0; antenna < NUM_RX_ANTENNAS; antenna++)
    {
        if (cw_handle->is_rx_antenna_enabled(antenna))
            rx_mask |= (0x1 << antenna);
    }

    return rx_mask;
}

static uint32_t get_tx_antenna_enabled_count(Infineon::Avian::Constant_Wave_Controller* cw_handle)
{
    IFX_ERR_BRV_NULL(cw_handle, 0);

    uint32_t tx_enabled_count = 0;

    const uint32_t NUM_TX_ANTENNAS = cw_handle->get_number_of_tx_antennas();

    for (uint32_t antenna = 0; antenna < NUM_TX_ANTENNAS; antenna++)
    {
        if (cw_handle->is_tx_antenna_enabled(antenna))
            tx_enabled_count += 1;
    }

    return tx_enabled_count;
}

static uint32_t get_rx_antenna_enabled_count(Infineon::Avian::Constant_Wave_Controller* cw_handle)
{
    IFX_ERR_BRV_NULL(cw_handle, 0);

    uint32_t rx_enabled_count = 0;

    const uint32_t NUM_RX_ANTENNAS = cw_handle->get_number_of_rx_antennas();

    for (uint32_t antenna = 0; antenna < NUM_RX_ANTENNAS; antenna++)
    {
        if (cw_handle->is_rx_antenna_enabled(antenna))
            rx_enabled_count += 1;
    }

    return rx_enabled_count;
}
/**
 * @brief This function disables cw mode and ignoring it's error.
 * 
 * Useful for handling error situations
 * 
 * @param handle of CW device
 */
static void disable_cw_force(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRK_NULL(handle);
    try
    {
        handle->m_cw->enable_constant_wave(false);
    } catch(...){
        //EMPTY
    }
}

ifx_Radar_CW_Controller::ifx_Radar_CW_Controller(Infineon::Avian::Constant_Wave_Controller* cw) : 
    m_cw(cw)
{
    baseband_configs.vga_gain = ifx_Avian_Baseband_Vga_Gain(cw->get_vga_gain());
    baseband_configs.hp_gain = ifx_Avian_Baseband_Hp_Gain(cw->get_hp_gain());
    baseband_configs.hp_cutoff_Hz = 1000u * cw->get_hp_cutoff(); // kHz to Hz
    baseband_configs.aaf_cutoff_Hz = 1000u * cw->get_aaf_cutoff();  // kHz to Hz

    adc_configs.sample_time = ifx_Avian_ADC_SampleTime (cw->get_adc_sample_time());
    adc_configs.tracking = ifx_Avian_ADC_Tracking (cw->get_adc_tracking());
    adc_configs.oversampling = ifx_Avian_ADC_Oversampling (cw->get_adc_oversampling());
    adc_configs.double_msb_time = cw->get_adc_double_msb_time();
    adc_configs.samplerate_Hz = uint32_t(cw->get_sampling_rate());

    test_signal_configs.frequency_Hz = cw->get_test_signal_frequency();
    test_signal_configs.mode = ifx_Avian_Test_Signal_Generator_Mode(cw->get_test_signal_generator_mode());

    // initialize default settings struct, once CW controller created.

    default_configs.tx_mask = get_tx_antenna_mask(cw);
    default_configs.rx_mask = get_rx_antenna_mask(cw);
    default_configs.duty_cycle = 100; // hard coded 100% duty cycle

    std::pair<uint32_t, uint32_t> num_of_samples = cw->get_number_of_samples_range();
    default_configs.num_of_samples.value = cw->get_number_of_samples();
    default_configs.num_of_samples.min = num_of_samples.first;
    default_configs.num_of_samples.max = num_of_samples.second;

    std::pair<uint32_t, uint32_t> tx_dac_value = cw->get_constant_wave_power_range();
    default_configs.tx_dac_value.value = cw->get_constant_wave_power();
    default_configs.tx_dac_value.min = tx_dac_value.first;
    default_configs.tx_dac_value.max = tx_dac_value.second;

    std::pair<double, double> rf_freq_Hz = cw->get_constant_wave_frequency_range();
    default_configs.rf_freq_Hz.value = static_cast<uint64_t>(cw->get_constant_wave_frequency());
    default_configs.rf_freq_Hz.min = static_cast<uint64_t>(rf_freq_Hz.first);
    default_configs.rf_freq_Hz.max = static_cast<uint64_t>(rf_freq_Hz.second);

    std::pair<ifx_Float_t, ifx_Float_t> sampling_range = cw->get_sampling_rate_range();
    default_configs.sample_rate_range_Hz.min = uint32_t(sampling_range.first);
    default_configs.sample_rate_range_Hz.max = uint32_t(sampling_range.second);
    default_configs.adc_configs = adc_configs;
        
    std::pair<ifx_Float_t, ifx_Float_t> test_signal_freq_Hz = cw->get_test_signal_frequency_range();
    default_configs.test_signal_freq_range_Hz.min = test_signal_freq_Hz.first;
    default_configs.test_signal_freq_range_Hz.max = test_signal_freq_Hz.second;
    default_configs.test_signal_configs = test_signal_configs;

    default_configs.baseband_configs = baseband_configs;
}

ifx_Radar_CW_Controller::~ifx_Radar_CW_Controller()
{

}

/*
==============================================================================
7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_Avian_CW_Control_Handle_t ifx_avian_cw_create(ifx_Avian_Device_t* device_handle)
{
    IFX_ERR_BRN_NULL(device_handle);
    return new ifx_Radar_CW_Controller(ifx_avian_get_constant_wave_controller(device_handle));
}

void ifx_avian_cw_destroy(ifx_Avian_CW_Control_Handle_t handle)
{
    if (!handle)
        return;

    ifx_avian_cw_stop_emission(handle);

    delete handle;
}

void ifx_avian_cw_get_default_config(ifx_Avian_CW_Control_Handle_t handle, ifx_Avian_CW_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    *config = handle->default_configs;
}

bool ifx_avian_cw_enabled(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, false);

    return handle->m_cw->is_constant_wave_enabled();
}

void ifx_avian_cw_start_emission(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRK_NULL(handle);

    try
    {
        handle->m_cw->enable_constant_wave(true);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_INTERNAL);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot start emission: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot start emission: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

void ifx_avian_cw_stop_emission(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRK_NULL(handle);

    try
    {
        handle->m_cw->enable_constant_wave(false);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_INTERNAL);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot start emission: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot start emission: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

void ifx_avian_cw_set_rf_frequency(ifx_Avian_CW_Control_Handle_t handle, uint64_t frequency_Hz)
{
    IFX_ERR_BRK_NULL(handle);

    try
    {
        handle->m_cw->set_constant_wave_frequency(static_cast<double>(frequency_Hz));
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_RF_OUT_OF_RANGE);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set RF frequency: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set RF frequency: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

uint64_t ifx_avian_cw_get_rf_frequency(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return static_cast<uint64_t>(handle->m_cw->get_constant_wave_frequency());
}

void ifx_avian_cw_set_tx_dac_value(ifx_Avian_CW_Control_Handle_t handle, uint32_t dac_value)
{
    IFX_ERR_BRK_NULL(handle);

    try
    {
        handle->m_cw->set_constant_wave_power(dac_value);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_TX_POWER_OUT_OF_RANGE);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set DAC value: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set DAC value: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

uint32_t ifx_avian_cw_get_tx_dac_value(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return  handle->m_cw->get_constant_wave_power();
}

void ifx_avian_cw_enable_tx_antenna(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna, bool enable)
{
    IFX_ERR_BRK_NULL(handle);

    try
    {
        handle->m_cw->enable_tx_antenna(antenna, enable);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_TX_ANTENNA_COMBINATION_NOT_ALLOWED);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot enable antenna: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot enable antenna: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

bool ifx_avian_cw_tx_antenna_enabled(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna)
{
    IFX_ERR_BRV_NULL(handle, false);
    try
    {
        return handle->m_cw->is_tx_antenna_enabled(antenna);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_TX_ANTENNA_COMBINATION_NOT_ALLOWED);
        return false;
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot reach antenna: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        return false;
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot reach antenna: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
        return false;
    }
}

void ifx_avian_cw_enable_rx_antenna(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna, bool enable)
{
    IFX_ERR_BRK_NULL(handle);

    try
    {
        handle->m_cw->enable_rx_antenna(antenna, enable);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_RX_ANTENNA_COMBINATION_NOT_ALLOWED);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot enable antenna: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot enable antenna: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

bool ifx_avian_cw_rx_antenna_enabled(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna)
{
    IFX_ERR_BRV_NULL(handle, false);

    try
    {
        return handle->m_cw->is_rx_antenna_enabled(antenna);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_RX_ANTENNA_COMBINATION_NOT_ALLOWED);
        return false;
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot reach antenna: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        return false;
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot reach antenna: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
        return false;
    }
}

void ifx_avian_cw_set_num_of_samples_per_antenna(ifx_Avian_CW_Control_Handle_t handle, uint32_t samples)
{
    IFX_ERR_BRK_NULL(handle);
    /* DataAvian does not allow configuring an odd number of samples, because
     * it is not possible to read out an odd number from the BGT FIFO (since
     * 2 samples are packed into 3 bytes...).
     * For this reason we have to make sure that the number of samples is an
     * even number
     */
    IFX_ERR_BRK_COND((samples % 2) != 0, IFX_ERROR_ARGUMENT_INVALID);

    try
    {
        handle->m_cw->set_number_of_samples(samples);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_NUM_SAMPLES_OUT_OF_RANGE);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set num of samples: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set num of samples: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

uint32_t ifx_avian_cw_get_num_of_samples_per_antenna(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return handle->m_cw->get_number_of_samples();
}

void ifx_avian_cw_set_sampling_rate_Hz(ifx_Avian_CW_Control_Handle_t handle, ifx_Float_t frequency_Hz)
{
    IFX_ERR_BRK_NULL(handle);
    try
    {
        handle->m_cw->set_sampling_rate(frequency_Hz);
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_SAMPLERATE_OUT_OF_RANGE);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot get num of samples: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot get num of samples: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }
}

ifx_Float_t ifx_avian_cw_get_sampling_rate(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return handle->m_cw->get_sampling_rate();
}

ifx_Error_t ifx_avian_cw_get_sampling_rate_limits(ifx_Avian_CW_Control_Handle_t handle,
                                                   const ifx_Avian_ADC_Config_t* config,
                                                   ifx_Float_t* min_sample_rate,
                                                   ifx_Float_t* max_sample_rate)
{
    using namespace Infineon::Avian;

    IFX_ERR_BRV_NULL(handle, IFX_ERROR_ARGUMENT_NULL);
    IFX_ERR_BRV_NULL(config, IFX_ERROR_ARGUMENT_NULL);
    IFX_ERR_BRV_NULL(min_sample_rate, IFX_ERROR_ARGUMENT_NULL);
    IFX_ERR_BRV_NULL(max_sample_rate, IFX_ERROR_ARGUMENT_NULL);

    /*
     * Note:
     * Optionally these function take also the reference clock frequency as
     * additional parameter. At this place this ignored, because official
     * Infineon hardware always uses the recommended clock frequency of 80MHz.
     *
     * Changing the clock frequency is a lab feature, which is needed for some
     * special lab testing boards only. It is currently not supported by the RDK,
     * so the clock frequency is ignored here.
     */
    *min_sample_rate = get_min_sampling_rate();
    *max_sample_rate = get_max_sampling_rate(Adc_Sample_Time(config->sample_time),
                                             Adc_Tracking(config->tracking),
                                             config->double_msb_time,
                                             Adc_Oversampling(config->oversampling));

    return IFX_OK;
}

void ifx_avian_cw_set_baseband_params(ifx_Avian_CW_Control_Handle_t handle, const ifx_Avian_Baseband_Config_t* config)
{
    using namespace Infineon::Avian;
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    try
    {
        handle->m_cw->set_vga_gain(Vga_Gain(config->vga_gain));
        handle->m_cw->set_hp_gain(Hp_Gain(config->hp_gain));
        handle->m_cw->set_hp_cutoff(config->hp_cutoff_Hz / 1000u); // Hz to kHz
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_BASEBAND_CONFIG_NOT_ALLOWED);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set baseband params: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set baseband params: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }

    try
    {
        handle->m_cw->set_aaf_cutoff(config->aaf_cutoff_Hz / 1000); // Hz to kHz
        handle->baseband_configs.aaf_cutoff_Hz = config->aaf_cutoff_Hz;
    }
    catch (const std::runtime_error&)
    {
        // suppress error as AAF cutoff is not valid for C Avian class of devices.
    }
    catch (const EConnection&)
    {
        // suppress error as AAF cutoff is not valid for C Avian class of devices.
    }
    catch (const EException&)
    {
        // suppress error as AAF cutoff is not valid for C Avian class of devices. 
    }

    handle->baseband_configs.vga_gain = config->vga_gain;
    handle->baseband_configs.hp_gain = config->hp_gain;
    handle->baseband_configs.hp_cutoff_Hz = config->hp_cutoff_Hz;
}

const ifx_Avian_Baseband_Config_t* ifx_avian_cw_get_baseband_params(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return &handle->baseband_configs;
}

void ifx_avian_cw_set_adc_params(ifx_Avian_CW_Control_Handle_t handle, const ifx_Avian_ADC_Config_t* config)
{
    using namespace Infineon::Avian;
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    try
    {
        handle->m_cw->set_adc_sample_time(Adc_Sample_Time(config->sample_time));
        handle->m_cw->set_adc_tracking(Adc_Tracking(config->tracking));
        handle->m_cw->set_adc_oversampling(Adc_Oversampling(config->oversampling));
        handle->m_cw->set_adc_double_msb_time(config->double_msb_time);
        handle->m_cw->set_sampling_rate(ifx_Float_t(config->samplerate_Hz));
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_ADC_CONFIG_NOT_ALLOWED);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set adc params: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set adc params: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }

    handle->adc_configs.samplerate_Hz = uint32_t(handle->m_cw->get_sampling_rate());
    handle->adc_configs.sample_time = config->sample_time;
    handle->adc_configs.tracking = config->tracking;
    handle->adc_configs.oversampling = config->oversampling;
    handle->adc_configs.double_msb_time = config->double_msb_time;
}

const ifx_Avian_ADC_Config_t* ifx_avian_cw_get_adc_params(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return &handle->adc_configs;
}

void ifx_avian_cw_set_test_signal_generator_config(ifx_Avian_CW_Control_Handle_t handle, const ifx_Avian_Test_Signal_Generator_t* config)
{
    using namespace Infineon::Avian;
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    try
    {
        handle->m_cw->set_test_signal_frequency(config->frequency_Hz);
        handle->m_cw->set_test_signal_generator_mode(Constant_Wave_Controller::Test_Signal_Generator_Mode(config->mode));
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_TEST_SIGNAL_MODE_NOT_ALLOWED);
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set test signal generator: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot set test signal generator: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }

    handle->test_signal_configs.frequency_Hz = config->frequency_Hz;
    handle->test_signal_configs.mode = config->mode;
}

const ifx_Avian_Test_Signal_Generator_t* ifx_avian_cw_get_test_signal_generator_config(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return &handle->test_signal_configs;
}

ifx_Float_t ifx_avian_cw_measure_temperature(ifx_Avian_CW_Control_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    try
    {
        return handle->m_cw->measure_temperature();
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_TEMPERATURE_MEASUREMENT_FAILED);
        return -1;
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot measure temperature: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        return -1;
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot measure temperature: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
        return -1;
    }
}

ifx_Float_t ifx_avian_cw_measure_tx_power(ifx_Avian_CW_Control_Handle_t handle, const uint32_t antenna)
{
    IFX_ERR_BRV_NULL(handle, 0);
    try
    {
        return handle->m_cw->measure_tx_power(antenna);      
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        ifx_error_set(IFX_ERROR_POWER_MEASUREMENT_FAILED);
        return -1;
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot measure TX power: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        return -1;
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot measure TX power: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
        return -1;
    }
}

ifx_Matrix_R_t* ifx_avian_cw_capture_frame(ifx_Avian_CW_Control_Handle_t handle, ifx_Matrix_R_t* frame)
{
    IFX_ERR_BRN_NULL(handle);

    if (frame == nullptr)
    {
        frame = ifx_mat_create_r(get_tx_antenna_enabled_count(handle->m_cw) *
            get_rx_antenna_enabled_count(handle->m_cw), handle->m_cw->get_number_of_samples());
    }
    if (ifx_error_get() != IFX_OK)
    {
        return frame;
    }

    std::map<uint32_t, std::vector<ifx_Float_t>> signals;
    uint8_t num_rx = 0;

    try
    {
        signals = handle->m_cw->capture_rx_signals();
    }
    catch (const std::runtime_error& e)
    {
        (void)e;
        IFX_LOG_ERROR(e.what());
        disable_cw_force(handle);
        ifx_error_set(IFX_ERROR_FRAME_ACQUISITION_FAILED);
        return frame;
    }
    catch (const EConnection& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot capture frame: %s (Strata EConnection exception)", e.what());
        disable_cw_force(handle);
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        return frame;
    }
    catch (const EException& e)
    {
        (void)e;
        IFX_LOG_ERROR("Cannot capture frame: %s (Strata EException exception)", e.what());
        disable_cw_force(handle);
        ifx_error_set(IFX_ERROR);
        return frame;
    }
    
    for (auto it = signals.cbegin(); it != signals.cend(); ++it)
    {
        for (uint32_t sample = 0; sample < it->second.size(); sample ++)
        {
            IFX_MAT_AT(frame, num_rx, sample) = static_cast<ifx_Float_t>(it->second[sample]);
        }
        num_rx++;
    }

    return frame;
}

