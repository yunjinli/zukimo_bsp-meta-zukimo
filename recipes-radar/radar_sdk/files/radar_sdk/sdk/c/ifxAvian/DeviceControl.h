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
 * @file DeviceControl.h
 *
 * \brief \copybrief gr_devicecontrol
 *
 * For details refer to \ref gr_devicecontrol
 */

#ifndef IFX_RADAR_DEVICE_CONTROL_H
#define IFX_RADAR_DEVICE_CONTROL_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "ifxBase/Types.h"
#include "ifxBase/Error.h"
#include "ifxBase/List.h"
#include "ifxBase/Cube.h"

#include "ifxAvian/DeviceConfig.h"
#include "ifxRecording/Recording.h"
#include "ifxRadarDeviceCommon/RadarDeviceCommon.h"

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

typedef struct RadarDeviceBase ifx_Avian_Device_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_Avian
  * @{
  */

/** @defgroup gr_devicecontrol Device Control
  * @brief API for Radar device control operations
  * @{
  */

/**
 * \brief This function returns a list of available devices.
 *
 * The function returns a list of available device radar devices. Each
 * list element is of type \ref ifx_Radar_Sensor_List_Entry_t.
 *
 * The list must be freed after use using \ref ifx_list_destroy.
 *
 * Here is an example how to use the function:
 * @code
 *      ifx_List_t* device_list = ifx_avian_get_list();
 *      for (size_t i = 0; i < ifx_list_size(device_list); i++)
 *      {
 *          ifx_Radar_Sensor_List_Entry_t* entry = ifx_list_get(device_list, i);
 *          // ...
 *      }
 *      ifx_list_destroy(device_list);
 * @endcode
 *
 * \return The function returns a list of all found devices.
 */
IFX_DLL_PUBLIC
ifx_List_t* ifx_avian_get_list(void);

/**
 * \brief This function returns a list of available specified devices.
 *
 * The function is similar to \ref ifx_avian_get_list. It returns a list of available
 * Avian radar sensors of type \ref ifx_Radar_Sensor_t. Each list element is of type
 * \ref ifx_Radar_Sensor_List_Entry_t. If the specified sensor is not
 * found, an empty list is returned.
 *
 * The list must be freed after use using \ref ifx_list_destroy.
 *
 * Here is an example how to use the function:
 * @code
 *      ifx_List_t* device_list = ifx_avian_get_list_by_sensor_type(IFX_AVIAN_BGT60TR13C);
 *      for (size_t i = 0; i < ifx_list_size(device_list); i++)
 *      {
 *          ifx_Radar_Sensor_List_Entry_t* entry = ifx_list_get(device_list, i);
 *          // ...
 *      }
 *      ifx_list_destroy(device_list);
 * @endcode
 *
 * \return The function returns a list of all found devices.
 *
 */
IFX_DLL_PUBLIC
ifx_List_t* ifx_avian_get_list_by_sensor_type(ifx_Radar_Sensor_t sensor_type);

/**
 * @brief Creates a dummy device handle.
 *
 * This function creates a dummy device.
 *
 * @param [in]  sensor_type    specialization of the dummy device.
 *
 * @return Handle to the newly created dummy instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Avian_Device_t* ifx_avian_create_dummy(ifx_Radar_Sensor_t sensor_type);

/**
 * @brief Creates a dummy device handle from a handle to a saved Common File Format recording.
 *
 * This function creates a special device handle, which uses a data saved in a recording
 * for its functionality, like getting configuration from a saved config.json file in a
 * Common File Format Recording or getting a saved radar frames data from a radar.npy.
 *
 * To get a frame from a device handle created using this function one can for example:
 * 
 *  @code
 *      ifx_Recording_t* recording_handle = ifx_recording_create("recording_dir", IFX_RECORDING_READ_MODE, IFX_RECORDING_AVIAN, 0);
 *      ifx_Avian_Device_t* device_handle = ifx_avian_create_dummy_from_recording(recording_handle, true);
 *      ifx_Cube_R_t* frame = NULL;
 *
 *      while(1)
 *      {
 *          frame = ifx_avian_get_next_frame(device_handle, frame);
 *          ifx_Error_t ret = ifx_error_get_and_clear();
 *          if(ret == IFX_ERROR_TIMEOUT)
 *              continue; // no data available, do something else
 *          else if(ret != IFX_OK)
 *              // error handling
 *              break;
 *
 *          // process data
 *          // ...
 *      }
 *
 *      ifx_cube_destroy_r(frame);
 *      ifx_avian_destroy_r(device_handle);
 *      ifx_recording_destroy_r(recording_handle);
 * @endcode *
 * 
 * For the code above, when one specify correct_timing parameter as true, the call of:
 * \ref ifx_avian_get_next_frame or \ref ifx_avian_get_next_frame_timeout will only give
 * a frame, when a proper amount of time (at least equal or larger than a frame repetition time
 * saved in config.json) have passed since a last call of the function or since the call
 * to start acquisition.
 * 
 * It also respects a given timeout like a normal device handle, so that
 * if timeout is given or a default of 10s is used, it will raise \ref IFX_ERROR_TIMEOUT,
 * but if correct_timing was set to false, it will return a frame regardless of the timeout.
 *
 * @param [in]  recording         a handle to the recording.
 * @param [in]  correct_timing    a timing flag. If set to true - when getting next frame,
 *  the data will be given after a period of time according to the frame repetition time,
 *  thus simulating a real device timing. Otherwise it will return the frame immediately.
 * 
 * @return Handle to the newly created dummy instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Avian_Device_t* ifx_avian_create_dummy_from_recording(ifx_Recording_t* recording, bool correct_timing);

/**
 * @brief Creates a device handle.
 *
 * This function searches for an Avian radar sensor connected to the host machine and
 * connects to the first sensor found.
 *
 * See also \ref ifx_avian_create_by_uuid for a list of potential errors.
 *
 * @return Handle to the newly created instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Avian_Device_t* ifx_avian_create(void);

/**
 * @brief Opens an Avian sensor.
 *
 * This function opens the Avian sensor connected to the port given by port.
 * If given port is nullptr, \ref ifx_avian_create is called to open an Avian sensor.
 * See also \ref ifx_avian_create_by_uuid for a list of potential errors.
 *
 * @param [in]  port    name of port to open
 * @return Handle to the newly created instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Avian_Device_t* ifx_avian_create_by_port(const char* port);

/**
 * @brief Creates a device handle.
 *
 * This function opens the Avian radar sensor connected to the host machine
 * with the unique id given by uuid.
 *
 * For converting the UUID from byte array to/from string see
 * \see ifx_uuid_from_string and \see ifx_uuid_to_string.
 *
 * On failure one of the following error codes are set:
 * - \ref IFX_ERROR_ARGUMENT_INVALID : The format of the uuid is wrong.
 * - \ref IFX_ERROR_NO_DEVICE : Sensor with given uuid not found.
 * - \ref IFX_ERROR_MEMORY_ALLOCATION_FAILED : Memory allocation failed.
 * - \ref IFX_ERROR_NOT_SUPPORTED : Sensor is not an Avian sensor.
 * - \ref IFX_ERROR_FIRMWARE_VERSION_NOT_SUPPORTED : Firmware version not supported anymore.
 * - \ref IFX_ERROR_COMMUNICATION_ERROR : A communication error occurred.
 * - \ref IFX_ERROR : An unknown error occurred.
 *
 * @param [in]     uuid       uuid as string
 *
 * @return  handle  Handle to the newly created instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Avian_Device_t* ifx_avian_create_by_uuid(const char* uuid);

/**
 * @brief returns exported register list as hexadecimal string format.
 *
 * The returned string is of the format 'char register_list[] = { 0x3140210, 0x9e967fd, 0xb0805b4, ... 0x11e8271 };'
 * There is memory allocated by the function for this string. The caller is responsible to deallocate it using \ref ifx_mem_free
 *
 * @param [in]  handle              A handle to the radar device object.
 * @param [in]  set_trigger_bit     boolean for wrapped libAvian function.
 * @return      pointer to allocated string containing register values in hexadecimal string format. this needs to be deallocated using \ref ifx_mem_free
 *
 */
IFX_DLL_PUBLIC
char* ifx_avian_get_register_list_string(ifx_Avian_Device_t* handle, bool set_trigger_bit);

 /**
 * @brief Configures radar sensor device and starts acquisition of time domain data.
 * The board is configured according to the parameters provided through *config*
 * and acquisition of time domain data is started.
 *
 * If the function fails ifx_error_get() function will return one of the following error codes:
 *         - \ref IFX_ERROR_NO_DEVICE
 *         - \ref IFX_ERROR_DEVICE_BUSY
 *         - \ref IFX_ERROR_COMMUNICATION_ERROR
 *         - \ref IFX_ERROR_NUM_SAMPLES_OUT_OF_RANGE
 *         - \ref IFX_ERROR_RX_ANTENNA_COMBINATION_NOT_ALLOWED
 *         - \ref IFX_ERROR_TX_ANTENNA_MODE_NOT_ALLOWED
 *         - \ref IFX_ERROR_IF_GAIN_OUT_OF_RANGE
 *         - \ref IFX_ERROR_SAMPLERATE_OUT_OF_RANGE
 *         - \ref IFX_ERROR_RF_OUT_OF_RANGE
 *         - \ref IFX_ERROR_TX_POWER_OUT_OF_RANGE
 *         - \ref IFX_ERROR_CHIRP_RATE_OUT_OF_RANGE
 *         - \ref IFX_ERROR_FRAME_RATE_OUT_OF_RANGE
 *         - \ref IFX_ERROR_NUM_CHIRPS_NOT_ALLOWED
 *         - \ref IFX_ERROR_FRAME_SIZE_NOT_SUPPORTED
 *
 * @param [in]     handle    A handle to the radar device object
 * @param [in]     config    This struct contains the parameters for data acquisition.
 *                           The device in configured according to these parameters.
 */
IFX_DLL_PUBLIC
void ifx_avian_set_config(ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config);

/**
* @brief Reads the current configurations of the radar sensor device.
* If this function succeeds and time domain data is available, IFX_OK is returned and the
* configuration is copied to *config*. If readout fails IFX_ERROR_COMMUNICATION_ERROR will be
* returned.
*
* @param [in]     handle     A handle to the radar device object.
* @param [out]    config     The current configuration of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_avian_get_config(ifx_Avian_Device_t* handle, ifx_Avian_Config_t* config);

/**
* @brief Get default configuration
*
* Returns a valid device configuration structure for the current Avian sensor.
*
* If the function succeeds, the device configuration is copied to the *config* output parameter,
* otherwise an error is set.
*
* \note The returned device configuration might differ for different releases.
*
* @param [in]     handle     A handle to the radar device object.
* @param [out]    config     The default configuration of the device.
*/
IFX_DLL_PUBLIC
void ifx_avian_get_config_defaults(ifx_Avian_Device_t* handle, ifx_Avian_Config_t* config);

/**
* @brief Returns the upper and lower limits configuration of the radar sensor device (according to the sensor type).
* If this function succeeds, the configuration is copied to the *config_lower* and *config_upper* output parameters otherwise the
* IFX_ERROR will be set.
* returned.
*
* @param [in]     handle           A handle to the radar device object.
* @param [out]    config_lower     The configuration of the lower limits for the device.
* @param [out]    config_upper     The configuration of the upper limits for the device.
*
*/
IFX_DLL_PUBLIC
void ifx_avian_config_get_default_limits(const ifx_Avian_Device_t* handle, ifx_Avian_Config_t* config_lower, ifx_Avian_Config_t* config_upper);

/**
 * @brief Get sensor type of connected device
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return                  Sensor type.
 *
 */
IFX_DLL_PUBLIC
ifx_Radar_Sensor_t ifx_avian_get_sensor_type(const ifx_Avian_Device_t* handle);

/**
* @brief Reads the temperature of the radar sensor device.
*
* On success, the current temperature of the radar sensor is written to the variable
* temperature_celsius. The temperature is in units of degrees Celsius.
* The 'temperature' field of the device handle is updated with the millidegree Celsius
* value.
*
* If an error occurs, temperature_celsius is not accessed and the error code can
* be retrieved using the function \ref ifx_error_get
*
* It is currently not possible to read the temperature from a BGT60UTR11AIP. For a
* BGT60UTR11AIP this function will return IFX_ERROR_NOT_SUPPORTED.
*
* @param [in]     handle                  A handle to the radar device object.
* @param [out]    temperature_celsius     The current configuration of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_avian_get_temperature(ifx_Avian_Device_t* handle, ifx_Float_t* temperature_celsius);

/**
 * @brief Configures radar sensor device ADC settings.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @param [in]     config    This struct contains the ADC parameters to apply.
 *                           The ADC in configured according to these parameters.
 *
 */
IFX_DLL_PUBLIC
void ifx_avian_configure_adc(ifx_Avian_Device_t* handle,
                              const ifx_Avian_ADC_Config_t* config);

/**
 * @brief Closes the connection to the radar sensor device.
 *
 * This function stops acquisition of time domain data, closes the connection to the device and
 * destroys the handle. The handle becomes invalid and must not be used any more after this
 * function was called.
 *
 * Please note that this function is not thread-safe and must not accessed
 * at the same time by multiple threads.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 */
IFX_DLL_PUBLIC
void ifx_avian_destroy(ifx_Avian_Device_t* handle);

/**
 * @brief Starts the acquisition of time domain data.
 *
 * This function starts the acquisition of time domain data. If data acquisition
 * is already running, the function has no effect.
 *
 * The time-domain signal can be retrieved by calling the function \ref ifx_avian_get_next_frame.
 *
 * This function is thread-safe.
 *
 * @param [in]     handle    A handle to the radar device object.
 */
IFX_DLL_PUBLIC
void ifx_avian_start_acquisition(ifx_Avian_Device_t* handle);

/**
 * @brief Stops the acquisition of time domain data.
 *
 * This function stops the acquisition of time domain data. If data acquisition
 * in not running, the function has no effect.
 *
 * This function is thread-safe.
 *
 * @param [in]     handle    A handle to the radar device object.
 */
IFX_DLL_PUBLIC
void ifx_avian_stop_acquisition(ifx_Avian_Device_t* handle);

/**
 * @brief Retrieves the next frame of time domain data from a radar device.
 *
 * This function retrieves the next complete frame of time domain data from the
 * connected device. The samples from all chirps and all enabled RX antennas
 * (virtual RX antennas in case of MIMO) will be copied to the provided *frame*
 * data structure of type \ref ifx_Cube_R_t (where rows = number of virtual antennas,
 * columns = chirps per frame, and slices = samples per chirp).
 * If the provided structure is NULL then a structure is created by the function.
 *
 * When time domain data is acquired by a radar sensor device, it is copied into an instance of
 * \ref ifx_Cube_R_t. Each row of the \ref ifx_Cube_R_t contains data corresponding to each
 * virtual active Rx antenna.
 * Columns correspond to chirps, slices correspond to samples.
 *
 * For a description of the frame structure see \ref sct_radarsdk_device_config_guide_measurement_frame and
 * For a description of virtual Rx antenna ordering in the *MIMO* use case see
 * \ref sct_radarsdk_device_config_guide_virtual_antenna_order.
 *
 * Here is a typical usage of this function:
 * @code
 *      ifx_Cube_R_t* frame = NULL;
 *      while(1)
 *      {
 *          frame = ifx_avian_get_next_frame(device_handle, frame);
 *          ifx_Error_t ret = ifx_error_get_and_clear();
 *          if(ret == IFX_ERROR_TIMEOUT)
 *              continue; // no data available, do something else
 *          else if(ret != IFX_OK)
 *              // error handling
 *              break;
 *
 *          // process data
 *          // ...
 *      }
 *      ifx_cube_destroy_r(frame);
 * @endcode *
 *
 * The function blocks until a full frame has been received or an error
 * occurred. Possible errors are:
 *  - \ref IFX_ERROR_COMMUNICATION_ERROR - communication error between board and
 *    host. This error typically occurs if the board was unplugged while fetching
 *    data.
 *  - \ref IFX_ERROR_FIFO_OVERFLOW - the radar board could not fetch the data
 *    fast enough from the radar sensor. The FIFO of the radar sensor overflowed
 *    and the state machine of the radar sensor was stopped. This typically
 *    occurs with radar configurations that cause high data rates, e.g. high
 *    frame rate, many activated receiving antennas, many samples per frame...
 *  - \ref IFX_ERROR_TIMEOUT - no full frame was available within 10 seconds.
 *  - \ref IFX_ERROR - an unknown error occurred.
 * The error status can be retrieved by calling ifx_error_get() after this function call.
 *
 * This function is equivalent to a call to \ref
 * ifx_avian_get_next_frame_timeout with a timeout of 10 seconds. For high
 * values of the frame repetition time (10 seconds or larger, corresponding to
 * a frame rate of 0.1Hz or lower) the function might return \ref
 * IFX_ERROR_TIMEOUT. In that case, please use \ref
 * ifx_avian_get_next_frame_timeout instead.
 *
 * This function automatically starts the time domain data acquisition if not
 * previously specified. It is not required to call \ref
 * ifx_avian_start_acquisition manually.
 *
 * The radar sensor sends the time-domain data in slices and not in full frames.
 * This means that after fetching a frame it does not necessarily take
 * frame_repetition_time_s seconds until this function returns the next frame.
 * For example, if both the frame and the frame repetition time are small (not
 * too many samples in a frame, high frame rate), one slice may contain several
 * frames. In this case two calls to \ref ifx_avian_get_next_frame will return
 * a frame immediately.
 *
 * See also \ref ifx_avian_start_acquisition and \ref
 * ifx_avian_get_next_frame_timeout.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @param [out]    frame     The frame (\ref ifx_Cube_R_t) structure where the time domain data should be copied to.
 *                           If set to NULL, then a new structure is created which the caller is responsible to free.
 *                           The frame is a cube of dimensions:
 *                           num_rx_antennas (rows) x num_chirps_per_frame (cols) x num_samples_per_chirp (slices)
 *
 * @return \ref ifx_Cube_R_t pointer to structure where the time domain data is populated.
 *                           This would be the same as the *frame* parameter if it is present.
 *                           If the *frame* parameter is NULL it would point to a newly allocated memory containing
 *                           a frame (\ref ifx_Cube_R_t) structure which the caller is
 *                           responsible to free.
 */
IFX_DLL_PUBLIC
ifx_Cube_R_t* ifx_avian_get_next_frame(ifx_Avian_Device_t* handle,
    ifx_Cube_R_t* frame);

/**
 * @brief Retrieves the next frame of time domain data from a radar device (non-blocking).
 *
 * This function retrieves the next frame of time domain data from the
 * connected device and stores it in a
 * data structure of type \ref ifx_Cube_R_t (where rows = number of virtual antennas,
 * columns = chirps per frame, and slices = samples per chirp).
 * If the provided structure is NULL then a structure is created by the function.
 *
 * For a description of the frame structure see \ref sct_radarsdk_device_config_guide_measurement_frame and
 * For a description of virtual Rx antenna ordering in the *MIMO* use case see
 * \ref sct_radarsdk_device_config_guide_virtual_antenna_order.
 *
 * The function will either populate the current frame into the structure
 * as soon as a complete frame is available or return if an \ref
 * IFX_ERROR_TIMEOUT occurs if no frame was available within *timeout_ms* of
 * milliseconds. The possible errors are
 * \ref IFX_OK                           if the function succeeded.
 * \ref IFX_ERROR_COMMUNICATION_ERROR    if a communication error occurred.
 * \ref IFX_ERROR_TIMEOUT                if a timeout occurred.
 * \ref IFX_ERROR_FIFO_OVERFLOW          if a FIFO overflow occurred.
 * \ref IFX_ERROR                        if an unknown error occurred.
 * See \ref ifx_avian_get_next_frame for more information.
 *
 * Here is a typical usage of this function:
 * @code
 *      const uint16_t timeout_100ms = 100;
 *      ifx_Cube_R_t* frame = NULL;
 *      while(1)
 *      {
 *          frame = ifx_avian_get_next_frame_timeout(device_handle, frame, timeout_100ms);
 *          ifx_Error_t ret = ifx_error_get_and_clear();
 *          if(ret == IFX_ERROR_TIMEOUT)
 *              continue; // no data available, do something else
 *          else if(ret != IFX_OK)
 *              // error handling
 *              break;
 *
 *          // process data
 *          // ...
 *      }
 *      ifx_cube_destroy_r(frame);
 * @endcode
 *
 * See also \ref ifx_avian_get_next_frame for a blocking version of this
 * function.
 *
 * @param [in]      handle              A handle to the radar device object.
 * @param [out]     frame               The frame structure where the time domain data should be copied to.
 *                                      If NULL, then a new structure is created which the caller is
 *                                      responsible to free. The frame is a cube (\ref ifx_Cube_R_t) of dimensions:
 *                                      num_rx_antennas (rows) x num_chirps_per_frame (cols) x num_samples_per_chirp (slices)
 * @param [in]      timeout_ms          Time in milliseconds after which the function times out.
 *
 * @return \ref ifx_Cube_R_t pointer to structure where the time domain data is populated.
 *                           This can be the same as the input parameter or new structure which the caller is
 *                           responsible to free.
 */
IFX_DLL_PUBLIC
ifx_Cube_R_t* ifx_avian_get_next_frame_timeout(ifx_Avian_Device_t* handle, ifx_Cube_R_t* frame, uint16_t timeout_ms);

/**
 * @brief Retrieves the number of RX antennas available on the connected radar device.
 *
 * This function returns the number of receiving antennas that are available on the
 * radar device specified by the given device handle.
 *
 * @note The function has been deprecated and will be removed in Radar SDK 3.4.
 *       Please use \ref ifx_avian_get_sensor_information to get the number of RX
 *       antennas instead.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 * @return Number of available receiving antennas. If the input parameter is null
 *         the returned value would be 0
 */
IFX_DLL_PUBLIC
uint8_t ifx_avian_get_num_rx_antennas(ifx_Avian_Device_t* handle);

IFX_FUNCTION_DEPRECATED("ifx_avian_get_num_rx_antennas will be removed in Radar SDK 3.4, please use ifx_avian_get_sensor_information instead.", IFX_DLL_PUBLIC uint8_t ifx_avian_get_num_rx_antennas(ifx_Avian_Device_t* handle));

/**
 * @brief Retrieves the number of TX antennas available on the connected radar device.
 *
 * This function returns the number of transmitting antennas that are available on the
 * radar device specified by the given device handle.
 *
 *  @note The function has been deprecated and will be removed in Radar SDK 3.4.
 *        Please use \ref ifx_avian_get_sensor_information to get the number of TX
 *        antennas instead.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 * @return Number of available transmitting antennas. If the input parameter is null
 *         the returned value would be 0
 */
IFX_DLL_PUBLIC
uint8_t ifx_avian_get_num_tx_antennas(ifx_Avian_Device_t* handle);

IFX_FUNCTION_DEPRECATED("ifx_avian_get_num_tx_antennas will be removed in Radar SDK 3.4, please use ifx_avian_get_sensor_information instead.", IFX_DLL_PUBLIC uint8_t ifx_avian_get_num_tx_antennas(ifx_Avian_Device_t* handle));

/**
 * @brief Retrieves the unique id of the connected board
 *
 * This function returns the unique id of the connected board.
 *
 * You can use \ref ifx_uuid_from_string to convert the string into an array
 * of 16 bytes.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @return         uuid      Unique id as a string.
 */
IFX_DLL_PUBLIC
const char* ifx_avian_get_board_uuid(const ifx_Avian_Device_t* handle);

/**
 * @brief Get information about the sensor on the connected device.
 *
 * The returned pointer is only valid until the device is destroyed.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return      pointer to \ref ifx_Radar_Sensor_Info_t structure
 *
 */
IFX_DLL_PUBLIC
const ifx_Radar_Sensor_Info_t* ifx_avian_get_sensor_information(const ifx_Avian_Device_t* handle);

/**
 * @brief Get information about the firmware version.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return      pointer to \ref ifx_Firmware_Info_t structure
 *
 */
IFX_DLL_PUBLIC
const ifx_Firmware_Info_t* ifx_avian_get_firmware_information(const ifx_Avian_Device_t* handle);

/**
 * @brief Get information about the RF shield.
 *
 * The shield information is read from the EEPROM on the RF shield. If the RF shield
 * does not contain an EEPROM, the EEPROM is broken or not correctly initialized,
 * the error \ref IFX_ERROR_EEPROM is set.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @param [in]  shield_info pointer to \ref ifx_RF_Shield_Info_t structure
 *
 */
IFX_DLL_PUBLIC
void ifx_avian_get_shield_information(const ifx_Avian_Device_t* handle, ifx_RF_Shield_Info_t* shield_info);

/**
 * @brief Computes the chirp time.
 *
 * The chirp time is the total chirp time consisting of the sampling time and
 * the sampling delay.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @param [in]  config      The current configuration of the device.
 * @return      chirp time
 *
 */
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_get_chirp_time(const ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config);

/**
 * @brief Retrieves the current RF transmission power from the sensor device.
 *
 * The power is returned for the specified antenna in units of dBm.
 *
 * tx_antenna must be smaller than the number of total TX antennas, see also
 * \ref ifx_avian_get_num_tx_antennas.
 *
 * On errors NAN is returned and the error code can be retrieved using \ref ifx_error_get.
 *
 * @param [in]     handle       A handle to the radar device object.
 * @param [in]     tx_antenna   TX antenna.
 * @return TX power for specified antenna in units of dBm.
 */
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_get_tx_power(ifx_Avian_Device_t* handle, uint8_t tx_antenna);

/**
 * @brief Retrieves the information about High Pass Cutoff Frequency available values for a current sensor device.
 *
 * Each list element is pointing to uint32_t type values and represents one possible HP Cutoff frequency in [Hz].
 *
 * User takes ownership over the list. The list must be freed after use using \ref ifx_list_destroy.
 *
 * Here is an example on how to use the function:
 * @code
 *      ifx_List_t* available_hp_cutoff_list = ifx_avian_get_hp_cutoff_list(avian_handle);
 *      for (size_t i = 0; i < ifx_list_size(available_hp_cutoff_list); i++)
 *      {
 *          uint32_t possible_frequency = *(uint32_t*)ifx_list_get(available_hp_cutoff_list, i);
 *          // ...
 *      }
 *      ifx_list_destroy(available_hp_cutoff_list);
 * @endcode
 *
 * @param [in]     handle       A handle to the radar device object.
 * @return  The pointer to \ref ifx_List_t of available HP Cutoff frequencies (pointed values are in [Hz]).
 */
IFX_DLL_PUBLIC
ifx_List_t* ifx_avian_get_hp_cutoff_list(const ifx_Avian_Device_t* handle);

/**
 * @brief Retrieves the information about AAF Cutoff Frequency available values for a current sensor device.
 *
 * Each list element is pointing to uint32_t type values and represents one possible AAF Cutoff frequency in [Hz].
 *
 * User takes ownership over the list. The list must be freed after use using \ref ifx_list_destroy.
 *
 * Here is an example on how to use the function:
 * @code
 *      ifx_List_t* available_aaf_cutoff_list = ifx_avian_get_aaf_cutoff_list(avian_handle);
 *      for (size_t i = 0; i < ifx_list_size(available_aaf_cutoff_list); i++)
 *      {
 *          uint32_t possible_frequency = *(uint32_t*)ifx_list_get(available_aaf_cutoff_list, i);
 *          // ...
 *      }
 *      ifx_list_destroy(available_aaf_cutoff_list);
 * @endcode
 *
 * @param [in]     handle       A handle to the radar device object.
 * @return  The list of available AAF Cutoff frequencies (pointed values are in [Hz]).
 */
IFX_DLL_PUBLIC
ifx_List_t* ifx_avian_get_aaf_cutoff_list(const ifx_Avian_Device_t* handle);


/**
 * @brief Computes the minimum chirp repetition time with interchirp power mode as active
 *
 * The minimum chirp repetition time consists of the sampling time, pre and post chirp delays and minimum shape end delay (25ns).
 * Power mode is also set to Active.
 *
 * @param [in]  handle      A handle to the radar device object
 * @param [in]  config      The current configuration of the device.
 * @return      minimum chirp repetition time (in s)
 *
 */
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_config_get_min_chirp_repetition_time(const ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config);

/**
 * @brief Computes the minimum frame repetition time
 *
 * The minimum frame repetition time consists of the current chirp repetition time, number of chirps per frame and minimum frame end delay of 15ns.
 * In case of MIMO, calculation uses twice the chirp repetition time as two shapes are configure in MIMO.
 *
 * @param [in]  handle      A handle to the radar device object
 * @param [in]  config      The current configuration of the device.
 * @return      minimum frame repetition time (in s)
 *
 */
IFX_DLL_PUBLIC
ifx_Float_t ifx_avian_config_get_min_frame_repetition_time(const ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config);

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_DEVICE_CONTROL_H */
