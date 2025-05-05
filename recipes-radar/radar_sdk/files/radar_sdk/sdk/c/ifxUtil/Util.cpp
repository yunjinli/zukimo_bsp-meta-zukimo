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
 * @file Util.c
 *
 * @brief This file implements helper methods used in unit and integration tests.
 *
 */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <cstring>
#include <climits>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <regex>
#include <vector>
#include <type_traits>
#include <cinttypes>
#include <zip.h>

#include "ifxUtil/internal/NpyReader.hpp"
#include "ifxUtil/internal/Endianess.hpp"
#include "ifxUtil/Util.h"

#include "ifxUtil/Mmap.h"

#include "ifxBase/internal/Macros.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Error.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

struct ifx_npz_s
{
    struct zip_t* zip;
};

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

const uint16_t MAGIC_STR_SIZE = 6;
const uint16_t VERSION_SIZE = 2;
const uint16_t HEADER_LEN_SIZE = 2;
const char DICTIONARY_END_CH = 0x0A;

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

namespace
{
  uint16_t calculate_padding_spaces_number(const std::string& dictionary)
  {
    auto dictionary_local = dictionary;

    if(dictionary_local[dictionary_local.size() - 1] == '\n') {
      dictionary_local.erase(dictionary_local.size() - 1);
    }

    /* Note: truncating to uint16_t doesn't cause any loss of precision
     * in the final result, since it is modulo 64 which divides 65536
     * where the overflow occurs without remainder. */
    const auto BASE = uint16_t(MAGIC_STR_SIZE + dictionary_local.size() + 1 /* new line char */ + VERSION_SIZE + HEADER_LEN_SIZE);
    const auto REM = (BASE % 64);

    return (64 - REM) ;
  }

  std::string create_padding_string(const uint16_t pads_num)
  {
    return std::string(pads_num, ' ');
  }

  void setNpyError(const NPYException& e)
  {
    if(! e.what()) {
      return;
    }

    ifx_error_set(IFX_ERROR_FILE_INVALID);
  }
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_npz_t* ifxu_npz_open(const char* filename)
{
    //return zip_open(filename, ZIP_RDONLY, nullptr);
    //mz_zip_open(void* handle, void* stream, MZ_OPEN_MODE_READ);

    auto *archive = static_cast<ifx_npz_t*>(ifx_mem_alloc(sizeof(ifx_npz_t)));
    IFX_ERR_BRN_MEMALLOC(archive);
    archive->zip = zip_open(filename, 0, 'r');
    if (!archive->zip)
        return nullptr;

    return archive;        
}

void ifxu_npz_close(ifx_npz_t* archive)
{
    if (!archive)
        return;

    if(archive->zip)
        zip_close(archive->zip);

    ifx_mem_free(archive);
}

size_t ifxu_npz_num_entries(ifx_npz_t* archive)
{
    return zip_entries_total(archive->zip);
}

const char* npz_get_name(ifx_npz_t* archive, size_t index)
{
    zip_entry_openbyindex(archive->zip, int(index));
    const char* name = zip_entry_name(archive->zip);
    return name;
}

void* ifxu_npz_read(ifx_npz_t* archive, const char* name, int* type)
{
    void* p = nullptr;
    *type = IFX_TYPE_INVALID;

    if (zip_entry_open(archive->zip, name) != 0)
        return nullptr;

    void* buffer = nullptr;
    size_t bufsize;
    zip_entry_read(archive->zip, &buffer, &bufsize);
    zip_entry_close(archive->zip);

    try {
      NpyReader npy_reader(static_cast<uint8_t*>(buffer), bufsize);

      NPYInfo info = npy_reader.get_info();

      if (info.dtype[0] != 'c')
      {
          switch (info.shape.size())
          {
          case 0:
              p = ifx_mem_alloc(sizeof(ifx_Float_t));
              if (p)
                  *static_cast<ifx_Float_t*>(p) = npy_reader.get_scalar_r();
              *type = IFX_TYPE_SCALAR_REAL;
              break;
          case 1: p = npy_reader.get_vector_r(); *type = IFX_TYPE_VECTOR_REAL; break;
          case 2: p = npy_reader.get_matrix_r(); *type = IFX_TYPE_MATRIX_REAL; break;
          case 3: p = npy_reader.get_cube_r(); *type = IFX_TYPE_CUBE_REAL; break;
          }
      }
      else
      {
          switch (info.shape.size())
          {
          case 0:
              p = ifx_mem_alloc(sizeof(ifx_Complex_t));
              if (p)
                  *static_cast<ifx_Complex_t*>(p) = npy_reader.get_scalar_c();
              *type = IFX_TYPE_SCALAR_COMPLEX;
              break;
          case 1: p = npy_reader.get_vector_c(); *type = IFX_TYPE_VECTOR_COMPLEX; break;
          case 2: p = npy_reader.get_matrix_c(); *type = IFX_TYPE_MATRIX_COMPLEX; break;
          case 3: p = npy_reader.get_cube_c(); *type = IFX_TYPE_CUBE_COMPLEX; break;
          }
      }
    } catch (const NPYException& e) {
      //std::error << e.what();
      ::setNpyError(e);
    }

    free(buffer);

    return p;
}

void* ifxu_npy_read(const char* filename, int* type)
{
  IFX_ERR_BRV_NULL(filename, nullptr);
  IFX_ERR_BRV_NULL(type, nullptr);

  *type = IFX_TYPE_INVALID;
  void* pdata = nullptr;

  size_t mmap_lenght = 0;
  auto * mmap_handle = ifx_mmap_create(filename, &mmap_lenght);

  if((mmap_handle == nullptr) || !mmap_lenght) {
    return pdata;
  }

  try {
    NpyReader npy_reader(ifx_mmap_const_data(mmap_handle), mmap_lenght);

    auto info = npy_reader.get_info();

    // real, not a complex
    if (info.dtype[0] != 'c')
    {
      switch (info.shape.size())
      {
      case 0:
        pdata = ifx_mem_alloc(sizeof(ifx_Float_t));
        if(pdata)
        {
          *(static_cast<ifx_Float_t*>(pdata)) = npy_reader.get_scalar_r();
          *type = IFX_TYPE_SCALAR_REAL;
        }
        break;
      case 1: pdata = npy_reader.get_vector_r(); *type = IFX_TYPE_VECTOR_REAL; break;
      case 2: pdata = npy_reader.get_matrix_r(); *type = IFX_TYPE_MATRIX_REAL; break;
      case 3: pdata = npy_reader.get_cube_r(); *type = IFX_TYPE_CUBE_REAL; break;
      }
    }
    else
    {
      switch (info.shape.size())
      {
      case 0:
        pdata = ifx_mem_alloc(sizeof(ifx_Complex_t));
        if(pdata)
        {
          *(static_cast<ifx_Complex_t*>(pdata)) = npy_reader.get_scalar_c();
          *type = IFX_TYPE_SCALAR_COMPLEX;
        }
        break;
      case 1: pdata = npy_reader.get_vector_c(); *type = IFX_TYPE_VECTOR_COMPLEX; break;
      case 2: pdata = npy_reader.get_matrix_c(); *type = IFX_TYPE_MATRIX_COMPLEX; break;
      case 3: pdata = npy_reader.get_cube_c(); *type = IFX_TYPE_CUBE_COMPLEX; break;
      }
    }
  } catch(const NPYException& e) {
    //std::error << e.what();
    ::setNpyError(e);
  }

  ifx_mmap_destroy(mmap_handle);

  return pdata;
}

bool ifxu_npy_write(const char* filename, int type, const void *object)
{
    IFX_ERR_BRV_NULL(filename, false);
    IFX_ERR_BRV_NULL(object, false);

    const void *data = nullptr;
    size_t nelems = 0;
    size_t size_element = 0;

#if defined(__STDC_LIB_EXT1__) || defined(_MSC_VER)
    FILE* stream = nullptr;
    if(0 != fopen_s(&stream, filename, "wb"))
        return false;
#else
    FILE* stream = fopen(filename, "wb");
#endif
    if(stream == nullptr)
        return false;

    /* We don't want to convert data here, so we write out the information in
     * the endianess of this machine.
     * '>' means big    endianess
     * '<' means little endianess
     */
    const char endianess = is_little_endian() ? '<' : '>';

    #if !defined(USE_DOUBLE)
        const char* format_real = "f4";
        const char* format_complex = "c8";
    #else
        const char* format_real = "f8";
        const char* format_complex = "c16";
    #endif

    /* write length of header and header */
    char dictionary[512] = { 0 };
    if(type == IFX_TYPE_SCALAR_REAL)
    {
        data = object;
        nelems = 1;
        size_element = sizeof(ifx_Float_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': True, 'shape': (1,), }", endianess, format_real);
    }
    else if(type == IFX_TYPE_SCALAR_COMPLEX)
    {
        data = object;
        nelems = 1;
        size_element = sizeof(ifx_Complex_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': True, 'shape': (1,), }", endianess, format_complex);
    }
    else if(type == IFX_TYPE_VECTOR_REAL)
    {
        auto *vector = static_cast<ifx_Vector_R_t*>(const_cast<void*>(object));
        data = vDat(vector);
        uint64_t len = vLen(vector);
        nelems = len;
        size_element = sizeof(ifx_Float_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': True, 'shape': (%" PRIu64",), }", endianess, format_real, len);
    }
    else if(type == IFX_TYPE_VECTOR_COMPLEX)
    {
        auto *vector = static_cast<ifx_Vector_C_t*>(const_cast<void*>(object));
        data = vDat(vector);
        uint64_t len = vLen(vector);
        nelems = len;
        size_element = sizeof(ifx_Complex_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': True, 'shape': (%" PRIu64",), }", endianess, format_complex, len);
    }
    else if(type == IFX_TYPE_MATRIX_REAL)
    {
        auto *matrix = static_cast<ifx_Matrix_R_t*>(const_cast<void*>(object));
        data = mDat(matrix);
        uint64_t rows = mRows(matrix);
        uint64_t cols = mCols(matrix);
        nelems = rows*cols;
        size_element = sizeof(ifx_Float_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': False, 'shape': (%" PRIu64",%" PRIu64"), }", endianess, format_real, rows, cols);
    }
    else if(type == IFX_TYPE_MATRIX_COMPLEX)
    {
        auto *matrix = static_cast<ifx_Matrix_C_t*>(const_cast<void*>(object));
        data = mDat(matrix);
        uint64_t rows = mRows(matrix);
        uint64_t cols = mCols(matrix);
        nelems = rows*cols;
        size_element = sizeof(ifx_Complex_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': False, 'shape': (%" PRIu64",%" PRIu64"), }", endianess, format_complex, rows, cols);
    }
    else if(type == IFX_TYPE_CUBE_REAL)
    {
        auto *cube = static_cast<ifx_Cube_R_t*>(const_cast<void*>(object));

        ifx_Cube_R_t* c = ifx_cube_create_r(cCols(cube), cRows(cube), cSlices(cube));
        for(uint32_t s = 0; s < cSlices(cube); ++s)
        {
            ifx_Matrix_R_t mat;
            ifx_Matrix_R_t m;
            ifx_cube_get_slice_r(cube, s, &mat);
            ifx_cube_get_slice_r(c, s, &m);
            ifx_mat_transpose_r(&mat, &m);
        }
        void* d = cDat(c);
        cDat(c) = cDat(cube);
        cDat(cube) = static_cast<ifx_Float_t*>(d);
        ifx_cube_destroy_r(c);

        data = cDat(cube);
        uint64_t slices = cSlices(cube);
        uint64_t rows = cRows(cube);
        uint64_t cols = cCols(cube);
        nelems = slices*rows*cols;
        size_element = sizeof(ifx_Float_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': True, 'shape': (%" PRIu64",%" PRIu64",%" PRIu64"), }", endianess, format_real, rows, cols, slices);
    }
    else if(type == IFX_TYPE_CUBE_COMPLEX)
    {
        auto *cube = static_cast<ifx_Cube_C_t*>(const_cast<void*>(object));

        ifx_Cube_C_t* c = ifx_cube_create_c(cCols(cube), cRows(cube), cSlices(cube));
        for(uint32_t s = 0; s < cSlices(cube); ++s)
        {
            ifx_Matrix_C_t mat;
            ifx_Matrix_C_t m;
            ifx_cube_get_slice_c(cube, s, &mat);
            ifx_cube_get_slice_c(c, s, &m);
            ifx_mat_transpose_c(&mat, &m);
        }
        void* d = cDat(c);
        cDat(c) = cDat(cube);
        cDat(cube) = static_cast<ifx_Complex_t*>(d);
        ifx_cube_destroy_c(c);

        data = cDat(cube);
        uint64_t slices = cSlices(cube);
        uint64_t rows = cRows(cube);
        uint64_t cols = cCols(cube);
        nelems = slices*rows*cols;
        size_element = sizeof(ifx_Complex_t);
        snprintf(dictionary, sizeof(dictionary)/sizeof(dictionary[0]), "{'descr': '%c%s', 'fortran_order': True, 'shape': (%" PRIu64",%" PRIu64",%" PRIu64"), }", endianess, format_complex, rows, cols, slices);
	}
    else
        return false;

    const auto padding_spaces_num = ::calculate_padding_spaces_number(dictionary);

    auto d_len = static_cast<uint16_t>(strlen(dictionary) + 1 /* new line */ + padding_spaces_num);
    if(is_big_endian())
    {
        /* if host is big endian, convert len to little endian */
        d_len = ::swap_endian(d_len);
    }

    /* write magic string, major number and minor number */
    fwrite("\x93NUMPY\x01\x00", sizeof(char), 8, stream);

    /* write out the length of the python dict string */
    fwrite(&d_len, sizeof(d_len), 1, stream);

    /* write out the dict string itself */
    fwrite(dictionary, sizeof(char), strlen(dictionary), stream);

    /* write out padding spaces */
    const auto pads = ::create_padding_string(padding_spaces_num);
    fwrite(pads.data(), sizeof(char), pads.size(), stream);

    /* write the ending new line character */
    fwrite(&DICTIONARY_END_CH, sizeof(DICTIONARY_END_CH), 1, stream);

    /* write serialized data */
    fwrite(data, size_element, nelems, stream);

    /* close stream */
    fclose(stream);

    return true;
}

// NOLINTNEXTLINE(google-runtime-int)
bool ifxu_npy_write_raw(const char* filename, uint16_t* data, bool is_complex, bool fortran_order, uint64_t shape[], int num_shape)
{
#if defined(__STDC_LIB_EXT1__) || defined(_MSC_VER)
    FILE* stream = nullptr;
    if (0 != fopen_s(&stream, filename, "wb"))
        return false;
#else
    FILE* stream = fopen(filename, "wb");
#endif
    if (stream == nullptr)
        return false;

    /* We don't want to convert data here, so we write out the information in
     * the endianess of this machine.
     * '>' means big    endianess
     * '<' means little endianess
     */
    const char endianess = is_little_endian() ? '<' : '>';

    /* this specifies the header values of npy for uint16 */
	const char* format_real = "u2";
	const char* format_complex = "u4";

    /* write length of header and header */
    const int SIZE = 512;
    char dictionary[SIZE] = { 0 };

	size_t size_element = is_complex ? sizeof(uint16_t)*2 : sizeof(uint16_t);

    /* Construct header of shapes */
	std::string shape_s = "(";
    for (int i = 0; i < num_shape; i++) {
        shape_s += std::to_string(shape[i]) + ",";
    }
	shape_s += ")";

    snprintf(dictionary, SIZE, "{'descr': '%c%s', 'fortran_order': %s, 'shape': %s, }", endianess, (is_complex ? format_complex : format_real), (fortran_order ? "True" : "False"), shape_s.c_str());

	const auto padding_spaces_num = ::calculate_padding_spaces_number(dictionary);

	auto d_len = static_cast<uint16_t>(strlen(dictionary) + 1 /* new line */ + padding_spaces_num);
	if (is_big_endian())
	{
		/* if host is big endian, convert len to little endian */
		d_len = ::swap_endian(d_len);
	}

	/* write magic string, major number and minor number */
	fwrite("\x93NUMPY\x01\x00", sizeof(char), 8, stream);

	/* write out the length of the python dict string */
	fwrite(&d_len, sizeof(d_len), 1, stream);

	/* write out the dict string itself */
	fwrite(dictionary, sizeof(char), strlen(dictionary), stream);

	/* write out padding spaces */
	const auto pads = ::create_padding_string(padding_spaces_num);
	fwrite(pads.data(), sizeof(char), pads.size(), stream);

	/* write the ending new line character */
	fwrite(&DICTIONARY_END_CH, sizeof(DICTIONARY_END_CH), 1, stream);

	/* write serialized data */
	size_t elements = shape[0];
	for (int i = 1; i < num_shape; i++) {
		elements *= shape[i];
	}
	fwrite(data, size_element, elements, stream);

	/* close stream */
	fclose(stream);

	return true;
}

void ifxu_npy_free(void* p, int type)
{
    if(p == nullptr)
        return;

    switch(type)
    {
        case IFX_TYPE_SCALAR_REAL:
        case IFX_TYPE_SCALAR_COMPLEX:
            ifx_mem_free(p);
            return;

        case IFX_TYPE_VECTOR_REAL:
            ifx_vec_destroy_r(static_cast<ifx_Vector_R_t*>(p));
            return;

        case IFX_TYPE_VECTOR_COMPLEX:
            ifx_vec_destroy_c(static_cast<ifx_Vector_C_t*>(p));
            return;

        case IFX_TYPE_MATRIX_REAL:
            ifx_mat_destroy_r(static_cast<ifx_Matrix_R_t*>(p));
            return;

        case IFX_TYPE_MATRIX_COMPLEX:
            ifx_mat_destroy_c(static_cast<ifx_Matrix_C_t*>(p));
            return;

        case IFX_TYPE_CUBE_REAL:
            ifx_cube_destroy_r(static_cast<ifx_Cube_R_t*>(p));
            return;

        case IFX_TYPE_CUBE_COMPLEX:
            ifx_cube_destroy_c(static_cast<ifx_Cube_C_t*>(p));
            return;
    }
}

template <typename T>
static constexpr int type_from_type(T*)
{
    if(std::is_same<T,ifx_Float_t>::value)
        return IFX_TYPE_SCALAR_REAL;
    if (std::is_same<T, ifx_Complex_t>::value)
        return IFX_TYPE_SCALAR_COMPLEX;
    if (std::is_same<T, ifx_Vector_R_t>::value)
        return IFX_TYPE_VECTOR_REAL;
    if (std::is_same<T, ifx_Vector_C_t>::value)
        return IFX_TYPE_VECTOR_COMPLEX;
    if (std::is_same<T, ifx_Matrix_R_t>::value)
        return IFX_TYPE_MATRIX_REAL;
    if (std::is_same<T, ifx_Matrix_C_t>::value)
        return IFX_TYPE_MATRIX_COMPLEX;
    if (std::is_same<T, ifx_Cube_R_t>::value)
        return IFX_TYPE_CUBE_REAL;
    if (std::is_same<T, ifx_Cube_C_t>::value)
        return IFX_TYPE_CUBE_COMPLEX;

    return IFX_TYPE_INVALID;
}

template <typename T>
T* npy_read(const char* filename)
{
    int type;
    auto * v = static_cast<T*>(ifxu_npy_read(filename, &type));
    if (type != type_from_type(v))
    {
        /* wrong format */
        ifxu_npy_free(v, type);
        return nullptr;
    }
    
    return v;
}

ifx_Float_t* ifxu_npy_read_scalar_r(const char* filename) { return npy_read<ifx_Float_t>(filename); }
ifx_Complex_t* ifxu_npy_read_scalar_c(const char* filename) { return npy_read<ifx_Complex_t>(filename); }

ifx_Vector_R_t* ifxu_npy_read_vec_r(const char* filename) { return npy_read<ifx_Vector_R_t>(filename);  }
ifx_Vector_C_t* ifxu_npy_read_vec_c(const char* filename) { return npy_read<ifx_Vector_C_t>(filename); }

ifx_Matrix_R_t* ifxu_npy_read_mat_r(const char* filename) { return npy_read<ifx_Matrix_R_t>(filename); }
ifx_Matrix_C_t* ifxu_npy_read_mat_c(const char* filename) { return npy_read<ifx_Matrix_C_t>(filename); }

ifx_Cube_R_t* ifxu_npy_read_cube_r(const char* filename) { return npy_read<ifx_Cube_R_t>(filename); }
ifx_Cube_C_t* ifxu_npy_read_cube_c(const char* filename) { return npy_read<ifx_Cube_C_t>(filename); }

bool ifxu_npy_write_scalar_r(const char* filename, ifx_Float_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_SCALAR_REAL, scalar); }
bool ifxu_npy_write_scalar_c(const char* filename, ifx_Complex_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_SCALAR_COMPLEX, scalar); }

bool ifxu_npy_write_vec_r(const char* filename, ifx_Vector_R_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_VECTOR_REAL, scalar); }
bool ifxu_npy_write_vec_c(const char* filename, ifx_Vector_C_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_VECTOR_COMPLEX, scalar); }

bool ifxu_npy_write_mat_r(const char* filename, ifx_Matrix_R_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_MATRIX_REAL, scalar); }
bool ifxu_npy_write_mat_c(const char* filename, ifx_Matrix_C_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_MATRIX_COMPLEX, scalar); }

bool ifxu_npy_write_cube_r(const char* filename, ifx_Cube_R_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_CUBE_REAL, scalar); }
bool ifxu_npy_write_cube_c(const char* filename, ifx_Cube_C_t* scalar) { return ifxu_npy_write(filename, IFX_TYPE_CUBE_COMPLEX, scalar); }

template <typename T>
T* npz_read(ifx_npz_t* archive, const char* name)
{
    int type;
    auto * v = static_cast<T*>(ifxu_npz_read(archive, name, &type));
    if (type != type_from_type(v))
    {
        /* wrong format */
        ifxu_npy_free(v, type);
        return nullptr;
    }
    return v;
}

ifx_Float_t* ifxu_npz_read_scalar_r(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Float_t>(archive, name); }
ifx_Complex_t* ifxu_npz_read_scalar_c(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Complex_t>(archive, name); }

ifx_Vector_R_t* ifxu_npz_read_vec_r(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Vector_R_t>(archive, name); }
ifx_Vector_C_t* ifxu_npz_read_vec_c(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Vector_C_t>(archive, name); }

ifx_Matrix_R_t* ifxu_npz_read_mat_r(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Matrix_R_t>(archive, name); }
ifx_Matrix_C_t* ifxu_npz_read_mat_c(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Matrix_C_t>(archive, name); }

ifx_Cube_R_t* ifxu_npz_read_cube_r(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Cube_R_t>(archive, name); }
ifx_Cube_C_t* ifxu_npz_read_cube_c(ifx_npz_t* archive, const char* name) { return npz_read<ifx_Cube_C_t>(archive, name); }
