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

#include <string.h>

#include "ifxBase/internal/Util.h"

#include "ifxBase/Cube.h"
#include "ifxBase/Complex.h"
#include "ifxBase/internal/Macros.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Matrix.h"
#include "ifxBase/Error.h"

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

/**
 * @brief Computes the required memory size (in bytes) for cube and checks for overflows
 *
 * The function computes the required size of memory size in bytes
 * alloc_size=rows*columns*slices*elem_size for a cube. elem_size is the size
 * of one element of the cube and is typically either sizeof(ifx_Float_t) for
 * a real cube or sizeof(ifx_Complex_t) for a complex cube.
 *
 * If an overflow occurs during multiplication true is returned. If no overflow
 * occurs, false is returned.
 *
 *
 * @param [in] rows        first dimension of the cube
 * @param [in] columns     second dimension of the cube
 * @param [in] slices      third dimension of the cube
 * @param [in] elem_size   size of one element of the cube
 * @param [out] alloc_size size of memory to allocate for the vector in bytes
 * @retval true     if an overflow occurs during multiplication or addition
 * @retval false    if no overflow occurs
 */
static bool cube_alloc_size_overflow(size_t rows, size_t columns, size_t slices, size_t elem_size, size_t* alloc_size);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

static bool cube_alloc_size_overflow(size_t rows, size_t columns, size_t slices, size_t elem_size, size_t* alloc_size)
{
    if (ifx_util_overflow_mul3_size_t(rows, columns, slices, alloc_size))
        return true;
    return ifx_util_overflow_mul_size_t(*alloc_size, elem_size, alloc_size);
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_Cube_R_t* ifx_cube_create_r(uint32_t rows,
                                uint32_t columns,
                                uint32_t slices)
{
    ifx_Cube_R_t* cube = NULL;

    IFX_ERR_BRN_ARGUMENT(rows == 0);
    IFX_ERR_BRN_ARGUMENT(columns == 0);
    IFX_ERR_BRN_ARGUMENT(slices == 0);

    size_t alloc_size = 0;
    bool overflow = cube_alloc_size_overflow(rows, columns, slices, sizeof(ifx_Float_t), &alloc_size);
    IFX_ERR_BRV_COND(overflow, IFX_ERROR_MEMORY_ALLOCATION_FAILED, NULL);

    cube = ifx_mem_calloc(1, sizeof(ifx_Cube_R_t));
    IFX_ERR_BRN_MEMALLOC(cube);

    cDat(cube) = ifx_mem_aligned_alloc(alloc_size, IFX_MEMORY_ALIGNMENT);
    if(((cube) == NULL) || ((cDat(cube)) == NULL)){
        ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
        ifx_cube_destroy_r(cube);
        return NULL;
    }
    memset(cDat(cube), 0, alloc_size);

    cube->rows = rows;
    cube->cols = columns;
    cube->slices = slices;
    cube->stride[0] = 1;
    cube->stride[1] = slices;
    cube->stride[2] = (size_t)columns * (size_t)slices;
    cube->owns_d = 1;

    return cube;
}

//----------------------------------------------------------------------------

ifx_Cube_C_t* ifx_cube_create_c(uint32_t rows,
                                uint32_t columns,
                                uint32_t slices)
{
    ifx_Cube_C_t* cube = NULL;

    IFX_ERR_BRN_ARGUMENT(rows == 0);
    IFX_ERR_BRN_ARGUMENT(columns == 0);
    IFX_ERR_BRN_ARGUMENT(slices == 0);

    size_t alloc_size;
    bool overflow = cube_alloc_size_overflow(rows, columns, slices, sizeof(ifx_Complex_t), &alloc_size);
    IFX_ERR_BRV_COND(overflow, IFX_ERROR_MEMORY_ALLOCATION_FAILED, NULL);

    cube = ifx_mem_calloc(1, sizeof(ifx_Cube_C_t));
    IFX_ERR_BRN_MEMALLOC(cube);

    cDat(cube) = ifx_mem_aligned_alloc(alloc_size, IFX_MEMORY_ALIGNMENT);
    if(((cube) == NULL) || ((cDat(cube)) == NULL)){
        ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
        ifx_cube_destroy_c(cube);
        return NULL;
    }
    memset(cDat(cube), 0, alloc_size);

    cube->rows = rows;
    cube->cols = columns;
    cube->slices = slices;
    cube->stride[0] = 1;
    cube->stride[1] = slices;
    cube->stride[2] = (size_t)columns * (size_t)slices;
    cube->owns_d = 1;

    return cube;
}

//----------------------------------------------------------------------------

void ifx_cube_get_slice_r(const ifx_Cube_R_t* cube,
                          uint32_t depth_index,
                          ifx_Matrix_R_t* slice)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(slice);
    IFX_ERR_BRK_ARGUMENT(depth_index >= cube->slices);

    mCols(slice) = cCols(cube);
    mRows(slice) = cRows(cube);
    mDat(slice) = &cAt(cube, 0, 0, depth_index);
    mStride(slice, 0) = cStride(cube, 1);
    mStride(slice, 1) = cStride(cube, 2);
    slice->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_cube_get_slice_c(const ifx_Cube_C_t* cube,
                          uint32_t depth_index,
                          ifx_Matrix_C_t* slice)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(slice);
    IFX_ERR_BRK_ARGUMENT(depth_index >= cube->slices);

    mCols(slice) = cCols(cube);
    mRows(slice) = cRows(cube);
    mDat(slice) = &cAt(cube, 0, 0, depth_index);
    mStride(slice, 0) = cStride(cube, 1);
    mStride(slice, 1) = cStride(cube, 2);
    slice->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_cube_get_row_r(const ifx_Cube_R_t* cube,
    uint32_t row_index,
    ifx_Matrix_R_t* row_matrix)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(row_matrix);
    IFX_ERR_BRK_ARGUMENT(row_index >= cRows(cube));

    mCols(row_matrix) = cSlices(cube);
    mRows(row_matrix) = cCols(cube);
    mDat(row_matrix) = &cAt(cube, row_index, 0, 0);
    mStride(row_matrix, 0) = cStride(cube, 0);
    mStride(row_matrix, 1) = cStride(cube, 1);
    row_matrix->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_cube_get_row_c(const ifx_Cube_C_t* cube,
    uint32_t row_index,
    ifx_Matrix_C_t* row_matrix)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(row_matrix);
    IFX_ERR_BRK_ARGUMENT(row_index >= cRows(cube));

    mCols(row_matrix) = cSlices(cube);
    mRows(row_matrix) = cCols(cube);
    mDat(row_matrix) = &cAt(cube, row_index, 0, 0);
    mStride(row_matrix, 0) = cStride(cube, 0);
    mStride(row_matrix, 1) = cStride(cube, 1);
    row_matrix->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_cube_get_col_r(const ifx_Cube_R_t* cube,
    uint32_t col_index,
    ifx_Matrix_R_t* col_matrix)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(col_matrix);
    IFX_ERR_BRK_ARGUMENT(col_index >= cCols(cube));

    mCols(col_matrix) = cSlices(cube);
    mRows(col_matrix) = cRows(cube);
    mDat(col_matrix) = &cAt(cube, 0, col_index, 0);
    mStride(col_matrix, 0) = cStride(cube, 0);
    mStride(col_matrix, 1) = cStride(cube, 2);
    col_matrix->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_cube_get_col_c(const ifx_Cube_C_t* cube,
    uint32_t col_index,
    ifx_Matrix_C_t* col_matrix)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(col_matrix);
    IFX_ERR_BRK_ARGUMENT(col_index >= cCols(cube));

    mCols(col_matrix) = cSlices(cube);
    mRows(col_matrix) = cRows(cube);
    mDat(col_matrix) = &cAt(cube, 0, col_index, 0);
    mStride(col_matrix, 0) = cStride(cube, 0);
    mStride(col_matrix, 1) = cStride(cube, 2);
    col_matrix->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_cube_destroy_r(ifx_Cube_R_t* cube)
{
    if (cube == NULL)
    {
        return;
    }

    cCols(cube) = 0;
    cRows(cube) = 0;
    cSlices(cube) = 0;

    if (cDat(cube) && cube->owns_d)
    {
        ifx_mem_aligned_free(cDat(cube));
        cDat(cube) = NULL;
    }

    ifx_mem_free(cube);
}

//----------------------------------------------------------------------------

void ifx_cube_destroy_c(ifx_Cube_C_t* cube)
{
    if (cube == NULL)
    {
        return;
    }

    cCols(cube) = 0;
    cRows(cube) = 0;
    cSlices(cube) = 0;

    if (cDat(cube) && cube->owns_d)
    {
        ifx_mem_aligned_free(cDat(cube));
        cDat(cube) = NULL;
    }

    ifx_mem_free(cube);
}

//----------------------------------------------------------------------------

void ifx_cube_copy_r(const ifx_Cube_R_t* cube, ifx_Cube_R_t* target)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(target);
    IFX_CUBE_BRK_DIM(cube, target);

    for (uint32_t r = 0; r < cRows(cube); r++)
    {
        for (uint32_t c = 0; c < cCols(cube); c++)
        {
            for (uint32_t s = 0; s < cSlices(cube); s++)
                cAt(target, r, c, s) = cAt(cube, r, c, s);
        }
    }
}

//----------------------------------------------------------------------------

void ifx_cube_copy_c(const ifx_Cube_C_t* cube, ifx_Cube_C_t* target)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(target);
    IFX_CUBE_BRK_DIM(cube, target);

    for (uint32_t r = 0; r < cRows(cube); r++)
    {
        for (uint32_t c = 0; c < cCols(cube); c++)
        {
            for (uint32_t s = 0; s < cSlices(cube); s++)
                cAt(target, r, c, s) = cAt(cube, r, c, s);
        }
    }
}

//----------------------------------------------------------------------------

ifx_Cube_R_t* ifx_cube_clone_r(const ifx_Cube_R_t* cube)
{
    IFX_ERR_BRN_NULL(cube);

    ifx_Cube_R_t* clone = ifx_cube_create_r(IFX_CUBE_ROWS(cube), IFX_CUBE_COLS(cube), IFX_CUBE_SLICES(cube));
    IFX_ERR_BRN_NULL(clone);

    ifx_cube_copy_r(cube, clone);

    return clone;
}

//----------------------------------------------------------------------------

ifx_Cube_C_t* ifx_cube_clone_c(const ifx_Cube_C_t* cube)
{
    IFX_ERR_BRN_NULL(cube);

    ifx_Cube_C_t* clone = ifx_cube_create_c(IFX_CUBE_ROWS(cube), IFX_CUBE_COLS(cube), IFX_CUBE_SLICES(cube));
    IFX_ERR_BRN_NULL(clone);

    ifx_cube_copy_c(cube, clone);

    return clone;
}

//----------------------------------------------------------------------------

void ifx_cube_col_abs_r(const ifx_Cube_C_t* cube,
                        uint32_t column_index,
                        ifx_Matrix_R_t* matrix)
{
    IFX_ERR_BRK_NULL(cube);
    IFX_ERR_BRK_NULL(matrix);
    IFX_ERR_BRK_ARGUMENT(column_index >= cube->cols);

    for (uint32_t r = 0; r < cRows(cube); r++)
    {
        for (uint32_t c = 0; c < cSlices(cube); c++)
        {
            mAt(matrix, r, c) = ifx_complex_abs(cAt(cube, r, column_index, c));
        }
    }
}

//----------------------------------------------------------------------------

void ifx_cube_clear_r(ifx_Cube_R_t* cube)
{
    IFX_ERR_BRK_NULL(cube);

    for (uint32_t r = 0; r < cRows(cube); r++)
    {
        for (uint32_t c = 0; c < cCols(cube); c++)
        {
            for (uint32_t s = 0; s < cSlices(cube); s++)
                cAt(cube, r, c, s) = 0;
        }
    }
}

//----------------------------------------------------------------------------

void ifx_cube_clear_c(ifx_Cube_C_t* cube)
{
    IFX_ERR_BRK_NULL(cube);

    const ifx_Complex_t complex_zero = IFX_COMPLEX_DEF(0, 0);

    for (uint32_t r = 0; r < cRows(cube); r++)
    {
        for (uint32_t c = 0; c < cCols(cube); c++)
        {
            for (uint32_t s = 0; s < cSlices(cube); s++)
                cAt(cube, r, c, s) = complex_zero;
        }
    }
}
