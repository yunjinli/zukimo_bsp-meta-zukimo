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

#ifndef IFX_BASE_UTIL_INTERNAL_H
#define IFX_BASE_UTIL_INTERNAL_H

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

/**
 * @brief Check for overflow in multiplication of two integers of type size_t
 *
 * This function computes the product of a and b. If the multiplication
 * overflows true is returned. If the multiplication does not overflow, the
 * result a*b is written to result and false is returned.
 *
 * @param [in]  a       first multiplicand
 * @param [in]  b       second multiplicand
 * @param [out] result  a*b if no overflow occurs
 * @retval true         if an overflow occurs
 * @retval false        if no overflow occurs
 */
IFX_DLL_PUBLIC
bool ifx_util_overflow_mul_size_t(size_t a, size_t b, size_t* result);

/**
 * @brief Check for overflow in multiplication of three integers of type size_t
 *
 * This function computes the product of a, b, and c. If the multiplication
 * overflows true is returned. If the multiplication does not overflow, the
 * result a*b*c is written to result and false is returned.
 *
 * @param [in]  a       first multiplicand
 * @param [in]  b       second multiplicand
 * @param [in]  c       third multiplicand
 * @param [out] result  a*b*c if no overflow occurs
 * @retval true         if an overflow occurs
 * @retval false        if no overflow occurs
 */
IFX_DLL_PUBLIC
bool ifx_util_overflow_mul3_size_t(size_t a, size_t b, size_t c, size_t* result);

/**
 * @brief Check for overflow in addition of two integers of type size_t
 *
 * This function computes the sum of a and b. If the addition
 * overflows true is returned. If the addition does not overflow, the
 * result a+b is written to result and false is returned.
 *
 * @param [in]  a       first addend
 * @param [in]  b       second addend
 * @param [out] result  a+b if no overflow occurs
 * @retval true         if an overflow occurs
 * @retval false        if no overflow occurs
 */
IFX_DLL_PUBLIC
bool ifx_util_overflow_add_size_t(size_t a, size_t b, size_t* result);


/*
 * @brief Create copy of string
 *
 * This function returns a pointer to a new string which is a duplicate of the string orig.
 * Memory for the new string is obtained with ifx_mem_alloc, and can be freed with ifx_mem_free.
 *
 * The caller is responsible to free the memory.
 *
 * @param [in]  orig    input string
 * @retval      copy    copy of string if successful
 * @retval      NULL    on errors
 */
IFX_DLL_PUBLIC
char* ifx_util_strdup(const char* orig);

/*
 * @brief Return number of bits set to 1 in variable
 *
 * Returns the number of bits that are set to 1 in the variable mask.
 *
 * @param [in]	mask	input
 * @retval	number of one bits in mask
 */
IFX_DLL_PUBLIC
uint32_t ifx_util_popcount(uint32_t mask);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_BASE_UTIL_INTERNAL_H */
