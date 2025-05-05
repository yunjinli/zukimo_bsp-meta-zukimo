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

#ifndef IFX_RADAR_DEVICE_COMMON_HPP
#define IFX_RADAR_DEVICE_COMMON_HPP

#include "ifxBase/Error.h"
#include "ifxBase/Types.h"
#include "ifxRadarDeviceCommon/RadarDeviceCommon.h"

#include "common/exception/EException.hpp"
#include "ifxBase/Exception.hpp"

#include <platform/BoardInstance.hpp>
#include <platform/exception/EConnection.hpp>

#include <functional>
#include <memory>
#include <vector>

namespace rdk
{
    namespace RadarDeviceCommon
    {
        /** Return true if sensor belongs to Avian family */
        inline bool sensor_is_avian(ifx_Radar_Sensor_t sensor_type)
        {
            return sensor_type < IFX_MIMOSE_BGT24ATR22;
        }

        /** Return true if sensor belongs to Mimose family */
        inline bool sensor_is_mimose(ifx_Radar_Sensor_t sensor_type)
        {
            return sensor_type == IFX_MIMOSE_BGT24ATR22;
        }

        /** Return true if sensor is BGT60LTR11 */
        inline bool sensor_is_ltr11(ifx_Radar_Sensor_t sensor_type)
        {
            return sensor_type == IFX_BGT60LTR11AIP;
        }

        /**
         * @brief Type of selector function
         *
         * If the selector function returns true the respective element will
         * be selected.
         */
        using SelectorFunction = std::function<bool(const ifx_Radar_Sensor_List_Entry_t&)>;

        /**
         * @brief Determine board type from product id
         *
         * @param [in]    pid            product id
         * @retval        boardtype    board type
         */
        inline ifx_Board_Type_t get_boardtype_from_pid(uint16_t pid)
        {
            constexpr uint16_t pid_RadarBaseboardMCU7 = 0x0251;  // PID of RadarBaseboardMCU7 (as returned by Strata)
            constexpr uint16_t pid_RadarBaseboardAurix = 0x0252; // PID of RadarBaseboardAurix (as returned by Strata)
            constexpr uint16_t pid_V9 = 0x0253; // PID of V9 board (as returned by Strata)

            switch (pid)
            {
            case pid_RadarBaseboardAurix:
                return IFX_BOARD_BASEBOARD_AURIX;
            case pid_RadarBaseboardMCU7:
                return IFX_BOARD_BASEBOARD_MCU7;
            case pid_V9:
                return IFX_BOARD_BASEBOARD_V9;
            default:
                return IFX_BOARD_UNKNOWN;
            }
        }

        inline const char* sensor_to_string(ifx_Radar_Sensor_t sensor_type)
        {
            switch (sensor_type)
            {
            case IFX_AVIAN_BGT60TR13C:
                return "BGT60TR13C";
            case IFX_AVIAN_BGT60ATR24C:
                return "BGT60ATR24C";
            case IFX_AVIAN_BGT60UTR13D:
                return "BGT60UTR13D";
            case IFX_AVIAN_BGT60TR12E:
                return "BGT60TR12E";
            case IFX_AVIAN_BGT60UTR11:
                return "BGT60UTR11";
            case IFX_AVIAN_BGT120UTR13E:
                return "BGT60UTR13E";
            case IFX_AVIAN_BGT24LTR24:
                return "BGT24LTR24";
            case IFX_AVIAN_BGT120UTR24:
                return "IFX_AVIAN_BGT120UTR24";
            case IFX_AVIAN_UNKNOWN:
                return "unknown Avian sensor";

            case IFX_MIMOSE_BGT24ATR22:
                return "BGT24ATR22";

            case IFX_BGT60LTR11AIP:
                return "BGT60LTR11";

            default:
                return "unknown";
            }
        }

        inline ifx_Radar_Sensor_t string_to_sensor(const std::string& sensor)
        {
            if (sensor == "BGT60TR13C")
                return IFX_AVIAN_BGT60TR13C;
            else if (sensor == "BGT60ATR24C")
                return IFX_AVIAN_BGT60ATR24C;
            else if (sensor == "BGT60UTR13D")
                return IFX_AVIAN_BGT60UTR13D;
            else if (sensor == "BGT60UTR12E")
                return IFX_AVIAN_BGT60TR12E;
            else if (sensor == "BGT60UTR11")
                return IFX_AVIAN_BGT60UTR11;
            else if (sensor == "BGT120UTR13E")
                return IFX_AVIAN_BGT120UTR13E;
            else if (sensor == "BGT24LTR24")
                return IFX_AVIAN_BGT24LTR24;
            else if (sensor == "BGT120UTR24")
                return IFX_AVIAN_BGT120UTR24;
            else if (sensor == "BGT24ATR22")
                return IFX_MIMOSE_BGT24ATR22;
            else if (sensor == "BGT60LTR11")
                return IFX_BGT60LTR11AIP;
            else
                return IFX_RADAR_SENSOR_UNKNOWN;
        }

        inline const char* board_type_to_string(ifx_Board_Type_t board_type)
        {
            switch (board_type)
            {
            case IFX_BOARD_BASEBOARD_AURIX:
                return "RadarBaseboardAurix";
            case IFX_BOARD_BASEBOARD_MCU7:
                return "RadarBaseboardMCU7";
            case IFX_BOARD_BASEBOARD_V9:
                return "V9";
            default:
                return "unknown";
            }
        }

        /**
         * @brief Open board by unique id
         *
         * Open a board by UUID and return the Strata BoardInstance. If no
         * board with that uuid was found nullptr is returned.
         *
         * @param [in]    uuid        uuid as array of 16 bytes
         * @retval        board       board instance
         */
        std::unique_ptr<BoardInstance> open_by_uuid(const uint8_t uuid[16]);

        /**
         * @brief Returns list of boards
         *
         * Returns a vector with all boards connected where the selector
         * function returns true.
         *
         * @param [in]    selector    selector function
         */
        std::vector<ifx_Radar_Sensor_List_Entry_t> get_list(const SelectorFunction& selector);

        /**
         * @brief Open first board found
         *
         * Opens the first board found for which the selector function returns true.
         *
         * @param [in]    selector    selector function
         */
        std::unique_ptr<BoardInstance> open(const SelectorFunction& selector);

        /**
         * @brief Return firmware info
         *
         * The returned firmware info is valid until the board instance is
         * valid. If the board instance is destroyed the returned firmware
         * info must no longer be accessed.
         *
         * @param [in]    board     board instance
         * @retval        info      firmware info
         */
        void get_firmware_info(BoardInstance* board, ifx_Firmware_Info_t* firmware_info);

        /**
         * @brief checks whether the firmware is valid
         * 
         * firmware info is compared to the minimum firmware requirement
         * returns true if firmware is valid otherwise false
         * 
         * @param [in]    min_version   minimum required version 
         * @param [in]    firmware_info firmware info 
         * @retval        bool          is valid
         */
        bool is_firmware_version_valid(const std::array<uint16_t, 3> &min_version, ifx_Firmware_Info_t firmware_info);

        /**
         * @brief Return shield info
         * 
         * The returned shield info is obtained by reading the shield EEPROM
         * contents. In case the shield EEPROM is not present, not readable,
         * or not initialised, the function returns an error.
         *
         * @param [in]    board         board instance
         * @param [in]    shield_info   shield info
         * @retval        status        execution status
         */
        ifx_Error_e get_shield_info(BoardInstance* board, ifx_RF_Shield_Info_t *shield_info);

        // Execute function func and catch the exception if an exception occurs
        //
        // This template executes the function func. If func throws an exception, the
        // template catches the exception, sets the corresponding IFX error using
        // ifx_error_set, and returns default_return_value. If func doesn't throw
        // an exception, the return value of func is returned.
        //
        // Example:
        //     auto f = [&handle]() -> bool {
        //         return handle->acquisition_started;
        //     };
        //
        //     bool result = exception_handler<bool>(f, false); // returns false if an exception occurs
        template <typename ReturnType>
        ReturnType exec_func(const std::function<ReturnType()>& func, const ReturnType default_return_value)
        {
            try
            {
                return func();
            }
            catch (const std::bad_alloc&)
            {
                ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
            }
            catch (const rdk::exception::exception& e)
            {
                ifx_error_set(e.error_code());
            }
            catch (const EConnection&)
            {
                ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
            }
            catch (const EException&)
            {
                ifx_error_set(IFX_ERROR);
            }
            catch (const std::exception&)
            {
                ifx_error_set(IFX_ERROR);
            }
            catch (...)
            {
                ifx_error_set(IFX_ERROR);
            }

            return default_return_value;
        }

        // Similar like execute_function_with_exceptions above but for void functions
        inline void exec_func(const std::function<void(void)>& func)
        {
            // Adapt func such that we can use the more generic exception handler
            // We add a return type (nullptr_t) such that we can use the definition
            // of exception handler with a default return argument.
            auto adapter = [&func]() {
                func();
                return nullptr;
            };

            exec_func<std::nullptr_t>(adapter, nullptr);
        }


        template <typename Caller, typename Returner, typename... Args>
        auto exception_handler(Caller caller, Returner returner, Args &...args) -> decltype(caller(args...))
        {
            try
            {
                return caller(args...);
            }
            catch (const std::bad_alloc&)
            {
                ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
            }
            catch (const rdk::exception::exception& e)
            {
                ifx_error_set(e.error_code());
            }
            catch (const EConnection&)
            {
                ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
            }
            catch (const EException&)
            {
                ifx_error_set(IFX_ERROR);
            }
            catch (const std::exception&)
            {
                ifx_error_set(IFX_ERROR);
            }
            catch (...)
            {
                ifx_error_set(IFX_ERROR);
            }

            return returner();
        }


        // Comments on the following templates:
        // - we should use Args &&... instead of Args &..., but then we could not match the function parameters
        //   so to keep the template simple, this was done, and should be optimized anyways...
        // - we need identical overloads for const member function, otherwise we would not be able to deduce the type


        // Execute callable "caller" and catch the exception if an exception occurs
        //
        // This template executes the callable "caller".
        // If func throws an exception, the template catches the exception,
        // sets the corresponding IFX error using ifx_error_set(), and returns default_return_value.
        // If func doesn't throw an exception, the return value of func is returned.
        // There is also an overload for void function that does not have a default_return_value.
        //
        // Example:
        //     auto f = [&value](param1) -> bool {
        //         return do_stuff(value, param1);
        //     };
        //
        //     bool result = call_func(f, false, param1); // returns false if an exception occurs
        template <typename Caller, typename DefaultType, typename... Args>
        auto call_func(Caller caller, DefaultType &&default_return_value, Args &...args) -> decltype(caller(args...))
        {
            auto returner = [&default_return_value]()
            {
                return default_return_value;
            };
            return exception_handler(caller, returner, args...);
        }

        template <typename Caller, typename... Args>
        void call_func(Caller caller, Args &...args)
        {
            auto returner = [](){};
            exception_handler(caller, returner, args...);
        }

        // Execute member function "func" and catch the exception if an exception occurs
        //
        // This template executes the member function "func" of the instance given by handle.
        // If func throws an exception, the template catches the exception,
        // sets the corresponding IFX error using ifx_error_set(), and returns default_return_value.
        // If func doesn't throw an exception, the return value of func is returned.
        // There is also an overload for void function that does not have a default_return_value.
        //
        // Example:
        //     bool result = call_func(handle, &Class::set_config, false, param1); // returns false if an exception occurs
        template <typename Return, typename Class, typename DefaultType, typename... Args>
        Return call_func(Class *handle, Return (Class::*func)(Args...), DefaultType &&default_return_value, Args &...args)
        {
            auto caller = [handle, func](Args &...args)
            {
                return (handle->*func)(args...);
            };
            return call_func(caller, std::forward<DefaultType>(default_return_value), args...);
        }

        template <typename Class, typename... Args>
        void call_func(Class *handle, void (Class::*func)(Args...), Args &...args)
        {
            auto caller = [handle, func](Args &...args)
            {
                (handle->*func)(args...);
            };
            call_func(caller, args...);
        }

        template <typename Return, typename Class, typename DefaultType, typename... Args>
        Return call_func(const Class *handle, Return (Class::*func)(Args...) const, DefaultType &&default_return_value, Args &...args)
        {
            auto caller = [handle, func](Args &...args)
            {
                return (handle->*func)(args...);
            };
            return call_func(caller, std::forward<DefaultType>(default_return_value), args...);
        }

        template <typename Class, typename... Args>
        void call_func(const Class *handle, void (Class::*func)(Args...) const, Args &...args)
        {
            auto caller = [handle, func](Args &...args)
            {
                (handle->*func)(args...);
            };
            call_func(caller, args...);
        }
    }
}

#endif /* IFX_RADAR_DEVICE_COMMON_HPP */
