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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "ifxBase/Exception.hpp"

#include "ifxAvian/internal/RadarDevice.hpp"
#include "ifxAvian/internal/RadarDeviceErrorTranslator.hpp"

#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"

#include <cstring>
#include "ifxBase/Uuid.h"
#include "ifxBase/internal/Util.h"
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
    constexpr std::array<uint16_t, 3> minVersion = { 2, 5, 0 };

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

static std::vector<uint16_t> packRaw12(const uint8_t* input, size_t num_samples)
{
    std::vector<uint16_t> output;
    output.reserve(num_samples);

    for (size_t i = 0; i < num_samples / 2; i++)
    {
        output.push_back((input[3 * i + 0] << 4) | (input[3 * i + 1] >> 4));
        output.push_back(((input[3 * i + 1] & 0x0f) << 8) | input[3 * i + 2]);
    }

    return output;
}

//----------------------------------------------------------------------------

ifx_Radar_Device_s::ifx_Radar_Device_s(std::unique_ptr<BoardInstance> board)
    : m_board(std::move(board))
{
    if (!m_board)
        throw rdk::exception::no_device();

    // Check that we are really connected to a board with an Avian sensor
    if (m_board->getComponentImplementation<IRadar>(0) != IRadarAvian::getImplementation())
        throw rdk::exception::not_supported();
    
    /* check if firmware is valid */
    {
        rdk::RadarDeviceCommon::get_firmware_info(m_board.get(), &m_firmware_info);
        if (!rdk::RadarDeviceCommon::is_firmware_version_valid(minVersion, m_firmware_info))
            throw rdk::exception::firmware_version_not_supported();
    }

    try
    {
        m_avian_port = std::make_unique<Avian::StrataPort>(m_board.get());
    }
    catch (const EException&)
    {
        throw rdk::exception::no_device();
    }

    try
    {
        m_driver = Avian::Driver::create_driver(*m_avian_port);
    }
    catch (const EException&)
    {
        throw rdk::exception::communication_error();
    }

    if (!m_driver)
    {
        throw rdk::exception::device_not_supported();
    }

    {
        // get UUID
        const auto uuid = m_board->getUuid();
       
        // convert to string
        char str[64] = "";
        ifx_uuid_to_string(uuid.data(), str);
        m_board_uuid.assign(str);
    }

    initialize();

    //m_driver->stop_and_reset_sequence();
    stop_and_reset();
}

//----------------------------------------------------------------------------

ifx_Radar_Device_s::~ifx_Radar_Device_s()
{
    try
    {
        stop_acquisition();
    }
    catch (...)
    {
        // It might happen that stop_acquisition throws an exception in case
        // the device is no longer present.
        //
        // As a destructor must not throw exceptions, we ignore the exception here.
        //
        // Anyhow, if the device is longer present, it is also not necessary to
        // stop the acquisition.
    }
};

void ifx_Radar_Device_s::initialize()
{
    RadarDeviceBase::initialize();

    m_driver->get_device_id(&m_sensor_info.device_id);
}

void ifx_Radar_Device_s::start_acquisition()
{
    const auto aquisition_stopped_previously = AcquisitionStateLogic::update(m_acquisition_state, Acquisition_State_t::Stopped, Acquisition_State_t::Starting);

    if(! aquisition_stopped_previously)
    {
        return;
    }

    try
    {
        // The driver knows the current slice size
        uint16_t slice_size;
        m_driver->get_slice_size(&slice_size);

        // clear the buffer
        m_fifo.clear();

        auto data_ready_callback = [this, slice_size](Avian::HW::Spi_Response_t /*status_word*/) {
            if (m_acquisition_state != Acquisition_State_t::Started)
                return;

            // handle packed raw data
            const uint8_t* data = reinterpret_cast<uint8_t*>(m_avian_buffer.data());
            std::vector<uint16_t> v = packRaw12(data, slice_size);
            this->m_fifo.push(v);
        };

        auto error_callback = [this](Avian::StrataPort* /*port_adapter*/, uint32_t error_code) {
            if (error_code == Avian::ERR_FIFO_OVERFLOW)
                m_acquisition_state = Acquisition_State_t::FifoOverflow;
            else
                m_acquisition_state = Acquisition_State_t::Error;
        };

        // The data buffer is partitioned according to the new block size.
        m_avian_buffer.resize(static_cast<size_t>(slice_size) * 2);

        auto* avian_port = get_strata_avian_port();

        // Finally buffering is enabled by passing the first data block to the Avian port.
        avian_port->set_buffer(m_avian_buffer.data());

        // Register error callback which is called on FIFO overflows or communication errors
        avian_port->register_error_callback(error_callback);

        // The reader is setup with the new slice size
        avian_port->start_reader(m_driver->get_burst_prefix(), slice_size, data_ready_callback);

        // Data reading is active now, but the Avian device must be triggered, too.
        m_driver->get_device_configuration().send_to_device(*avian_port, true);
        m_driver->notify_trigger();

        m_acquisition_state = Acquisition_State_t::Started;
    }
    catch (const EConnection&)
    {
        m_acquisition_state = Acquisition_State_t::Error;
        throw rdk::exception::communication_error();
    }
    catch (const EException&)
    {
        m_acquisition_state = Acquisition_State_t::Error;
        throw rdk::exception::error();
    }
}

//----------------------------------------------------------------------------

void ifx_Radar_Device_s::stop_acquisition()
{
    if(m_acquisition_state != Acquisition_State_t::Started)
        return;

    const auto aquisition_started_previously = AcquisitionStateLogic::update(m_acquisition_state, Acquisition_State_t::Started, Acquisition_State_t::Stopping);

    if(! aquisition_started_previously)
        return;

    try
    {
        auto* avian_port = get_strata_avian_port();
        avian_port->stop_reader();
        stop_and_reset();
    }
    catch (const EConnection&)
    {
        m_acquisition_state = Acquisition_State_t::Error;
        throw rdk::exception::communication_error();
    }
    catch (const EException&)
    {
        m_acquisition_state = Acquisition_State_t::Error;
        throw rdk::exception::error();
    }
}

//----------------------------------------------------------------------------

void ifx_Radar_Device_s::send_to_device()
{
    Avian::HW::RegisterSet register_set = m_driver->get_device_configuration();

    try
    {
        register_set.send_to_device(*m_avian_port, false);
    }
    catch (...)
    {
        throw rdk::exception::communication_error();
    }
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_Radar_Device_s::get_temperature()
{
    // Reading temperature from a BGT60UTR11AIP is not supported:
    // With BGT60UTR11AIP you can measure the temperature when the chip is off,
    // in CW mode, or when the chip is operating with a triangular shape.
    // During saw tooth shapes temperature measurement is not supported.
    // To avoid any problems we set the error IFX_ERROR_NOT_SUPPORTED
    // when reading the temperature with a BGT60UTR11AIP.
    if (m_driver->get_device_type() == Avian::Device_Type::BGT60UTR11)
    {
        throw rdk::exception::not_supported();
    }

    // Do not read the temperature from the radar sensor too often as it
    // decreases performance (negative impact on data rate), might cause
    // problems as fetching temperature takes some time, and it is not
    // required (the temperature hardly changes within 100ms).
    //
    // If the cached temperature value is not yet expired, we simply use the
    // cached value instead of reading a new value from the radar sensor.
    auto now = std::chrono::steady_clock::now();
    if (now < m_temperature_expiration_time)
    {
        // Returning a cached value is not an error, so don't throw an
        // exception here but merely return from this method.
        return m_temperature_value;;
    }

    // temperature in units of 0.001 degree Celsius
    int32_t temp = 0;
    Avian::Driver::Error rc = m_driver->get_temperature(&temp);
    if (rc == Avian::Driver::Error::OK)
    {
        // Save the current temperature value and get a new temperature value
        // from the radar sensor earliest in 100ms again.
        m_temperature_value = temp * ifx_Float_t(0.001);
        m_temperature_expiration_time = now + std::chrono::milliseconds(100);
        return m_temperature_value;
    }
    else{
        throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
    }
}

//----------------------------------------------------------------------------

ifx_Cube_R_t* ifx_Radar_Device_s::get_next_frame(ifx_Cube_R_t* frame, uint16_t timeout_ms)
{
    const uint32_t num_virtual_antennas = ifx_devconf_count_rx_antennas(&m_config);
    const uint32_t num_chirps_per_frame = m_config.num_chirps_per_frame;
    const uint32_t num_samples_per_chirp = m_config.num_samples_per_chirp;
    const size_t num_samples_per_frame = num_virtual_antennas * num_chirps_per_frame * num_samples_per_chirp;

    if(frame != nullptr)
    {
        // check if dimensions of cube frame are correct
        if (IFX_CUBE_ROWS(frame) != num_virtual_antennas ||
            IFX_CUBE_COLS(frame) != num_chirps_per_frame ||
            IFX_CUBE_SLICES(frame) != num_samples_per_chirp)
            throw rdk::exception::dimension_mismatch();
    }

    switch (m_acquisition_state)
    {
    // If an error occurred (typically a communication error), try to stop and
    // start data acquisition.
    case Acquisition_State_t::Error:
        throw rdk::exception::communication_error();

    // On FIFO overflow stop and start data acquisition.
    case Acquisition_State_t::FifoOverflow:
        stop_acquisition();
        start_acquisition();
        throw rdk::exception::fifo_overflow();

    case Acquisition_State_t::Stopped:
        start_acquisition();
        break;

    // avoid warning from gcc
    default:
        break;
    }

    std::vector<uint16_t> raw_data;

    /* Check every 100ms (timeout_pop_ms) if a FIFO overflow or another
     * error occurred. If an error occurred return an error code and
     * return from this function.
     */
    do
    {
        constexpr uint16_t timeout_pop_ms = 100;
        const uint16_t cur_timeout = std::min(timeout_pop_ms, timeout_ms);
        bool success = m_fifo.pop(raw_data, num_samples_per_frame, cur_timeout);
        timeout_ms -= cur_timeout;

        switch (m_acquisition_state.load())
        {
        case Acquisition_State_t::FifoOverflow:
            throw rdk::exception::fifo_overflow();
        case Acquisition_State_t::Error:
            throw rdk::exception::communication_error();

        // avoid warning from gcc
        default:
            break;
        }

        if (success)
            break;
    } while (timeout_ms > 0);

    // if we still have no data return timeout
    if (raw_data.empty())
        throw rdk::exception::timeout();

    // number of physical RX antennas used
    const size_t num_rx = ifx_util_popcount(m_config.rx_mask);

    // number of physical TX antennas used (2 if MIMO, otherwise 1)
    const size_t num_tx = m_config.mimo_mode == IFX_MIMO_TDM ? 2 : 1;

    // maximum ADC value: 2**12-1
    constexpr ifx_Float_t adc_max = 4095;

    if (!frame)
    {
        // allocate memory for frame
        frame = ifx_cube_create_r(num_virtual_antennas, num_chirps_per_frame, num_samples_per_chirp);
        if (!frame)
            throw rdk::exception::memory_allocation_failed();
    }

    size_t index = 0;
    // Copy the data into the cube structure
    for (size_t chirp = 0; chirp < num_chirps_per_frame; chirp++) // slices
    {
        for (size_t tx = 0; tx < num_tx; tx++) // for column offset
        {
            const size_t column_offset = tx * num_rx;
            for (size_t sample = 0; sample < num_samples_per_chirp; sample++) // rows
            {
                for (size_t rx = column_offset; rx < (column_offset + num_rx); rx++) // columns
                {
                    IFX_CUBE_AT(frame, rx, chirp, sample) = raw_data.at(index++) / adc_max;
                }
            }
        }
    }

    return frame;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_Radar_Device_s::get_tx_power(uint8_t tx_antenna)
{
    if (m_config.mimo_mode == IFX_MIMO_TDM)
    {
        /* In case of MIMO the two antennas are not active at the same time. In
         * shape 0 TX1 is activated and in shape 1 TX2 is activated. In order to
         * avoid measuring the TX power of a disabled TX antenna, we first have
         * to select the correct shape.
         */
        uint8_t shape;
        bool down_chirp;
        m_driver->get_selected_shape(&shape, &down_chirp);
        if (tx_antenna == 0)
            m_driver->select_shape_to_configure(0, down_chirp);
        else
            m_driver->select_shape_to_configure(1, down_chirp);
    }

    int32_t tx_power_001dBm = 0;
    auto rc = m_driver->get_tx_power(tx_antenna, &tx_power_001dBm);

    if (rc != Avian::Driver::Error::OK)
    {
        throw rdk::exception::error::exception(RadarDeviceErrorTranslator::translate_error_code(rc));
    }

    return tx_power_001dBm * static_cast<ifx_Float_t>(0.001);
}
