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

#include <cassert>
#include <cstdint>
#include <cstring>
#include <regex>
#include <string>
#include <vector>
#include <functional>

#include "internal/NpyReader.hpp"
#include "internal/Endianess.hpp"
#include "internal/NpyHelpers.hpp"

#include <ifxBase/Complex.h>

constexpr uint8_t npy_magic[] = { 0x93, 'N', 'U', 'M', 'P', 'Y' };
constexpr size_t npy_magic_offset = 0;
constexpr size_t npy_major_offset = sizeof(npy_magic);
constexpr size_t npy_minor_offset = npy_major_offset + 1;
constexpr size_t npy_header_len_offset = npy_minor_offset + 1;
constexpr size_t npy_header_offset = npy_header_len_offset + 2;
constexpr size_t npy_preamble_size = npy_header_offset;

// Type punning
// Interpret pointer src as a pointer of type T, dereference the pointer and
// convert the value to a float.
// If swap is true the byte order will be changed in addition.
// The template avoids undefined behavior (does not break strict aliasing rule)
template <class T>
inline ifx_Float_t cast_to_float(const void* src, bool swap = false)
{
    // Copy sizeof(T) bytes to dest: std::memcpy is a legal way to do type punning
    // Note that modern compilers recognize this pattern and typically optmize
    // away the memcpy.
    T dest{};
    std::memcpy(&dest, src, sizeof(T));

    // adjust endianess if necessary
    if (swap)
        dest = swap_endian(dest);

    // convert dest to float
    return static_cast<ifx_Float_t>(dest);
}

template <class T>
inline ifx_Complex_t cast_to_complex(const void* src, bool swap = false)
{
    // Casting src to uint8_t is allowed
    const auto* real_ptr = static_cast<const uint8_t*>(src);
    const auto* imag_ptr = real_ptr + sizeof(T);

    auto real = cast_to_float<T>(real_ptr, swap);
    auto imag = cast_to_float<T>(imag_ptr, swap);
    return IFX_COMPLEX_DEF(real, imag);
}

static size_t compute_size_data(const std::vector<uint32_t>& v, size_t dtype_size)
{
    auto mul = [](size_t a, size_t b) {
        size_t ab = a * b;
        if (a != 0 && ab / a != b)
            throw NPYException(NPYErrorMessages::COMPUTING_DATA_SIZE_OVERFLOW);
        return ab;
    };

    size_t p = 1;
    for (auto e : v)
        p = mul(e, p);

    return mul(p, dtype_size);
}

static std::vector<uint32_t> get_shape(const std::string& shape)
{
    std::vector<uint32_t> v;

    const std::string delim = ",";
    size_t start = 0;
    while (true)
    {
        size_t end = shape.find(delim, start);
        std::string part = shape.substr(start, end - start);
        start = end + delim.length();

        if (part.length() == 0)
            throw NPYException(NPYErrorMessages::PARSING_SHAPE_LENGHT_ZERO);

        const auto value = std::stoull(part);
        if (value > UINT32_MAX)
            throw NPYException(NPYErrorMessages::PARSING_SHAPE_LENGHT_EXCEEDS_LIMIT);

        v.push_back(static_cast<uint32_t>(value));

        if (end == std::string::npos)
            break;
    }

    // scalar when shape is (1,)
    if((v.size() == 1) && (v[0] == 1))
    {
        v.clear();
    }

    return v;
}

static size_t npy_parse_preamble(const uint8_t data[npy_preamble_size])
{
    if (std::memcmp(&data[npy_magic_offset], npy_magic, sizeof(npy_magic)) != 0)
        throw NPYException(NPYErrorMessages::NO_MAGIC_TOKEN);

    const uint8_t major = data[npy_major_offset];
    const uint8_t minor = data[npy_minor_offset];
    if (major != 1 || minor != 0)
        throw NPYException(NPYErrorMessages::UNSUPPORTED_VERSION);

    return data[npy_header_len_offset] + (size_t(data[npy_header_len_offset + 1]) << 8);
}

static NPYInfo npy_parse_header(const std::string& header)
{
    const size_t left = header.find('{');
    const size_t right = header.rfind('}');

    if (left == std::string::npos || right == std::string::npos || left > right)
        throw NPYException(NPYErrorMessages::INVALID_FORMAT);

    NPYInfo info;
    info.offset = npy_header_offset + header.length();

    std::string dict = header.substr(left + 1, right - left);

    // parse fortran_order
    {
        std::regex re("['\"]fortran_order['\"]\\s*:\\s*(True|False)");
        std::smatch match;
        std::regex_search(dict, match, re);

        if (match.size() != 2)
            throw NPYException(NPYErrorMessages::FORTRAN_ATTRIBUTE_ERROR);

        info.fortran_order = (match.str(1) == "True");
    }

    // parse descr
    {
        std::regex re("['\"]descr['\"]\\s*:\\s*['\"]([\\|<>])(c8|c16|f4|f8|i1|i2|i4|i8|u1|u2|u4|u8)['\"]");
        std::smatch match;
        std::regex_search(dict, match, re);

        if (match.size() != 3)
            throw NPYException(NPYErrorMessages::DESCRIPTION_ATTRIBUTE_ERROR);

        info.byte_order = match.str(1)[0];
        info.dtype = match.str(2);
        info.dtype_size = std::stoi(info.dtype.c_str() + 1);
    }

    // parse shape
    {
        std::regex re("['\"]shape['\"]\\s*:\\s*\\(([0-9,\\s]*)\\)");
        std::smatch match;
        std::regex_search(dict, match, re);

        if (match.size() != 2)
            throw NPYException(NPYErrorMessages::SHAPE_ATTRIBUTE_ERROR);

        std::string shape = match.str(1);
        if (shape.empty())
        {
            // shape is empty, so we have scalar
            info.shape = {};
            info.size_data = info.dtype_size;
            info.nelems = 1;
        }
        else
        {
            // remove trailing comma if present
            if (shape.back() == ',')
                shape = shape.substr(0, shape.length() - 1);

            info.shape = get_shape(shape);
            info.size_data = compute_size_data(info.shape, info.dtype_size);
            info.nelems = info.size_data / info.dtype_size;
        }
    }

    return info;
}

NpyReader::NpyReader(const uint8_t* data, size_t size)
{
    if (size < npy_preamble_size)
        throw NPYException(NPYErrorMessages::PREAMBLE_SIZE_MISMATCH);

    const uint8_t* preamble = data;
    const uint8_t* header = data + npy_preamble_size;
    const size_t len_header = npy_parse_preamble(preamble);

    if (size < (npy_preamble_size + len_header))
        throw NPYException(NPYErrorMessages::HEADER_INCOMPLETE);

    std::string header_str(reinterpret_cast<const char*>(header), len_header);
    m_info = npy_parse_header(header_str);
    m_data = data;

    if (is_little_endian())
        m_byte_order = '<';
    else
        m_byte_order = '>';

    //m_is_complex = (m_info.dtype[0] == 'c');
    //m_dimensions = m_info.shape.size();
}

const uint8_t* NpyReader::get_element_pointer(const std::vector<size_t>& addr) const
{
    const auto& shape = m_info.shape;

    if (shape.size() != addr.size()) {
        throw NPYException(NPYErrorMessages::MISMATCH_DIMENSION_WHEN_READING);
    }

    size_t index = 0;
    if (!shape.empty())
    {
      auto index_calculator = [&](const std::function<size_t(size_t)> &idxResolver){
        // column-major order is fortran order; row-major order otherwise
        index = (m_info.fortran_order) ? addr.back() : addr[0];

        for (size_t addr_idx = 1; addr_idx < addr.size(); ++addr_idx)
        {
            const auto resolved_idx = idxResolver(addr_idx);
            index = (shape[resolved_idx] * index) + addr[resolved_idx];
        }
      };

      std::function<size_t(size_t)> idxResolver;

      if (m_info.fortran_order) {
        idxResolver = [&addr](size_t idx) { return addr.size() - idx - 1; };
      }
      else {
        idxResolver = [](size_t idx) { return idx; };
      }

      index_calculator(idxResolver);
    }

    if (index >= m_info.nelems)
        throw NPYException(NPYErrorMessages::OUT_OF_BOUNDS_WHEN_READING);

    return &m_data[m_info.offset + (index * m_info.dtype_size)];
}

ifx_Float_t NpyReader::get_real(const std::vector<size_t>& addr) const
{
    const uint8_t* start = get_element_pointer(addr);

    const bool swap = (m_info.byte_order != '|' && m_info.byte_order != m_byte_order);

    if (m_info.dtype == "i1")
        return cast_to_float<int8_t>(start);
    if (m_info.dtype == "u1")
        return cast_to_float<uint8_t>(start);

    if (m_info.dtype_size == 2)
    {
        if (m_info.dtype == "i2")
            return cast_to_float<int16_t>(start, swap);
        if (m_info.dtype == "u2")
            return cast_to_float<uint16_t>(start, swap);
    }
    else if (m_info.dtype_size == 4)
    {
        if (m_info.dtype == "i4")
            return cast_to_float<int32_t>(start, swap);
        if (m_info.dtype == "u4")
            return cast_to_float<uint32_t>(start, swap);
        if (m_info.dtype == "f4")
            return cast_to_float<float>(start, swap);
    }
    else if (m_info.dtype_size == 8)
    {
        if (m_info.dtype == "i8")
            return cast_to_float<int64_t>(start, swap);
        if (m_info.dtype == "u8")
            return cast_to_float<uint64_t>(start, swap);
        if (m_info.dtype == "f8")
            return cast_to_float<double>(start, swap);
    }

    //assert(0 && "unrecognized data type");
    //return 0.0f;

    throw NPYException(NPYErrorMessages::UNRECOGNIZED_DATA_TYPE);
}

ifx_Complex_t NpyReader::get_complex(const std::vector<size_t>& addr) const
{
    const uint8_t* start = get_element_pointer(addr);

    const bool swap = (m_info.byte_order != '|' && m_info.byte_order != m_byte_order);

    if (m_info.dtype == "c8")
        return cast_to_complex<float>(start, swap);
    else if (m_info.dtype == "c16")
        return cast_to_complex<double>(start, swap);

    //assert(0 && "unrecognized data type");
    //return IFX_COMPLEX_DEF(0,0);

    throw NPYException(NPYErrorMessages::UNRECOGNIZED_DATA_TYPE);
}

ifx_Float_t NpyReader::get_scalar_r() const
{
    if (!m_info.shape.empty())
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    return get_real({ });
}

ifx_Complex_t NpyReader::get_scalar_c() const
{
    if (!m_info.shape.empty())
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    return get_complex({ });
}

ifx_Vector_R_t* NpyReader::get_vector_r() const
{
    if (m_info.shape.size() != 1)
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    const size_t len = m_info.shape[0];

    return NpyHelpers::get_vector<ifx_Vector_R_t, ifx_Float_t>(len, std::bind(&NpyReader::get_real, this, std::placeholders::_1));
}

ifx_Vector_C_t* NpyReader::get_vector_c() const
{
    if (m_info.shape.size() != 1)
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    const size_t len = m_info.shape[0];

    return NpyHelpers::get_vector<ifx_Vector_C_t, ifx_Complex_t>(len, std::bind(&NpyReader::get_complex, this, std::placeholders::_1));
}

ifx_Matrix_R_t* NpyReader::get_matrix_r() const
{
    if (m_info.shape.size() != 2)
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    size_t rows = m_info.shape[0];
    size_t cols = m_info.shape[1];

    return NpyHelpers::get_matrix<ifx_Matrix_R_t, ifx_Float_t>(rows, cols, std::bind(&NpyReader::get_real, this, std::placeholders::_1));
}

ifx_Matrix_C_t* NpyReader::get_matrix_c() const
{
    if (m_info.shape.size() != 2)
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    size_t rows = m_info.shape[0];
    size_t cols = m_info.shape[1];

    return NpyHelpers::get_matrix<ifx_Matrix_C_t, ifx_Complex_t>(rows, cols, std::bind(&NpyReader::get_complex, this, std::placeholders::_1));
}

ifx_Cube_R_t* NpyReader::get_cube_r() const
{
    if (m_info.shape.size() != 3)
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    size_t rows = m_info.shape[0];
    size_t cols = m_info.shape[1];
    size_t slices = m_info.shape[2];

    return NpyHelpers::get_cube<ifx_Cube_R_t, ifx_Float_t>(rows, cols, slices, std::bind(&NpyReader::get_real, this, std::placeholders::_1));
}

ifx_Cube_C_t* NpyReader::get_cube_c() const
{
    if (m_info.shape.size() != 3)
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);

    size_t rows = m_info.shape[0];
    size_t cols = m_info.shape[1];
    size_t slices = m_info.shape[2];

    return NpyHelpers::get_cube<ifx_Cube_C_t, ifx_Complex_t>(rows, cols, slices, std::bind(&NpyReader::get_complex, this, std::placeholders::_1));;
}

ifx_Cube_R_t* NpyReader::get_cube_r_at(size_t frame_num, ifx_Cube_R_t* frame, float scale) const
{
    if ((m_info.shape.size() != 4) || (frame_num > m_info.shape[0])) {
        throw NPYException(NPYErrorMessages::INCOMPATIBLE_DIMENSION);
    }

    size_t rows = m_info.shape[1];
    size_t cols = m_info.shape[2];
    size_t slices = m_info.shape[3];

    if (!frame) {
        frame = ifx_cube_create_r(rows, cols, slices);
        if (!frame) {
            throw NPYException(NPYErrorMessages::MEMORY_ALLOCATION_FAILED);
        }
    }

    for (uint32_t r = 0; r < rows; ++r) {
        for (uint32_t c = 0; c < cols; ++c) {
            for(uint32_t s = 0; s < slices; ++s) {
                IFX_CUBE_AT(frame, r, c, s) = (get_real({frame_num, r, c, s}) * scale);
            }
        }
    }

    return frame;
}
