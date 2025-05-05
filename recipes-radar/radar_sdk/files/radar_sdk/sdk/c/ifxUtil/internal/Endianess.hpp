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

#ifndef IFX_ENDIANESS_HPP
#define IFX_ENDIANESS_HPP

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <cstdint>

// Check if host little endian
inline bool is_little_endian()
{
#ifdef SDK_TARGET_PLATFORM_BIG_ENDIAN
    return false;
#else
    return true;
#endif
}

// Check if host is big endian
inline bool is_big_endian()
{
    return !is_little_endian();
}

inline uint8_t byte_swap(const uint8_t* x) { return *x; }
inline int8_t byte_swap(const int8_t* x) { return *x; }

/// Change byte order of uint16_t
inline uint16_t byte_swap(const uint16_t* x)
{
    return  ((*x >> 8) & 0xFF) | ((*x << 8) & 0xFF00);
}

inline int16_t byte_swap(const int16_t* x)
{
    return byte_swap(reinterpret_cast<const uint16_t*>(x));
}

/// Change byte order of uint32_t
inline uint32_t byte_swap(const uint32_t* x)
{
    uint32_t y;
    y = ((*x << 8) & 0xFF00FF00 ) | ((*x >> 8) & 0xFF00FF);
    y = (*x << 16) | (*x >> 16);
    return y;
}

inline int32_t byte_swap(const int32_t* x)
{
    return byte_swap(reinterpret_cast<const uint32_t*>(x));
}

/// Change byte order of uint64_t
inline uint64_t byte_swap(const uint64_t* x)
{
    uint64_t y;
    y = ((*x << 8) & 0xFF00FF00FF00FF00ULL) | ((*x >> 8) & 0x00FF00FF00FF00FFUL);
    y = ((*x << 16) & 0xFFFF0000FFFF0000UL) | ((*x >> 16) & 0x0000FFFF0000FFFFUL);
    y = (*x << 32) | (*x >> 32);
    return y;
}

inline int64_t byte_swap(const int64_t* x)
{
    return byte_swap(reinterpret_cast<const uint64_t*>(x));
}

/// Changes the byte order for any T type - function does not perform any static assertion to check if T is a class.
template <typename T>
T swap_endian(T val)
{
  union
  {
    T val;
    unsigned char val_seq[sizeof(T)];
  } source, dest;

  source.val = val;

  for(size_t i = 0; i < sizeof(T); ++i) {
    dest.val_seq[i] = source.val_seq[sizeof(T) - i - 1];
  }

  return dest.val;
}

#endif // IFX_ENDIANESS_HPP
