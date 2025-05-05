/* ===========================================================================
** Copyright (C) 2021-2022 Infineon Technologies AG
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

#include <array>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "ifxAvian/Avian.h"
#include "ifxRadar/Radar.h"

#include "json.hpp"
#include "json.h"

using std::string;
using std::vector;
using std::to_string;

// use ordered_json to preserve the insertion order.
using json = nlohmann::ordered_json;

#define IFX_AVIAN_DEFAULT_SAMPLE_RATE_HZ ((ifx_Float_t)1000000)



/**
 * @brief Convert antenna mask to a C++ vector of uint32
 *
 * @param [in] mask     antenna mask
 * @retval antenna vector
 */
static vector<uint32_t> mask_to_antenna_vector(uint32_t mask)
{
    vector<uint32_t> v;
    for (uint32_t i = 0; i < 32; i++)
    {
        if (mask & (1 << i))
            v.push_back(i+1);
    }
    return v;
}


/**
 * @brief Read sample rate
 *
 * If sample rate is not specified the function returns the default value
 * IFX_AVIAN_DEFAULT_SAMPLE_RATE_HZ.
 *
 * @retval sample_rate_Hz
 */
uint32_t ifxJsonConfiguration::get_sample_rate() const
{
    const uint32_t lower = IFX_AVIAN_SAMPLE_RATE_HZ_LOWER;
    const uint32_t upper = IFX_AVIAN_SAMPLE_RATE_HZ_UPPER;
    const double sample_rate = get_number("sample_rate_Hz", IFX_AVIAN_DEFAULT_SAMPLE_RATE_HZ);
    if (sample_rate < lower || sample_rate > upper)
        throw string("sample_rate_Hz must be between " + to_string(lower) + " and " + to_string(upper));

    return static_cast<uint32_t>(std::round(sample_rate));
}

/**
 * @brief Set active json object
 *
 * Set active json object to the path given by the vector path. This function
 * works similar to chdir for filesystems: All other get functions reference
 * relative from the active json object.
 *
 * The method throws an exception and sets m_error if the object does not
 * exist.
 *
 * @param [in] path     path to active json object
 */
void ifxJsonConfiguration::set_active_json_object(const vector<string>& path)
{
    const json* active = &m_json;

    if (!active)
        throw string("No json configuration loaded");

    string s_path;
    for (size_t i = 0; i < path.size() - 1; i++)
        s_path += path[i] + ".";
    s_path += path.back();

    if (path.empty())
    {
        m_path = "";
        return;
    }

    for (const string& dir : path)
    {
        if (!active->contains(dir))
            throw string("Object " + s_path + " missing");
        active = &(active->at(dir));
    }

    m_path = s_path;
    m_active = active;
}

/**
 * @brief Get JSON object
 *
 * Relative to the active JSON object, get the object with the name given by
 * name. If the object does not exist an exception is thrown.
 *
 * @param [in] name     name of object
 * @retval json object
 */
const json& ifxJsonConfiguration::get_json_object(const string& name) const
{
    if (!m_active->contains(name))
        throw string("Property " + m_path + "." + name + " missing");

    return m_active->at(name);
}

/**
 * @brief Get number
 *
 * Get the number (of type ifx_Float_t) corresponding to name. If the object
 * does not exist or it is not a number, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
ifx_Float_t ifxJsonConfiguration::get_number(const string& name) const
{
    auto property = get_json_object(name);
    if (!property.is_number())
        throw m_path + "." + name + " must be number";

    return property.get<ifx_Float_t>();
}

/**
 * @brief Get number or default value.
 *
 * Get the number (of type ifx_Float_t) corresponding to name. If the object
 * does not exist the value default_value is returned. If the value exists but
 * it is not a number, an exception is thrown.
 *
 * @param [in] name             name of property
 * @param [in] default_value    value returned if property is not present
 * @retval value
 */
ifx_Float_t ifxJsonConfiguration::get_number(const string& name, ifx_Float_t default_value) const
{
    if (!m_active->contains(name))
        return default_value;

    return get_number(name);
}

/**
 * @brief Get string
 *
 * Get the string corresponding to name. If the object does not exist or it is
 * not a string, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
string ifxJsonConfiguration::get_string(const string& name) const
{
    auto property = get_json_object(name);
    if (!property.is_string())
        throw m_path + "." + name + " must be string";

    return property.get<string>();
}

/**
 * @brief Get string or default string
 *
 * Get the string corresponding to name. If the object does not exist the string
 * default_string is returned. If the object exists but it is not a string, an
 * exception is thrown.
 *
 * @param [in] name     name of property
 * @param [in] default_value    value returned if property is not present
 * @retval value
 */
string ifxJsonConfiguration::get_string(const string& name, const string& default_value) const
{
    if (!m_active->contains(name))
        return default_value;

    return get_string(name);
}

/**
 * @brief Get high-medium-low value
 *
 * Parse property with allowed values "high", "medium", "low" and return it
 * as enum class LowMedHigh. If the property is not present or it is not
 * an allowed value, an exception is thrown.
 *
 * @param [in]  name    name of property
 * @retval element of LowMedHigh class
 */
LowMedHigh ifxJsonConfiguration::get_low_med_high(const string& name) const
{
    string s = get_string(name);
    if (s == "low")
        return LowMedHigh::low;
    else if (s == "medium")
        return LowMedHigh::medium;
    else if (s == "high")
        return LowMedHigh::high;

    throw string(name + " must be either \"low\" or \"medium\" or \"high\"");
}

/**
 * @brief Get positive number or default value.
 *
 * Get the positive number (of type ifx_Float_t) corresponding to name. If the object
 * does not exist the value default_value is returned. If the value exists but it is
 * not a positive number, an exception is thrown.
 *
 * @param [in] name             name of property
 * @param [in] default_value    value returned if property is not present
 * @retval value
 */
ifx_Float_t ifxJsonConfiguration::get_positive_number(const string& name, ifx_Float_t default_value) const
{
    if (!m_active->contains(name))
        return default_value;

    return get_positive_number(name);
}

/**
 * @brief Get positive number
 *
 * Get the number (of type ifx_Float_t) corresponding to name. If the object
 * does not exist or it is not a positive number, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
ifx_Float_t ifxJsonConfiguration::get_positive_number(const string& name) const
{
    const ifx_Float_t value = get_number(name);
    if (value <= 0)
        throw m_path + "." + name + " must be positive number";

    return value;
}

/**
 * @brief Get number and check bounds
 *
 * Get the number (ifx_Float_t) corresponding to name. If the object does not
 * exist or it is not a number, an exception is thrown. If the value is
 * outside [min, max] an exception is thrown as well.
 *
 * @param [in] name     name of property
 * @retval value
 */
ifx_Float_t ifxJsonConfiguration::get_number_bounds(const string& name, ifx_Float_t min, ifx_Float_t max) const
{
    const ifx_Float_t value = get_number(name);
    if (value < min || value > max)
        throw m_path + "." + name + " must be between " + to_string(min) + " and " + to_string(max);

    return value;
}

/**
 * @brief Get boolean
 *
 * Get the boolean corresponding to name. If the object does not exist or it is
 * not a boolean, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
bool ifxJsonConfiguration::get_bool(const string& name) const
{
    auto property = get_json_object(name);
    if (!property.is_boolean())
        throw m_path + "." + name + " must be bool";

    return property.get<bool>();
}

/**
 * @brief Get boolean or default value.
 *
 * Get the boolean corresponding to name. If the object does not exist the
 * value default_value is returned. If the value exists but it is not a
 * boolean, an exception is thrown.
 *
 * @param [in] name             name of property
 * @param [in] default_value    value returned if property is not present
 * @retval value
 */
bool ifxJsonConfiguration::get_bool(const string& name, bool default_value) const
{
    if (!m_active->contains(name))
        return default_value;

    return get_bool(name);
}

/**
 * @brief Get uint32
 *
 * Get the uint32 corresponding to name. If the object does not exist or it is
 * not a uint32, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
uint32_t ifxJsonConfiguration::get_uint32(const string& name) const
{
    const uint64_t value_u64 = get_uint64(name);
    if (value_u64 > UINT32_MAX)
        throw m_path + "." + name + " too big to store as 32bit integer";

    return static_cast<uint32_t>(value_u64);
}

/**
 * @brief Get uint32 or default value.
 *
 * Get the uint32 corresponding to name. If the object does not exist the
 * value default_value is returned. If the value exists but it is not a
 * uint32, an exception is thrown.
 *
 * @param [in] name             name of property
 * @param [in] default_value    value returned if property is not present
 * @retval value
 */
uint32_t ifxJsonConfiguration::get_uint32(const string& name, uint32_t default_value) const
{
    const uint64_t value_u64 = get_uint64(name, default_value);
    if (value_u64 > UINT32_MAX)
        throw m_path + "." + name + " too big to store as 32bit integer";

    return static_cast<uint32_t>(value_u64);
}

/**
 * @brief Get uint64
 *
 * Get the uint64 corresponding to name. If the object does not exist or it is
 * not a uint64, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
uint64_t ifxJsonConfiguration::get_uint64(const string& name) const
{
    auto property = get_json_object(name);
    if (!property.is_number_unsigned())
        throw m_path + "." + name + " must be unsigned integer";

    return property.get<uint64_t>();
}

/**
 * @brief Get uint64 or default value.
 *
 * Get the uint64 corresponding to name. If the object does not exist the
 * value default_value is returned. If the value exists but it is not a
 * uint64, an exception is thrown.
 *
 * @param [in] name             name of property
 * @param [in] default_value    value returned if property is not present
 * @retval value
 */
uint64_t ifxJsonConfiguration::get_uint64(const string& name, uint64_t default_value) const
{
    if (!m_active->contains(name))
        return default_value;

    return get_uint64(name);
}

/**
 * @brief Get uint64 and check bounds
 *
 * Get the uint64 corresponding to name. If the object does not exist or it is
 * not a uint64, an exception is thrown. If the value is outside [min, max] an
 * exception is thrown as well.
 *
 * @param [in] name     name of property
 * @param [in] min      minimum allowed value
 * @param [in] max      maximum allowed value
 * @retval value
 */
uint64_t ifxJsonConfiguration::get_uint64_bounds(const string& name, uint64_t min, uint64_t max) const
{
    const uint64_t value = get_uint64(name);
    if (value < min || value > max)
        throw m_path + "." + name + " must be between " + to_string(min) + " and " + to_string(max);

    return value;
}

/**
 * @brief Get uint32 and check bounds
 *
 * Get the uint32 corresponding to name. If the object does not exist or it is
 * not a uint32, an exception is thrown. If the value is outside [min, max] an
 * exception is thrown as well.
 *
 * @param [in] name     name of property
 * @param [in] min      minimum allowed value
 * @param [in] max      maximum allowed value
 * @retval value
 */
uint32_t ifxJsonConfiguration::get_uint32_bounds(const string& name, uint32_t min, uint32_t max) const
{
    return static_cast<uint32_t>(get_uint64_bounds(name, min, max));
}

/**
 * @brief Get antenna mask
 *
 * Get the antenna mask corresponding to name. If the object does not exist or it is
 * not a valid antenna mask, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
uint32_t ifxJsonConfiguration::get_antenna_mask(const string& name) const
{
    auto property = get_json_object(name);
    if (!property.is_array())
        throw m_path + "." + name + " must be array";

    uint32_t bitmask = 0;
    for (const auto& it : property)
    {
        if (!it.is_number_unsigned())
            throw m_path + "." + name + " must contain only positive integers";

        auto value = it.get<uint64_t>();
        if (value < 1 || value > 32)
            throw m_path + "." + name + " must contain only integers in the range [1-32]";

        bitmask |= 1 << (value-1);
    }

    return bitmask;
}

/**
 * @brief Get float vector
 *
 * Get the float vector corresponding to name. If the object does not exist or
 * it is not a valid float vector, an exception is thrown.
 *
 * @param [in] name     name of property
 * @retval value
 */
ifx_Vector_R_t* ifxJsonConfiguration::get_vector_r(const string& name) const
{
    auto property = get_json_object(name);
    if (!property.is_array())
        throw m_path + "." + name + " must be array";

    vector<double> v;
    for (const auto& it : property)
    {
        double value;
        if (!it.is_number() || (value = it.get<double>()) <= 0)
            throw m_path + "." + name + " must contain only positive numbers";

        v.push_back(value);
    }

    if(v.empty())
        throw m_path + "." + name + " must contain at least one positive number";
    if(v.size() > 32)
        throw m_path + "." + name + " has too many entries";

    ifx_Vector_R_t* vec = ifx_vec_create_r(static_cast<uint32_t>(v.size()));
    for (size_t i = 0; i < v.size(); i++)
        IFX_VEC_AT(vec, i) = static_cast<ifx_Float_t>(v[i]);

    return vec;
}

/**
 * @brief Get MIMO mode
 *
 * Get the MIMO mode. If the property does not exist IFX_MIMO_OFF is
 * returned. If the property exists but is not an allowed value, an
 * exception is thrown.
 *
 * @retval mimo mode
 */
enum ifx_Avian_MIMO_Mode ifxJsonConfiguration::get_mimo_mode() const
{
    const string name = "mimo_mode";
    if (!m_active->contains(name))
        return IFX_MIMO_OFF; // off

    const string value = get_string(name);
    if (value == "off")
        return IFX_MIMO_OFF;
    else if (value == "tdm")
        return IFX_MIMO_TDM;
    else
        throw string("mimo_mode must be \"off\" or \"tdm\"");
}

/**
 * @brief Constructor
 */
ifxJsonConfiguration::ifxJsonConfiguration()
{
}

/**
 * @brief Load configuration from file
 *
 * Load json configuration from file. If the json file cannot be opened or is
 * not a valid json file, an exception is thrown.
 *
 * @param [in]  filename    filename of configuration
 * @param [in]  type        type of radar sensor (currently ignored)
 * @param [in]  uuid        uuid of radar sensor (currently ignored)
 * @param true  if successful
 * @param false on errors
 */
void ifxJsonConfiguration::load_from_file(const string& filename, int type, const char* uuid)
{
    std::ifstream file;

    file.open(filename);
    if (!file.is_open())
        throw string("Cannot open file for reading");

    try
    {
        file >> m_json;
    }
    catch (...)
    {
        throw string("Error parsing JSON file");
    }
}

/**
 * @brief Save configuration to file
 *
 * Save configuration to JSON file. If the file cannot be written, an exception
 * is thrown.
 *
 * @param [in]  filename    filename of configuration
 * @param true  if successful
 * @param false on errors
 */
void ifxJsonConfiguration::save_to_file(const string& filename)
{
    std::ofstream file;

    file.open(filename);
    if (!file.is_open())
        throw string("Cannot open file for writing");

    try
    {
        file << std::setw(4) << m_json;
    }
    catch (...)
    {
        throw string("Error writing to file");
    }
}

bool ifxJsonConfiguration::has_device() const
{
    return m_json.contains("device");
}

vector<std::string> ifxJsonConfiguration::get_device_uuids() const
{
    vector<std::string> list;
    if (!has_device() || !m_json["device"].contains("uuids"))
        return list;

    const auto j = m_json["device"]["uuids"];
    if (!j.is_array())
        throw string("device.uuids must be a list");

    for (const auto& s : j)
    {
        list.push_back(s);
    }

    return list;
}

/**
 * @brief Return true if fmcw_scene configuration is present
 */
bool ifxJsonConfiguration::has_config_fmcw_scene() const
{
    return (m_json.contains("device_config") && m_json["device_config"].contains("fmcw_scene"));
}

/**
 * @brief Set fmcw_scene configuration
 */
void ifxJsonConfiguration::set_config_fmcw_scene(const ifx_Avian_Metrics_t* config)
{
    m_json["device_config"] = { {
        "fmcw_scene", {
            { "rx_antennas", mask_to_antenna_vector(config->rx_mask) },
            { "tx_antennas", mask_to_antenna_vector(config->rx_mask) },
            { "tx_power_level", config->tx_power_level },
            { "if_gain_dB", config->if_gain_dB },

            { "range_resolution_m", config->range_resolution_m },
            { "max_range_m", config->max_range_m },
            { "max_speed_m_s", config->max_speed_m_s },
            { "speed_resolution_m_s", config->speed_resolution_m_s },

            { "frame_repetition_time_s", config->frame_repetition_time_s }
        }
    } };

    /* write out the optional parameters only if they are set */
    if (config->sample_rate_Hz)
        m_json["device_config"]["fmcw_scene"]["sample_rate_Hz"] = config->sample_rate_Hz;
    if(config->center_frequency_Hz)
        m_json["device_config"]["fmcw_scene"]["center_frequency_Hz"] = config->center_frequency_Hz;
}

/**
 * @brief Get fmcw_scene configuration
 *
 * Write the fmcw_scene configuration to config_scene. If the configuration is
 * incorrect, an exception is thrown.
 *
 * @param [out]     config_scene    scene configuration
 */
void ifxJsonConfiguration::get_config_fmcw_scene(ifx_Avian_Metrics_t* config_scene)
{
    ifx_Avian_Metrics_t config = {};

    set_active_json_object({ "device_config", "fmcw_scene" });

    // required parameters
    config.range_resolution_m = get_positive_number("range_resolution_m");
    config.max_range_m = get_positive_number("max_range_m");
    config.max_speed_m_s = get_positive_number("max_speed_m_s");
    config.speed_resolution_m_s = get_positive_number("speed_resolution_m_s");
    config.frame_repetition_time_s = get_positive_number("frame_repetition_time_s");
    config.rx_mask = get_antenna_mask("rx_antennas");
    config.tx_mask = get_antenna_mask("tx_antennas");
    config.tx_power_level = get_uint32_bounds("tx_power_level", TX_POWER_LEVEL_LOWER, TX_POWER_LEVEL_UPPER);
    config.if_gain_dB = get_uint32_bounds("if_gain_dB", IF_GAIN_DB_LOWER, IF_GAIN_DB_UPPER);

    // optional parameters
    config.sample_rate_Hz = get_sample_rate();
    config.center_frequency_Hz = get_positive_number("center_frequency_Hz", 0);

    *config_scene = config;
}

/**
 * @brief Return true if presence sensing configuration is present
 */
bool ifxJsonConfiguration::has_config_presence_sensing() const
{
    return m_json.contains("presence_sensing");
}

/**
 * @brief Get presence sensing configuration
 *
 * Write the presence sensing configuration to config_presence_sensing. If the
 * configuration is incorrect, an exception is thrown.
 *
 * @param [in]      device_config   device configuration
 * @param [out]     config_scene    scene configuration
 */
void ifxJsonConfiguration::get_config_presence_sensing(ifx_Avian_Config_t* device_config, ifx_PresenceSensing_Config_t* config_presence_sensing)
{
    ifx_PresenceSensing_Config_t config = {};

    // Use the default configuration for the algo parameters but use the
    // device_config given to this function.
    //
    // Get the default configuration for the presence sensing algorithm
    // We are only interested in the default algorithm parameters and not
    // in the default device configuration. That's why we ignore dummy and
    // overwrite config.device_config.
    ifx_Avian_Config_t dummy;
    ifx_presence_sensing_get_config_defaults(IFX_AVIAN_BGT60TR13C, &dummy, &config);

    set_active_json_object({ "presence_sensing" });

    // required parameters
    config.min_detection_range_m = get_positive_number("min_detection_range_m");
    config.max_detection_range_m = get_positive_number("max_detection_range_m");
    config.range_hysteresis_percentage = get_number_bounds("range_hysteresis_percentage", 0, 100);
    config.absence_confirm_count = get_uint32("absence_confirm_count");
    config.presence_confirm_count = get_uint32("presence_confirm_count");
    config.device_config = *device_config;

    *config_presence_sensing = config;
}

void ifxJsonConfiguration::set_config_presence_sensing(const ifx_PresenceSensing_Config_t* config)
{
    m_json["presence_sensing"] = { {
            { "min_detection_range_m", config->min_detection_range_m },
            { "max_detection_range_m", config->max_detection_range_m },
            { "range_hysteresis_percentage", config->range_hysteresis_percentage },
            { "absence_confirm_count", config->absence_confirm_count },
            { "presence_confirm_count", config->presence_confirm_count }
    } };
}

/**
 * @brief Return true if segmentation configuration is present
 */
bool ifxJsonConfiguration::has_config_segmentation() const
{
    return m_json.contains("segmentation");
}

/**
 * @brief Set segmentation configuration
 */
void ifxJsonConfiguration::set_config_segmentation(const ifx_Segmentation_Config_t* config)
{
    std::string orientation;
    switch (config->orientation)
    {
    case IFX_ORIENTATION_PORTRAIT:
        orientation = "portrait";
        break;

    case IFX_ORIENTATION_LANDSCAPE:
    default:
        orientation = "landscape";
        break;
    }

    m_json["segmentation"] = { {
            { "orientation", orientation }
    } };
}

/**
 * @brief Get segmentation configuration
 *
 * Write the segmentation configuration to config_segmentation. If the
 * configuration is incorrect, an exception is thrown.
 *
 * @param [in]      device_config   device configuration
 * @param [out]     config_scene    scene configuration
 */
void ifxJsonConfiguration::get_config_segmentation(const ifx_Avian_Config_t* device_config, ifx_Segmentation_Config_t* config_segmentation)
{
    ifx_Segmentation_Config_t config = {};

    set_active_json_object({ "segmentation" });

    config.num_samples_per_chirp = device_config->num_samples_per_chirp;
    config.num_chirps_per_frame = device_config->num_chirps_per_frame;
    config.bandwidth_Hz = ifx_devconf_get_bandwidth(device_config);
    config.center_frequency_Hz = ifx_devconf_get_center_frequency(device_config);

    std::string orientation = get_string("orientation", "landscape");
    if (orientation == "landscape")
        config.orientation = IFX_ORIENTATION_LANDSCAPE;
    else if (orientation == "portrait")
        config.orientation = IFX_ORIENTATION_PORTRAIT;
    else
        throw std::string("unknown value for orientation");

    *config_segmentation = config;
}

/**
 * @brief Return true if fmcw_single_shape configuration is present
 */
bool ifxJsonConfiguration::has_config_fmcw_single_shape() const
{
    return (m_json.contains("device_config") && m_json["device_config"].contains("fmcw_single_shape"));
}

/**
 * @brief Set fmcw single shape configuration
 */
void ifxJsonConfiguration::set_config_fmcw_single_shape(const ifx_Avian_Config_t* config_single_shape)
{
    m_json["device_config"] = { {
        "fmcw_single_shape", {
            { "sample_rate_Hz", config_single_shape->sample_rate_Hz },
            { "rx_antennas", mask_to_antenna_vector(config_single_shape->rx_mask) },
            { "tx_antennas", mask_to_antenna_vector(config_single_shape->rx_mask) },
            { "tx_power_level", config_single_shape->tx_power_level },
            { "if_gain_dB", config_single_shape->if_gain_dB },
            { "start_frequency_Hz", config_single_shape->start_frequency_Hz },
            { "end_frequency_Hz", config_single_shape->end_frequency_Hz },
            { "num_chirps_per_frame", config_single_shape->num_chirps_per_frame },
            { "num_samples_per_chirp", config_single_shape->num_samples_per_chirp },
            { "chirp_repetition_time_s", config_single_shape->chirp_repetition_time_s },
            { "frame_repetition_time_s", config_single_shape->frame_repetition_time_s },
            { "hp_cutoff_Hz", config_single_shape->hp_cutoff_Hz },
            { "aaf_cutoff_Hz", config_single_shape->aaf_cutoff_Hz },
            { "mimo_mode", (config_single_shape->mimo_mode == IFX_MIMO_TDM) ? "tdm" : "off" }
        }
    } };
}

/**
 * @brief Get fmcw_single_shape configuration
 *
 * Save the configuration to single_shape.
 *
 * @param [in]      single_shape    single shape configuration
 * @retval true     if successful
 * @retval false    if an error occurred
 */

/**
 * @brief Get fmcw_single_shape configuration
 *
 * Write the fmcw_single_shape configuration to single_shape. If the
 * configuration is incorrect, an exception is thrown.
 *
 * @param [in]      device_config   device configuration
 * @param [out]     config_scene    scene configuration
 */
void ifxJsonConfiguration::get_config_fmcw_single_shape(ifx_Avian_Config_t* single_shape)
{
    ifx_Avian_Config_t config = {};

    set_active_json_object({"device_config", "fmcw_single_shape"});

    // required parameters
    config.rx_mask = get_antenna_mask("rx_antennas");
    config.tx_mask = get_antenna_mask("tx_antennas");
    config.tx_power_level = get_uint32_bounds("tx_power_level", TX_POWER_LEVEL_LOWER, TX_POWER_LEVEL_UPPER);
    config.if_gain_dB = get_uint32_bounds("if_gain_dB", IF_GAIN_DB_LOWER, IF_GAIN_DB_UPPER);
    config.chirp_repetition_time_s = get_positive_number("chirp_repetition_time_s");
    config.frame_repetition_time_s = get_positive_number("frame_repetition_time_s");
    config.num_chirps_per_frame = get_uint32("num_chirps_per_frame");
    config.num_samples_per_chirp = get_uint32("num_samples_per_chirp");
    config.hp_cutoff_Hz = get_uint32("hp_cutoff_Hz", 80000);
    config.aaf_cutoff_Hz = get_uint32("aaf_cutoff_Hz", 500000);

    double start_frequency{0};
    double end_frequency{0};

    try {
        // Try if proper start and end frequency values are present in json
        start_frequency = get_number_bounds("start_frequency_Hz", 0, 80e9);
        end_frequency = get_number_bounds("end_frequency_Hz", 0, 80e9);
    }
    catch (string&) {
        try {
            // If no proper start and end frequency are present
            // try if proper lower and upper frequency values are in json (legacy)
            start_frequency = get_number_bounds("lower_frequency_Hz", 0, 80e9);
            end_frequency = get_number_bounds("upper_frequency_Hz", 0, 80e9);
        }
        catch (string& error) {
            throw error;
        }
    }

    config.start_frequency_Hz = static_cast<uint64_t>(std::round(start_frequency));
    config.end_frequency_Hz = static_cast<uint64_t>(std::round(end_frequency));

    // optional parameters
    config.sample_rate_Hz = get_sample_rate();
    config.mimo_mode = get_mimo_mode();

    // everything fine
    *single_shape = config;
}

/* ----------- *
 * C interface *
 * ----------- */

struct ifxJsonConfigurationC
{
    ifxJsonConfiguration obj;
    string error;
};

ifx_json_t* ifx_json_create(void)
{
    return new (std::nothrow) ifxJsonConfigurationC;
}

void ifx_json_destroy(ifx_json_t* j)
{
    delete j;
}

const char* ifx_json_get_error(const ifx_json_t* json)
{
    return json->error.c_str();
}

bool ifx_json_load_from_file(ifx_json_t* json, const char* filename)
{
    try
    {
        json->obj.load_from_file(filename);
        return true;
    }
    catch (string& error)
    {
        json->error = error;
        return false;
    }
}

bool ifx_json_save_to_file(ifx_json_t* json, const char* filename)
{
    try
    {
        json->obj.save_to_file(filename);
        return true;
    }
    catch (string& error)
    {
        json->error = error;
        return false;
    }
}

ifx_json_t* ifx_json_create_from_file(const char* filename)
{
    ifxJsonConfigurationC* json = ifx_json_create();
    if (json == nullptr)
        return nullptr;

    if (ifx_json_load_from_file(json, filename))
        return json;

    // loading was not successful
    ifx_json_destroy(json);
    return nullptr;
}

bool ifx_json_has_config_scene(const ifx_json_t* json)
{
    return json->obj.has_config_fmcw_scene();
}

void ifx_json_set_device_config_scene(ifx_json_t* json, const ifx_Avian_Metrics_t* metrics)
{
    return json->obj.set_config_fmcw_scene(metrics);
}

bool ifx_json_get_device_config_scene(ifx_json_t* json, ifx_Avian_Metrics_t* metrics)
{
    try
    {
        json->obj.get_config_fmcw_scene(metrics);
        return true;
    }
    catch (string& error)
    {
        json->error = error;
        return false;
    }
}

bool ifx_json_get_device_config_single_shape(ifx_json_t* json, ifx_Avian_Config_t* config)
{
    try
    {
        json->obj.get_config_fmcw_single_shape(config);
        return true;
    }
    catch (string& error)
    {
        json->error = error;
        return false;
    }
}

bool ifx_json_has_config_single_shape(const ifx_json_t* json)
{
    return json->obj.has_config_fmcw_single_shape();
}

void ifx_json_set_device_config_single_shape(ifx_json_t* json, const ifx_Avian_Config_t* config)
{
    json->obj.set_config_fmcw_single_shape(config);
}

void ifx_json_set_segmentation(ifx_json_t* json, const ifx_Segmentation_Config_t* segmentation_config)
{
    json->obj.set_config_segmentation(segmentation_config);
}

bool ifx_json_has_segmentation(const ifx_json_t* json)
{
    return json->obj.has_config_segmentation();
}

bool ifx_json_get_segmentation(ifx_json_t* json,
                               const ifx_Avian_Config_t* device_config,
                               ifx_Segmentation_Config_t* config_segmentation)
{
    try
    {
        json->obj.get_config_segmentation(device_config, config_segmentation);
        return true;
    }
    catch (string& error)
    {
        json->error = error;
        return false;
    }
}

void ifx_json_set_presence_sensing(ifx_json_t* json, const ifx_PresenceSensing_Config_t* config_presence_sensing)
{
    json->obj.set_config_presence_sensing(config_presence_sensing);
}

bool ifx_json_has_presence_sensing(const ifx_json_t* json)
{
    return json->obj.has_config_presence_sensing();
}

bool ifx_json_get_presence_sensing(ifx_json_t* json, ifx_Avian_Config_t* device_config, ifx_PresenceSensing_Config_t* config_presence_sensing)
{
    try
    {
        json->obj.get_config_presence_sensing(device_config, config_presence_sensing);
        return true;
    }
    catch (string& error)
    {
        json->error = error;
        return false;
    }
}
