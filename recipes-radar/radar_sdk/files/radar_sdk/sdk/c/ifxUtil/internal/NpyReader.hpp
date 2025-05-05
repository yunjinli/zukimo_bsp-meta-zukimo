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

#ifndef IFX_UTIL_NPY_H
#define IFX_UTIL_NPY_H

#include <stdexcept>
#include <cstdint>
#include <vector>

#include <ifxBase/Types.h>
#include <ifxBase/Cube.h>
#include <ifxBase/Matrix.h>
#include <ifxBase/Vector.h>

class NPYException : public std::runtime_error {
public:
    NPYException(const char* what) : std::runtime_error(what) { }
};

namespace NPYErrorMessages
{
  constexpr auto COMPUTING_DATA_SIZE_OVERFLOW = "Overflow while computing data size";
  constexpr auto PARSING_SHAPE_LENGHT_ZERO = "Error parsing shape";
  constexpr auto PARSING_SHAPE_LENGHT_EXCEEDS_LIMIT = "Error parsing shape: Size of dimension too big";
  constexpr auto NO_MAGIC_TOKEN = "Could not find magic string";
  constexpr auto UNSUPPORTED_VERSION = "Expected npy file format version 1.0";
  constexpr auto INVALID_FORMAT = "Invalid format";
  constexpr auto FORTRAN_ATTRIBUTE_ERROR = "Error parsing fortran order";
  constexpr auto DESCRIPTION_ATTRIBUTE_ERROR = "Error parsing descr";
  constexpr auto SHAPE_ATTRIBUTE_ERROR = "Error parsing shape";
  constexpr auto PREAMBLE_SIZE_MISMATCH = "Npy file too short: Preamble incomplete";
  constexpr auto HEADER_INCOMPLETE = "Npy file too short: Header incomplete";
  constexpr auto MISMATCH_DIMENSION_WHEN_READING = "Dimension mismatch";
  constexpr auto OUT_OF_BOUNDS_WHEN_READING = "Out of bounds";
  constexpr auto UNRECOGNIZED_DATA_TYPE = "Unrecognized data type";
  constexpr auto INCOMPATIBLE_DIMENSION = "Wrong dimension";
  constexpr auto MEMORY_ALLOCATION_FAILED = "Memory allocation failed";
}

class NPYInfo
{
public:
    char byte_order = 0;
    bool fortran_order = false;
    std::string dtype = "";
    size_t dtype_size = 0;
    std::vector<uint32_t> shape;
    size_t offset = 0;
    size_t size_data = 0;
    size_t nelems = 0;
};

class NpyReader
{
public:
    NpyReader(const uint8_t* data, size_t size);

    NPYInfo get_info() const { return m_info; }

    ifx_Float_t get_real(const std::vector<size_t>& addr) const;
    ifx_Complex_t get_complex(const std::vector<size_t>& addr) const;

    ifx_Float_t get_scalar_r() const;
    ifx_Complex_t get_scalar_c() const;

    ifx_Vector_R_t* get_vector_r() const;
    ifx_Vector_C_t* get_vector_c() const;

    ifx_Matrix_R_t* get_matrix_r() const;
    ifx_Matrix_C_t* get_matrix_c() const;

    ifx_Cube_R_t* get_cube_r() const;
    ifx_Cube_C_t* get_cube_c() const;

    ifx_Cube_R_t* get_cube_r_at(size_t frame_num, ifx_Cube_R_t* frame, float scale) const;

private:
    NPYInfo m_info;
    const uint8_t* m_data = nullptr;
    char m_byte_order;

    const uint8_t* get_element_pointer(const std::vector<size_t>& addr) const;
};

#endif /* IFX_UTIL_NPY_H */
