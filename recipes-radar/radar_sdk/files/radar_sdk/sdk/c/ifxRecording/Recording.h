/* ===========================================================================
** Copyright (C) 2022 Infineon Technologies AG
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
  * @file Recording.h
  *
  * \brief This file defines functions to handle Recordings.
  *
  * For details refer to \ref gr_cat_SDK_recording
  *
  * \defgroup gr_cat_SDK_recording              Recordings (ifxRecording)
  */

#ifndef IFX_RECORDING_RECORDING_H
#define IFX_RECORDING_RECORDING_H

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

#include "ifxBase/Types.h"
#include "ifxAvian/DeviceConfig.h"
#include "ifxBase/List.h"

/** @addtogroup gr_cat_SDK_recording
  * @{
  */

/** @defgroup gr_recording Recording
  * @brief API for Recording handling
  * @{
  */

/**
 * @brief Enum with supported Recording types.
 *
 * Recording types are associated with devices families.
 *
 * @attention Currently supporting only Avian Radar devices.
 */
typedef enum {
  IFX_RECORDING_AVIAN,
	// currently only supporting avian recordings
	IFX_RECORDING_UNKNOWN
} ifx_Recording_Type_t;


/**
 * @brief Structure of ifx_Recording_Entry_t.
 *
 * Defines the structure for the values read from existing directories that stores radar recordings.
 */
	
typedef struct {
	/* postfix directory index. Format is 'RecordingName_xx' where xx is 00 to 99, -1 means directories is not exist. */
	int32_t index;
	/* type of recording e.g. Avian. */
	ifx_Recording_Type_t type;
	/* the main directory path, where the recording is stored. */
	const char* parent_path;
}ifx_Recording_Entry_t;

/**
 * @brief Enum with possible Recording modes.
 *
 * @attention Currently supporting only Read mode.
 */
typedef enum {
    IFX_RECORDING_READ_MODE,
	  IFX_RECORDING_WRITE_MODE,
    IFX_RECORDING_READ_WRITE_MODE
} ifx_Recording_Mode_t;

/**
 * @brief Structure representing the common file format recording.
 */
typedef struct Recording ifx_Recording_t;

/**
 * @brief Creates new Recording.
 *
 * Create a new Recording from a given path to recording.
 * Given path must be a proper recording directory, otherwise NULL is returned.
 * 
 * @param [in]  path	path to a directory containing the recording (parent directory)
 * @param [in]  mode	ifx_Recording_Mode_t for read/write of the recording
 *                    Note! Currently only Read mode is supported.
 * @param	[in]  type	ifx_Recording_Type_t type of the recording (associated with device type)
 *                    Note! Currently only Avian recording type is supported.
 * @param	[in]  index	order number of the recording Catalogs with recordings are named: "RadarIfx<RadarType>_XX"
                    where XX is a recording index, e.g. RadarIfxAvian_01.
                    For numbers lower than 10, one provides just a number 0-9.
 * @return ifx_Recording_t handle if successful
 * @return NULL if an error occurred
 */
IFX_DLL_PUBLIC
ifx_Recording_t* ifx_recording_create(const char* path,
									  ifx_Recording_Mode_t mode,
									  ifx_Recording_Type_t type,
									  int32_t index);

/**
 * @brief Destroys the Recording.
 *
 * Removes allocated resources and closes the recording.
 *
 * @param [in]  recording	handle to the recording
 */
IFX_DLL_PUBLIC
void ifx_recording_destroy(ifx_Recording_t* recording);

/**
 * @brief Gets a string informing about format version.
 *
 * The format version string is an information about the version of the recording format.
 *
 * @param [in]  recording	handle to the recording
 * @return c-string with format version
 * @return NULL if recording handle is NULL
 */
IFX_DLL_PUBLIC
const char* ifx_recording_get_format_version(ifx_Recording_t* recording);

/**
 * @brief Get Recording list. 
 *
 * A recording list returned Avian or Unknown recording from a given path to recording resources.
 * Given path must be a proper recording directory, otherwise proper ifx_error occurred.
 * All error codes could be captured by ifx_error_get.
 * The returned list memory has to be freed, by using ifx_list_destroy(ifx_List_t* )
 * 
 * @param [in] path to a directory containing the recording.
 * @return ifx_List_t if successful or ifx_error.
  */

IFX_DLL_PUBLIC
ifx_List_t* ifx_recording_list(const char* path);

/**
  * @}
  */ /* gr_cat_SDK_recording */ 
 
/**
  * @}
  */ /* gr_recording Recording */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RECORDING_RECORDING_H */