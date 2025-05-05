/* ===========================================================================
** Copyright (C) 2022 Infineon Technologies AG
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

#include "ifxBase/Error.h"
#include "ifxUtil/Mmap.h"
#include <fstream>
#include <vector>
#include <mio.hpp>


namespace
{
    bool isFile(const char* filename)
    {
        std::ifstream file(filename);
        return file.good();
    }
}

ifx_MMAP_t* ifx_mmap_create(const char* filename, size_t* length)
{
    IFX_ERR_BRV_NULL(filename, NULL);
    IFX_ERR_BRV_NULL(length, NULL);

    if(! ::isFile(filename)) {
        ifx_error_set(IFX_ERROR_OPENING_FILE);
        return nullptr;
    }

    mio::mmap_source* ro_mmap = nullptr;

    try {
        ro_mmap = new mio::mmap_source();

        std::error_code error;
        ro_mmap->map(filename, error);
        if (error)
            throw std::runtime_error("Cannot create mapping");
    } catch(const std::exception&) {
        delete ro_mmap;
        ifx_error_set(IFX_ERROR);
        return nullptr;
    }

    *length = ro_mmap->length();

    return static_cast<ifx_MMAP_t*>(ro_mmap);
}

void ifx_mmap_destroy(ifx_MMAP_t* handle)
{
    if (!handle)
        return;

    auto* ro_mmap = static_cast<mio::mmap_source*>(handle);
    delete ro_mmap;
}

const uint8_t* ifx_mmap_const_data(ifx_MMAP_t* handle, uint32_t offset /*= 0*/)
{
    IFX_ERR_BRV_NULL(handle, NULL);

    auto* ro_mmap = static_cast<mio::mmap_source*>(handle);
    return ((ro_mmap->is_mapped() && (offset < ro_mmap->length())) ?
            reinterpret_cast<const uint8_t*>(ro_mmap->data() + offset) : nullptr);
}