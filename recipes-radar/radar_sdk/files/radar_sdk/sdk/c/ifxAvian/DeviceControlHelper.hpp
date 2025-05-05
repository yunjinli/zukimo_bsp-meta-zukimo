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
 * @file DeviceControlHelper.hpp
 *
 * \brief \copybrief gr_devicecontrol
 *
 * For details refer to \ref gr_devicecontrol
 */

#ifndef IFX_RADAR_DEVICE_CONTROL_HELPER_HPP
#define IFX_RADAR_DEVICE_CONTROL_HELPER_HPP

/*
==============================================================================
   0. FORWARD DECLARATIONS
==============================================================================
*/

class BoardInstance;

namespace Infineon
{
    namespace Avian
    {
        class Driver;
        class StrataPort;
    }
}

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "ifxBase/Types.h"
#include "ifxAvian/DeviceControl.h"
#include "ifxAvian_CwController.hpp"
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

/** @addtogroup gr_cat_Radar
  * @{
  */

/** @defgroup gr_devicecontrol Device Control
  * @brief API for radar device control helper functions
  * @{
  */

/**
 * @brief Get pointer to Avian Driver
 *        The ownership of the returned driver object stays with the radar device object.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return                  Pointer to the Avian Driver.
 *
 */
IFX_DLL_PUBLIC
Infineon::Avian::Driver* ifx_avian_get_avian_driver(const ifx_Avian_Device_t* handle);

/**
 * @brief Get pointer to the Strata Avian Board
 *        The ownership of the returned driver object stays with the radar device object.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return                  Pointer to the Strata Avian Board.
 *
 */
IFX_DLL_PUBLIC
BoardInstance* ifx_avian_get_strata_avian_board(const ifx_Avian_Device_t* handle);

/**
 * @brief Get pointer to the Strata Avian Port
 *        The ownership of the returned driver object stays with the radar device object.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return                  Pointer to the Strata Avian Port.
 *
 */
IFX_DLL_PUBLIC
Infineon::Avian::StrataPort* ifx_avian_get_strata_avian_port(const ifx_Avian_Device_t* handle);

/**
 * @brief Get pointer to the CW controller
 *        The ownership of the returned driver object stays with the radar device object.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return                  Pointer to the CW controller.
 *
 */
IFX_DLL_PUBLIC
Infineon::Avian::Constant_Wave_Controller* ifx_avian_get_constant_wave_controller(ifx_Avian_Device_t* handle);
/**
  * @}
  */

/**
  * @}
  */

#endif /* IFX_RADAR_DEVICE_CONTROL_HELPER_HPP */

/* --- End of File -------------------------------------------------------- */
