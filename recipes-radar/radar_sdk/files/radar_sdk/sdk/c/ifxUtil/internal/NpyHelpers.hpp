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

#ifndef IFX_UTIL_NPYHELPERS_H
#define IFX_UTIL_NPYHELPERS_H

#include <ifxBase/Types.h>
#include <ifxBase/Cube.h>
#include <ifxBase/Matrix.h>
#include <ifxBase/Vector.h>
#include <vector>


namespace NpyInternal{

// create vector allowed specializations
template<typename T>
T* create_vector(const uint32_t length)
{
  return nullptr;
}

template<>  
inline ifx_Vector_R_t* create_vector<ifx_Vector_R_t>(const uint32_t length)
{
  return ifx_vec_create_r(length);
}

template<>
inline ifx_Vector_C_t* create_vector<ifx_Vector_C_t>(const uint32_t length)
{
  return ifx_vec_create_c(length);
}

// create matrix allowed specializations
template<typename T>
inline T* create_matrix(const uint32_t rows, const uint32_t columns)
{
  return nullptr;
}

template<>
inline ifx_Matrix_R_t* create_matrix<ifx_Matrix_R_t>(const uint32_t rows, const uint32_t columns)
{
  return ifx_mat_create_r(rows, columns);
}

template<>
inline ifx_Matrix_C_t* create_matrix<ifx_Matrix_C_t>(const uint32_t rows, const uint32_t columns)
{
  return ifx_mat_create_c(rows, columns);
}

// create cube allowed specializations
template<typename T>
T* create_cube(const uint32_t rows, const uint32_t columns, const uint32_t slices)
{
  return nullptr;
}

template<>
inline ifx_Cube_R_t* create_cube<ifx_Cube_R_t>(const uint32_t rows, const uint32_t columns, const uint32_t slices)
{
  return ifx_cube_create_r(rows, columns, slices);
}

template<>
inline ifx_Cube_C_t* create_cube<ifx_Cube_C_t>(const uint32_t rows, const uint32_t columns, const uint32_t slices)
{
  return ifx_cube_create_c(rows, columns, slices);
}

} // end of anonymous namespace


namespace NpyHelpers
{

template<typename TVec, typename TVal>
TVec* get_vector(const size_t len, std::function<TVal(const std::vector<size_t>&)> element_getter)
{
  TVec* v = NpyInternal::create_vector<TVec>(len);

  if (!v) {
    return nullptr;
  }

  for (uint32_t i = 0; i < len; ++i) {
    IFX_VEC_AT(v, i) = element_getter({ i });
  }

  return v;
}

template<typename TMat, typename TVal>
TMat* get_matrix(const size_t rows, const size_t cols, std::function<TVal(const std::vector<size_t>&)> element_getter)
{
  TMat* m = NpyInternal::create_matrix<TMat>(rows, cols);

  if (!m) {
    return nullptr;
  }

  for (uint32_t r = 0; r < rows; ++r) {
      for (uint32_t c = 0; c < cols; ++c) {
          IFX_MAT_AT(m, r, c) = element_getter({ r, c });
      }
  }

  return m;
}

template<typename TCube, typename TVal>
TCube* get_cube(const size_t rows, const size_t cols, const size_t slices, std::function<TVal(const std::vector<size_t>&)> element_getter)
{
  TCube* cube = NpyInternal::create_cube<TCube>(rows, cols, slices);

  if (!cube) {
    return nullptr;
  }

  for (uint32_t r = 0; r < rows; ++r) {
      for (uint32_t c = 0; c < cols; ++c) {
          for(uint32_t s = 0; s < slices; ++s) {
              IFX_CUBE_AT(cube, r, c, s) = element_getter({ r, c, s });
          }
      }
  }

  return cube;
}

template<typename TCube, typename TVal>
TCube* get_cube_at(const size_t cube_num, const size_t rows, const size_t cols, const size_t slices, std::function<TVal(const std::vector<size_t>&)> element_getter)
{
  TCube* cube = NpyInternal::create_cube<TCube>(rows, cols, slices);

  if (!cube) {
    return nullptr;
  }

  for (uint32_t r = 0; r < rows; ++r) {
      for (uint32_t c = 0; c < cols; ++c) {
          for(uint32_t s = 0; s < slices; ++s) {
              IFX_CUBE_AT(cube, r, c, s) = element_getter({ cube_num, r, c, s});
          }
      }
  }

  return cube;
}

} // end of NpyHelpers namespace

#endif /* IFX_UTIL_NPYHELPERS_H */
