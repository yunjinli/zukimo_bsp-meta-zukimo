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
 * @file Mmap.h
 *
 * @brief This file defines the file memory mapping I/O.
  */

#ifndef SDK_MMAP_H
#define SDK_MMAP_H

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

/*
==============================================================================
   3. TYPES
==============================================================================
*/

typedef void ifx_MMAP_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/// Creates a memory mapped file and retrieves a handle to it.
IFX_DLL_HIDDEN ifx_MMAP_t* ifx_mmap_create(const char* filename, size_t* length);

/// Frees and destroys a memory mapped file.
IFX_DLL_HIDDEN void ifx_mmap_destroy(ifx_MMAP_t* handle);

/// Gets the read-only access to the memory mapped file at a specific offset (if specified).
IFX_DLL_HIDDEN const uint8_t* ifx_mmap_const_data(ifx_MMAP_t* handle, uint32_t offset = 0);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* SDK_MMAP_H */
