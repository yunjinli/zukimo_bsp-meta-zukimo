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

#include "ifxAvian/internal/RadarDeviceErrorTranslator.hpp"

using namespace Infineon;


/*static*/
ifx_Error_t RadarDeviceErrorTranslator::translate_error_code(const Avian::Driver::Error error)
{
    using Avian::Driver;

    switch (error)
    {

    case Driver::Error::OK:
        // no error
        return IFX_OK;

    case Driver::Error::BUSY:
        return IFX_ERROR_DEVICE_BUSY;

    case Driver::Error::UNSUPPORTED_FRAME_FORMAT:
        /* ep_radar_base_set_frame_format could return this. If this happens the only reason could
         * be an invalid number of samples, because all other parameters are known not to be
         * critical.
         */
        return IFX_ERROR_NUM_SAMPLES_OUT_OF_RANGE;

    case Driver::Error::ANTENNA_DOES_NOT_EXIST:
        /* ep_radar_base_set_frame_format returns this code to indicate a bad RX mask. */
        return IFX_ERROR_RX_ANTENNA_COMBINATION_NOT_ALLOWED;

    case Driver::Error::UNSUPPORTED_TX_MODE:
        /* ep_radar_bgt6x_set_tx_mode returns this code if specified TX mode is not
         * supported by the device.*/
        return IFX_ERROR_TX_ANTENNA_MODE_NOT_ALLOWED;

    case Driver::Error::SAMPLERATE_OUT_OF_RANGE:
        /* ep_radar_bgt6x_set_adc_samplerate can return this code. */
        return IFX_ERROR_SAMPLERATE_OUT_OF_RANGE;

    case Driver::Error::FREQUENCY_OUT_OF_RANGE:
        /* ep_radar_fmcw_set_fmcw_configuration can return this code. */
        return IFX_ERROR_RF_OUT_OF_RANGE;

    case Driver::Error::POWER_OUT_OF_RANGE:
        /* ep_radar_fmcw_set_fmcw_configuration can return this code. */
        return IFX_ERROR_TX_POWER_OUT_OF_RANGE;

    case Driver::Error::UNSUPPORTED_NUM_REPETITIONS:
        /* The number of chirps per frame is set as number of set repetitions to
        * ep_radar_bgt60trxx_set_frame_definition.
        */
        return IFX_ERROR_NUM_CHIRPS_NOT_ALLOWED;

    case Driver::Error::UNSUPPORTED_FIFO_SLICE_SIZE:
        /* ep_radar_bgt60trxx_set_data_slice_size returns this if the slice size is not a multiple
         * of 4.
         */
        return IFX_ERROR_FRAME_SIZE_NOT_SUPPORTED;

    default:
        /* If none of the above error code is returned this can have different reasons:
         * 1) Endpoint 0 or comm lib itself returned some errors indicating invalid protocol
         *    messages.
         * 2) It's known from BGT60TRxx driver code, that the received error code cannot be sent by
         *    the function calls used above. If this happens this must be a "misunderstanding"
         *    between host and radar device, so it is also a communication error.
         * 3) It's known from BGT60TRxx driver code, that the received error code can be sent by
         *    any of the function calls used above, but the sent parameters are known to be good,
         *    so the error should not occur. Again this is considered to be a communication error.
         *    Those known error codes are:
         *    - from ep_radar_bgt6x_set_baseband_configuration
         *        EP_RADAR_ERR_UNSUPPORTED_HP_GAIN
         *        EP_RADAR_ERR_UNSUPPORTED_HP_CUTOFF
         *        EP_RADAR_ERR_UNSUPPORTED_VGA_GAIN
         *        EP_RADAR_ERR_RESET_TIMER_OUT_OF_RANGE
         *    - from ep_radar_bgt6x_set_adc_samplerate
         *        EP_RADAR_ERR_UNDEFINED_TRACKING_MODE
         *        EP_RADAR_ERR_UNDEFINED_ADC_SAMPLE_TIME
         *    - from ep_radar_fmcw_set_fmcw_configuration
         *        EP_RADAR_ERR_UNSUPPORTED_DIRECTION
         *    - from ep_radar_bgt60trxx_set_frame_definition
         *        EP_RADAR_ERR_UNSUPPORTED_POWER_MODE
         *        EP_RADAR_ERR_NONCONTINUOUS_SHAPE_SEQUENCE
         *        EP_RADAR_ERR_NUM_FRAMES_OUT_OF_RANGE
         *        EP_RADAR_ERR_POST_DELAY_OUT_OF_RANGE (post delay can go up to 7h!)
         *    - from ep_radar_bgt60trxx_trigger_sequence
         *        EP_RADAR_ERR_INCOMPATIBLE_MODE
         *        EP_RADAR_ERR_NO_MEMORY
         */
        return IFX_ERROR_COMMUNICATION_ERROR;
    }
}
