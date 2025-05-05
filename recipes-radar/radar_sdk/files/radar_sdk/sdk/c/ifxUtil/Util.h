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
 * @file Util.h
 *
 * @brief This file defines some utility functions like printing to, reading from files
 *        or printing to console. The scope of these functions are internal, and not to be
 *        released as part of the SDK.
 *
 * For details refer to \ref gr_cat_Utilities
 *
 * \defgroup gr_cat_Utilities              Utility functionality (ifxUtil)
 */

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
// NOLINTNEXTLINE(modernize-deprecated-headers)
#include <stdlib.h>
// NOLINTNEXTLINE(modernize-deprecated-headers)
#include <stdio.h>
// NOLINTNEXTLINE(modernize-deprecated-headers)
#include <string.h>

#include "ifxBase/Vector.h"
#include "ifxBase/Matrix.h"
#include "ifxBase/Cube.h"
#include "ifxBase/Complex.h"
#include "ifxBase/Error.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

#define IFX_TYPE_INVALID 0
#define IFX_TYPE_SCALAR_REAL 1
#define IFX_TYPE_SCALAR_COMPLEX 2
#define IFX_TYPE_VECTOR_REAL 3
#define IFX_TYPE_VECTOR_COMPLEX 4
#define IFX_TYPE_MATRIX_REAL 5
#define IFX_TYPE_MATRIX_COMPLEX 6
#define IFX_TYPE_CUBE_REAL 7
#define IFX_TYPE_CUBE_COMPLEX 8

/*
==============================================================================
   3. TYPES
==============================================================================
*/

// forward declare npz_t
struct ifx_npz_s;
typedef struct ifx_npz_s ifx_npz_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_Utilities
  * @{
  */

/** @defgroup gr_utilities Utilities
  * @brief API for uitlity function for printing etc. This module is considered internal and the API of its fucntions might not be stable.
  * @{
  */

/**
 * @brief Reads .npy file
 *
 * Read the .npy filename given by filename. If successful, the pointer to the
 * corresponding ifx data type is returned. The data type is written to type.
 *
 * The caller must cast the pointer to the correct data type:
 * - type=IFX_TYPE_VECTOR_REAL: ifx_Vector_R_t*
 * - type=IFX_TYPE_VECTOR_COMPLEX: ifx_Vector_C_t*
 * - type=IFX_TYPE_MATRIX_REAL: ifx_Matrix_R_t*
 * - type=IFX_TYPE_MATRIX_COMPLEX: ifx_Matrix_C_t*
 * - type=IFX_TYPE_CUBE_REAL: ifx_Cube_R_t*
 * - type=IFX_TYPE_CUBE_COMPLEX: ifx_Cube_C_t*
 *
 * If an error occurred NULL is returned and type is set to IFX_TYPE_INVALID.
 *
 * The caller is responsible to release the memory after use for the
 * vector/matrix/cube.
 *
 * The npy file format is described in
 * https://numpy.org/neps/nep-0001-npy-format.html.
 *
 * @param [in]     filename  path to .npy file
 * @param [out]    type      type of returned object
 * @retval                   pointer to object type if successful
 * @retval                   NULL is an error occurred
 */
void* ifxu_npy_read(const char* filename,
                        int* type);

/**
 * @brief Write .npy file
 *
 * Write object to .npy file given by filename. If successful, it will
 * return true.
 *
 * @param [in]  filename    path to .npy file
 * @param [in]  type        type of input object
 * @param [in]  object      pointer to object to write out
 * @return true             succeed
 * @return false            fail
 */
bool ifxu_npy_write(const char* filename, int type, const void *object);

/**
 * @brief Write .npy file
 *
 * Write raw data to .npy file given by filename. If successful, it will
 * return true.
 * 
 * example queue array parameters for shape matrix:
 * - 4D: (frames, antennas, chirps, samples)
 * - 3D: (rows, cols, slices)
 * - 2D: (rows, cols)
 * - 1D: (len)
 *
 * @param [in]  filename        path to .npy file
 * @param [in]  data            input array
 * @param [in]  is_complex      True if elements of data are complex, otherwise false
 * @param [in]  fortran_order   True if data is Fortran order, otherwise false
 * @param [in]  shape           dimensions of data - 
 * @param [in]  num_shape       number of elements of array shape - example 1D is 1, 4D is 4;
 * @return true                 succeed
 * @return false                fail
 */

bool ifxu_npy_write_raw(const char* filename, uint16_t* data, bool is_complex, bool fortran_order, uint64_t shape[], int num_shape);

/**
 * @brief Frees memory allocated by ifxu_npy_read
 *
 * Depending on type the correct destroy function is called. If type is invalid
 * or p is NULL, the function has no effect.
 *
 * @param [in]     p         pointer returned by ifxu_npy_read
 * @param [in]     type      type as returned by ifxu_npy_read
 */
void ifxu_npy_free(void* p,
                       int type);

/// Read real float from npy file
ifx_Float_t* ifxu_npy_read_scalar_r(const char* filename);

/// Read complex float from npy file
ifx_Complex_t* ifxu_npy_read_scalar_c(const char* filename);

/// Read real vector from npy file
ifx_Vector_R_t* ifxu_npy_read_vec_r(const char* filename);

/// Read complex vector from npy file
ifx_Vector_C_t* ifxu_npy_read_vec_c(const char* filename);

/// Read real matrix from npy file
ifx_Matrix_R_t* ifxu_npy_read_mat_r(const char* filename);

/// Read complex matrix from npy file
ifx_Matrix_C_t* ifxu_npy_read_mat_c(const char* filename);

/// Read real cube from npy file
ifx_Cube_R_t* ifxu_npy_read_cube_r(const char* filename);

/// Read complex cube from npy file
ifx_Cube_C_t* ifxu_npy_read_cube_c(const char* filename);

/// Write real float to npy file
bool ifxu_npy_write_scalar_r(const char* filename, const ifx_Float_t* object);

/// Write complex to npy file
bool ifxu_npy_write_scalar_c(const char* filename, const ifx_Complex_t* object);

/// Write real vector to npy file
bool ifxu_npy_write_vec_r(const char* filename, const ifx_Vector_R_t* object);

/// Write complex vector to npy file
bool ifxu_npy_write_vec_c(const char* filename, const ifx_Vector_C_t* object);

/// Write real matrix to npy file
bool ifxu_npy_write_mat_r(const char* filename, const ifx_Matrix_R_t* object);

/// Write complex matrix to npy file
bool ifxu_npy_write_mat_c(const char* filename, const ifx_Matrix_C_t* object);

/// Write real cube to npy file
bool ifxu_npy_write_cube_r(const char* filename, const ifx_Cube_R_t* object);

/// Write complex cube to npy file
bool ifxu_npy_write_cube_c(const char* filename, const ifx_Cube_C_t* object);

/**
 * @brief Opens npz file for reading
 *
 * Open the npz file filename for reading. An npz file is a (uncompressed) zip
 * file that contains npy files.
 *
 * @param [in]     filename  path to the npz file
 *
 * @retval     pointer to ifx_npz_t structure if successful
 * @retval     NULL is an error occurred
 */
ifx_npz_t* ifxu_npz_open(const char* filename);

/**
 * @brief Closes npz file
 *
 * Close the npz file.
 *
 * @param [in]     archive   npz archive
 */
void ifxu_npz_close(ifx_npz_t* archive);

/**
 * @brief Gets the number of entries in archive
 *
 * @param [in]     archive   npz archive
 *
 * @retval uElems       number of entries in archive
 */
size_t ifxu_npz_num_entries(ifx_npz_t* archive);

/**
 * @brief Gets name of entry for index
 *
 * Given the index of an entry in the archive, return the entry's name.
 *
 * @param [in]     archive   npz archive
 * @param [in]     index     index of entry
 *
 * @retval         name      name of entry with given index
 */
const char* npz_get_name(ifx_npz_t* archive,
                         size_t index);

/**
 * @brief Reads name from archive
 *
 * Read the entry name from the archive. The object is returned, the type is
 * written to type, see also \ref ifxu_npy_read.
 *
 * @param [in]     archive   npz archive
 * @param [in]     name      name of entry
 * @param [out]    type      type of data
 *
 * @retval         object   object (real/complex vector/matrix/cube) if successful
 * @retval         NULL     if an error occurred
 */
void* ifxu_npz_read(ifx_npz_t* archive,
                        const char* name,
                        int* type);

// Reads real float from archive
ifx_Float_t* ifxu_npz_read_scalar_r(ifx_npz_t* archive,
                                        const char* name);

// Reads complex float from archive
ifx_Complex_t* ifxu_npz_read_scalar_c(ifx_npz_t* archive,
                                          const char* name);

// Reads real vector from archive
ifx_Vector_R_t* ifxu_npz_read_vec_r(ifx_npz_t* archive,
                                        const char* name);

// Reads complex vector from archive
ifx_Vector_C_t* ifxu_npz_read_vec_c(ifx_npz_t* archive,
                                        const char* name);

// Reads real matrix from archive
ifx_Matrix_R_t* ifxu_npz_read_mat_r(ifx_npz_t* archive,
                                        const char* name);

// Reads complex matrix from archive
ifx_Matrix_C_t* ifxu_npz_read_mat_c(ifx_npz_t* archive,
                                        const char* name);

// Reads real cube from archive
ifx_Cube_R_t* ifxu_npz_read_cube_r(ifx_npz_t* archive,
                                       const char* name);

// Reads complex cube from archive
ifx_Cube_C_t* ifxu_npz_read_cube_c(ifx_npz_t* archive,
                                       const char* name);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* UTIL_H */
