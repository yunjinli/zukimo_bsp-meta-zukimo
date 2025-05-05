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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <float.h>

#include "ifxBase/Math.h"
#include "ifxBase/Complex.h"
#include "ifxBase/Defines.h"
#include "ifxBase/Vector.h"
#include "ifxBase/Error.h"
#include "ifxBase/internal/Macros.h"

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

ifx_Float_t ifx_math_find_max(const ifx_Vector_R_t* input,
                              uint32_t* max_idx)
{
    IFX_ERR_BRV_NULL(input, -FLT_MAX);
    IFX_ERR_BRV_ARGUMENT(vLen(input) < 1, -FLT_MAX);

    uint32_t idx = 0;
    ifx_Float_t max_val = vAt(input, 0);

    for (uint32_t i = 1; i < vLen(input); ++i)
    {
        if (vAt(input, i) > max_val)
        {
            max_val = vAt(input, i);
            idx = i;
        }
    }

    if (max_idx != NULL)
    {
        *max_idx = idx;
    }

    return max_val;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_math_calc_l1norm(const ifx_Vector_R_t* input)
{
    IFX_ERR_BRV_NULL(input, -FLT_MAX)
    IFX_ERR_BRV_ARGUMENT(vLen(input) < 1, -FLT_MAX)

    ifx_Float_t fsum = 0;

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        fsum += FABS(vAt(input, i));
    }

    return fsum;
}

//----------------------------------------------------------------------------

void ifx_math_vec_clip_lt_threshold_r(const ifx_Vector_R_t* input,
                                      const ifx_Float_t threshold,
                                      const ifx_Float_t clip_value,
                                      ifx_Vector_R_t* output)
{
    IFX_ERR_BRK_NULL(input)
    IFX_ERR_BRK_NULL(output)
    IFX_ERR_BRK_ARGUMENT(vLen(input) < 1)
    IFX_ERR_BRK_ARGUMENT(vLen(output) < 1)
    IFX_ERR_BRK_ARGUMENT(threshold < 0)

    uint32_t N = vLen(output);

    if (vLen(input) < N)
    {
        N = vLen(input);
    }

    for (uint32_t i = 0; i < N; i++)
    {
        if (vAt(input, i) < threshold)
        {
            vAt(output, i) = clip_value;
        }
        else
        {
            vAt(output, i) = vAt(input, i);
        }
    }
}

//----------------------------------------------------------------------------

void ifx_math_vec_clip_gt_threshold_r(const ifx_Vector_R_t* input,
                                      const ifx_Float_t threshold,
                                      const ifx_Float_t clip_value,
                                      ifx_Vector_R_t* output)
{
    IFX_ERR_BRK_NULL(input)
    IFX_ERR_BRK_NULL(output)
    IFX_ERR_BRK_ARGUMENT(vLen(input) < 1)
    IFX_ERR_BRK_ARGUMENT(vLen(output) < 1)
    IFX_ERR_BRK_ARGUMENT(threshold < 0)

    uint32_t N = vLen(output);

    if (vLen(input) < N)
    {
        N = vLen(input);
    }

    for (uint32_t i = 0; i < N; i++)
    {
        if (vAt(input, i) > threshold)
        {
            vAt(output, i) = clip_value;
        }
        else
        {
            vAt(output, i) = vAt(input, i);
        }
    }
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_math_linear_to_db(const ifx_Float_t input,
                                  const ifx_Float_t scale)
{
    IFX_ERR_BRV_ARGUMENT(scale == 0, -FLT_MAX)

    return scale * LOG10(input);     // always operate on real values
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_math_db_to_linear(const ifx_Float_t input,
                                  const ifx_Float_t scale)
{
    IFX_ERR_BRV_ARGUMENT(scale == 0, -FLT_MAX)

    return POW(10.0, input / scale); // always operate on real values
}

//----------------------------------------------------------------------------

int ifx_math_isclose_r(ifx_Float_t a, ifx_Float_t b, ifx_Float_t reltol, ifx_Float_t abstol)
{
    return FABS(a-b) <= MAX(reltol * MAX(FABS(a), FABS(b)), abstol);
}

//----------------------------------------------------------------------------

int ifx_math_isclose_c(ifx_Complex_t a, ifx_Complex_t b, ifx_Float_t reltol, ifx_Float_t abstol)
{
    return ifx_complex_abs(ifx_complex_sub(a,b)) <= MAX(reltol * MAX(ifx_complex_abs(a), ifx_complex_abs(b)), abstol);
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_math_abs_r(ifx_Float_t input)
{
    return FABS(input);
}

//----------------------------------------------------------------------------

uint32_t ifx_math_round_up_power_of_2_uint32(uint32_t x)
{
    // see https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;

    return x;
}

//----------------------------------------------------------------------------

int ifx_math_ispower_of_2(uint32_t n)
{
    return (n && !(n & (n - 1)));
}