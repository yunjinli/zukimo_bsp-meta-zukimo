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
* @file ConstantWaveControl.h
*
* @brief Header for Constant Wave Control. It provides APIs to use an Avian device in constant wave mode. All relevant
* parameters for the CW mode are set/get through these APIs. It also provides an interface to acquire data received 
* through the RX antennas.
*/

#ifndef IFX_AVIAN_CW_CONTROL_H
#define IFX_AVIAN_CW_CONTROL_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
1. INCLUDE FILES
==============================================================================
*/

#include <ifxAvian/DeviceControl.h>
#include <ifxAvian/DeviceConfig.h>

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

typedef struct ifx_Radar_CW_Controller* ifx_Avian_CW_Control_Handle_t;

/**
 * @brief Used to define a default set for the configuration values including their min and max values.
 */
typedef struct {	
	uint32_t tx_mask;   /**< Defines active TX antennas within device. Each antenna is assigned to a separate bit starting from LSB */
	uint32_t rx_mask;	/**< Defines active RX antennas within device. Each antenna is assigned to a separate bit starting from LSB */
	uint8_t duty_cycle;	/**< Defines the percentage of time period (time period is 1sec), where device emits signal via TX antenna */

	struct { uint32_t value; uint32_t min; uint32_t max; } num_of_samples;		/**< Defines number of samples per active RX antenna to be stored in raw data frame */
	struct { uint32_t value; uint32_t min; uint32_t max; } tx_dac_value;		/**< Defines the TX power level in terms of DAC value */
	struct { uint64_t value; uint64_t min; uint64_t max; } rf_freq_Hz;			/**< Defines the RF frequency in Hz */

	struct { uint32_t min; uint32_t max; } sample_rate_range_Hz;				/**< Defines the valid range of sampling rate in Hz */
	struct { ifx_Float_t min; ifx_Float_t max; } test_signal_freq_range_Hz;		/**< Defines the valid range of test signal frequency in Hz */

	ifx_Avian_ADC_Config_t adc_configs;					/**< Defines the ADC configurations */
	ifx_Avian_Baseband_Config_t baseband_configs;			/**< Defines the Baseband configurations */
	ifx_Avian_Test_Signal_Generator_t test_signal_configs; /**< Defines the test signal configurations */
	
} ifx_Avian_CW_Config_t;

/*
==============================================================================
4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_Avian
  * @{
  */

 /** @defgroup gr_constantwave Constant Wave
  * @brief API for Radar constant wave operations
  * @{
  */

/**
* @brief This function creates and initializes the constant wave mode controller instance.
*
* The provided port within the device handle is used to detect the sensor type of the connected device.
* According to the detected sensor type the CW mode controller is
* initialized. If the detected device is not recognized, an error is set.
* 
* Corresponding destroy method is not provided for CW mode. Reason is CW controller is part of 
* device handle. In order to destroy a CW controller, application must destroy device handle.
*
* After initialization the connection is still in deep sleep mode.
*
* @param [in]    handle    A device handle created before calling \ref ifx_avian_cw_create
* 
* @return A handle to the CW controller, used to control device for CW mode
*/
IFX_DLL_PUBLIC 
ifx_Avian_CW_Control_Handle_t ifx_avian_cw_create(ifx_Avian_Device_t* handle);

/**
 * @brief Delete instance of controller
 *  
 * If emission is ongoing it will be stopped.<BR>
 * This function doesn't delete device that was provided on create.  
 * 
 * @param [in] handle   A cw controller handle to be destroyed
 */
IFX_DLL_PUBLIC
void ifx_avian_cw_destroy(ifx_Avian_CW_Control_Handle_t handle);

/**
 * @brief Get a default configuration for the Constant wave controller
 *
 * This function provides a default set for the configuration values including their min and max values.
 *
 * In case something goes wrong, the ifx_error_set is used. Please read back the status with ifx_error_get or ifx_error_get_and_clear
 *
 * @param [in]     handle    A handle to the cw controller
 * @param [out]	   config    structure where the default configuration will be stored
 *
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_get_default_config(ifx_Avian_CW_Control_Handle_t handle, ifx_Avian_CW_Config_t* config);

/**
* @brief This function gives the emission status in constant wave mode.
*
* @param [in]     handle    A handle to the cw controller
*
* @retval         Returns "true" if CW is enabled and transmitting signal, else returns "false"
*/
IFX_DLL_PUBLIC
bool ifx_avian_cw_enabled(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This function enables constant wave emission.
*
* The Avian device is set to Active mode, and constant
* wave emission starts.
*
* In case the constant wave can't be enabled due to hardware failure, an error is set.
* 
* @param [in]     handle    A handle to the cw controller
* 
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_start_emission(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This function disables constant wave emission.
*
* The Avian device is set to Deep Sleep mode, and constant
* wave emission stops.
*
* In case the constant wave can't be enabled due to hardware failure, an error is set.
*
* @param [in]     handle    A handle to the cw controller
*
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_stop_emission(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method sets the frequency of the constant wave in Hz. 
* 
* Furthermore the frequency is internally rounded to meet hardware constraints. It's
* recommended to read back the applied frequency using \ref ifx_avian_cw_get_rf_frequency.
* 
* @param [in]     handle		A handle to the cw controller
* @param [in]     frequency_Hz  Desired center RF frequency within valid range of device
* 
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_set_rf_frequency(ifx_Avian_CW_Control_Handle_t handle, uint64_t frequency_Hz);

/**
* @brief This method gets the currently set device frequency of the constant wave in Hz.
*
* @param [in]     handle		A handle to the cw controller
* @retval         frequency_Hz  Applied center RF frequency in the device
*
*/
IFX_DLL_PUBLIC
uint64_t ifx_avian_cw_get_rf_frequency(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method sets the DAC value that controls the power of the constant wave
* 
* It's recommended to read back the applied value using \ref ifx_avian_cw_get_tx_dac_value.
* 
* @param [in]     handle		A handle to the cw controller
* @param [in]     dac_value     Desired TX power level within valid range of device
* 
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_set_tx_dac_value(ifx_Avian_CW_Control_Handle_t handle, uint32_t dac_value);

/**
* @brief This method gets the currently set TX power for constant wave mode within device.
*
* @param [in]     handle		A handle to the cw controller
* @retval         dac_value     Applied TX power level in the device
*
*/
IFX_DLL_PUBLIC
uint32_t ifx_avian_cw_get_tx_dac_value(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method enables or disables the specified TX antenna.
*
* @param [in]     handle		A handle to the cw controller
* @param [in]	  antenna		The index of the antenna to be enabled or disabled
*								The value is 0 based and must be less than the total
*                               number of physical TX antennas within device. If the
*								value is not in the allowed range, an error is set
* @param [in]	  enable		If this is true, the antenna is enabled. Otherwise it's
*								disabled
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_enable_tx_antenna(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna, bool enable);

/**
* @brief This method checks if the specified TX antenna is enabled.
*
* @param [in]     handle		A handle to the cw controller
* @param[in]	  antenna		The index of the antenna to be enabled or disabled.
*								The value is 0 based and must be less than the total
*                               number of physical TX antennas within device. If the
*								value is not in the allowed range, an error is set.
* @return	If the antenna is already enabled, true is returned. Otherwise false is returned.
*
*/
IFX_DLL_PUBLIC
bool ifx_avian_cw_tx_antenna_enabled(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna);

/**
* @brief This method enables or disables the specified Rx antenna.
*
* @param [in]     handle		A handle to the cw controller
* @param [in]	  antenna		The index of the antenna to be enabled or disabled
*								The value is 0 based and must be less than the total
*                               number of physical Rx antennas within device. If the
*								value is not in the allowed range, an error is set
* @param [in]	  enable		If this is true, the antenna is enabled. Otherwise it's
*								disabled
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_enable_rx_antenna(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna, bool enable);

/**
* @brief This method checks if the specified Rx antenna is enabled.
*
* @param [in]     handle		A handle to the cw controller
* @param[in]	  antenna		The index of the antenna to be enabled or disabled
*								The value is 0 based and must be less than the total
*                               number of physical Rx antennas within device. If the
*								value is not in the allowed range, an error is set
* @return	If the antenna is already enabled, true is returned. Otherwise false is returned.
*
*/
IFX_DLL_PUBLIC
bool ifx_avian_cw_rx_antenna_enabled(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna);

/**
* @brief This method sets the number of samples per Rx antenna to be captured in single shot.
*
* All active Rx antennas have same number of samples. There is no need to pass antenna index.
*
* The number of samples must be an even number. If samples is an odd number the error
* \ref IFX_ERROR_ARGUMENT_INVALID is set.
*
* @param [in]     handle		A handle to the cw controller
* @param [in]	  samples		The number of samples per Rx antenna to be captured in one shot
*
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_set_num_of_samples_per_antenna(ifx_Avian_CW_Control_Handle_t handle, uint32_t samples);

/**
* @brief This method provides samples count acquired per RX antenna.
*
* @param [in]     handle		A handle to the cw controller
* @return	      The number of samples per Rx antenna to be captured in one shot
*
*/
IFX_DLL_PUBLIC
unsigned int ifx_avian_cw_get_num_of_samples_per_antenna(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method configures the Avian device's baseband, defined by \ref ifx_Avian_Baseband_Config_t.
*
* It includes the configuration of gain and cutoff frequency of the high pass and anti aliasing filter and
* VGA gain of baseband amplifier. It's recommended to read back the applied value using 
* \ref ifx_avian_cw_get_baseband_params
* 
* @param [in]     handle		A handle to the cw controller
* @param [in]	  config	    Baseband configurations defined by \ref ifx_Avian_Baseband_Config_t
*
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_set_baseband_params(ifx_Avian_CW_Control_Handle_t handle, const ifx_Avian_Baseband_Config_t* config);

/**
* @brief It provides the current baseband configuration of Avian device.
*
* @param [in]     handle		A handle to the cw controller
* @return	      Current baseband configurations defined by \ref ifx_Avian_Baseband_Config_t
*
*/
IFX_DLL_PUBLIC
const ifx_Avian_Baseband_Config_t* ifx_avian_cw_get_baseband_params(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method configures the Avian device's ADC parameters, defined by \ref ifx_Avian_ADC_Config_t.
*
* It includes the configuration of Sample Time \ref ifx_Avian_ADC_SampleTime, tracking or number of conversion
* \ref ifx_Avian_ADC_Tracking, oversampling factor \ref ifx_Avian_ADC_Oversampling and enable/disable of double the MSB time.
* 
* It's recommended to read back the applied value using \ref ifx_avian_cw_get_adc_params.
*
* @param [in]     handle		A handle to the cw controller
* @param [in]	  config	    ADC configurations defined by \ref ifx_Avian_ADC_Config_t
*
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_set_adc_params(ifx_Avian_CW_Control_Handle_t handle, const ifx_Avian_ADC_Config_t* config);

/**
* @brief It provides the current ADC configurations of Avian device.
*
* @param [in]     handle		A handle to the cw controller
* 
* @return	      Current ADC configurations defined by \ref ifx_Avian_ADC_Config_t
*
*/
IFX_DLL_PUBLIC
const ifx_Avian_ADC_Config_t* ifx_avian_cw_get_adc_params(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief It provides the ADC sampling rate limits based on \ref ifx_Avian_ADC_Config_t.
*
* Sample rate is dependent on \ref ifx_Avian_ADC_Config_t. This method use \ref ifx_Avian_ADC_Config_t to calculate sampling rate limits.
* This method does not use \ref ifx_Avian_ADC_Config_t applied currently to the device.
*
* @param [in]     handle			A handle to the cw controller
* @param [in]     config			\ref ifx_Avian_ADC_Config_t used to calculate sample rate limits
* 
* @param [out]    min_sample_rate	Lower limit of sample rate in Hz
* @param [out]    max_sample_rate	Upper limit of sample rate in Hz
*
* @return	If any argument of this method is NULL then returns IFX_ERROR_ARGUMENT_NULL else IFX_OK.
*/
IFX_DLL_PUBLIC
ifx_Error_t ifx_avian_cw_get_sampling_rate_limits(ifx_Avian_CW_Control_Handle_t handle,
												   const ifx_Avian_ADC_Config_t* config,
												   ifx_Float_t* min_sample_rate,
												   ifx_Float_t* max_sample_rate);

/**
* @brief It provides the ADC sampling rate set in the device.
*
* @param [in]     handle		A handle to the cw controller
*
* @return  Return current sample rate in Hz used within device.
*
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_cw_get_sampling_rate(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method configures the Avian device's test signal generator instead of actually TX transmitted signal
*
* It is used for testing the IF section with a known signal without involving TX antennas.
*
* @param [in]     handle		A handle to the cw controller
* @param [in]	  config	    Test signal configurations defined by \ref ifx_Avian_Test_Signal_Generator_t
*
*/
IFX_DLL_PUBLIC
void ifx_avian_cw_set_test_signal_generator_config(ifx_Avian_CW_Control_Handle_t handle,
	                                                const ifx_Avian_Test_Signal_Generator_t* config);

/**
* @brief It provides the current test signal configurations of Avian device.
*
* @param [in]     handle		A handle to the cw controller
* 
* @return	      Current test signal configurations defined by \ref ifx_Avian_Test_Signal_Generator_t
*
*/
IFX_DLL_PUBLIC
const ifx_Avian_Test_Signal_Generator_t* ifx_avian_cw_get_test_signal_generator_config(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method measures the current temperature of the Avian device.
*
* This method will set an error in case of a hardware failure.
* 
* @param [in]     handle		A handle to the cw controller
*
* @return The measured device temperature in Celsius degree is returned.
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_cw_measure_temperature(ifx_Avian_CW_Control_Handle_t handle);

/**
* @brief This method measures returns the power that is emitted from the specified TX antenna
*
* The power measurement is only performed if the constant wave is enabled.
* 
* @param [in]     handle		A handle to the cw controller
* @param [in]     antenna		The index of the antenna to be enabled or disabled
*								The value is 0 based and must be less than the total
*                               number of physical TX antennas within device. If the
*								value is not in the allowed range, an error is set
*
* @return If the constant wave is enabled the measured TX power in dBm is
*         returned. Otherwise -infinity is returned.
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_cw_measure_tx_power(ifx_Avian_CW_Control_Handle_t handle, uint32_t antenna);

/**
* @brief This method captures one frame of raw data.
*
* This function captures for all activated antennas samples_per_antenna and
* returns the samples as a matrix with dimensions
* num_antennas (rows) x samples_per_antenna (columns).
*
* The samples of the raw data are normalized to values between -1...1. To
* convert the samples to voltages, each sample must be multiplied by the
* reference voltage 1.21V.
*
* If frame is NULL, memory for the matrix will be allocated and returned.
* Otherwise the memory of frame will be used.
*
* @param [in]     handle		A handle to the cw controller
* @param [in]     frame			Pointer to the \ref ifx_Matrix_R_t *frame* where raw data is stored. 
*                               If this is NULL, then a new frame is created. the caller is responsible to 
*                               deallocate the memory.
*
* @return	pointer to the \ref ifx_Matrix_R_t *frame* containing the received samples.
*
*/
IFX_DLL_PUBLIC
ifx_Matrix_R_t* ifx_avian_cw_capture_frame(ifx_Avian_CW_Control_Handle_t handle, ifx_Matrix_R_t* frame);
/**
  * @}
  */
 
/**
  * @}
  */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_AVIAN_CW_CONTROL_H */
