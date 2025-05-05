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

#if _MSC_VER
/* suppress warning about unsafe function strcpy */
#pragma warning(disable:4996)
#endif

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "ifxBase/Mem.h"
#include "ifxBase/internal/Util.h"

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

/*
==============================================================================
   4. LOCAL DATA
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

/* internal function */
char* ifx_util_strdup(const char* orig)
{
    char* copy = ifx_mem_calloc(strlen(orig) + 1, sizeof(char));
    if (copy)
        strcpy(copy, orig);

    return copy;
}

/* internal function */
bool ifx_util_overflow_mul_size_t(size_t a, size_t b, size_t* result)
{
    /* if a or b is 0, the result is result = 0 */
    if(a == 0 || b == 0)
    {
        *result = 0;
        return false;
    }

    const size_t ab = a*b;
    
    if(ab/a == b)
    {
        /* no overflow */
        *result = ab;
        return false;
    }

    /* overflow */
    return true;
}

/* internal function */
bool ifx_util_overflow_mul3_size_t(size_t a, size_t b, size_t c, size_t* result)
{
    size_t ab;
    if(ifx_util_overflow_mul_size_t(a, b, &ab))
        return true;
    
    return ifx_util_overflow_mul_size_t(ab, c, result);
}

/*internal function*/
bool ifx_util_overflow_add_size_t(size_t a, size_t b, size_t* result)
{
    if(a > 0 && b > (SIZE_MAX - a))
    {
        /* overflow */
        return true;
    }
    /* no overflow */
    *result = a+b;
    return false;
}

uint32_t ifx_util_popcount(uint32_t mask)
{
    uint32_t count = 0;

    for (; mask; mask >>= 1)
    {
        count += mask & 0x01;
    }

    return count;
}
