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

#include "ifxBase/Version.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/
#define XSTR(x) STR(x)
#define STR(x)  #x

#ifdef rdk_VERSION_MAJOR 
#define IFX_SDK_VERSION_MAJOR rdk_VERSION_MAJOR
#define IFX_SDK_VERSION_MINOR rdk_VERSION_MINOR
#define IFX_SDK_VERSION_PATCH rdk_VERSION_PATCH
#define IFX_SDK_VERSION_STR       XSTR(IFX_SDK_VERSION_MAJOR.IFX_SDK_VERSION_MINOR.IFX_SDK_VERSION_PATCH)
#define IFX_SDK_VERSION_STR_FULL  XSTR(rdk_VERSION_STRING_FULL)
#define IFX_SDK_VERSION_HASH  XSTR(rdk_VERSION_GIT_SHA)
#else
// TODO: The warning message is for transition phase only
// and should be replaced with an error in long term.
#define NO_VER_MSG "no version definition"
#ifdef _MSC_VER
#pragma message("Warning: "NO_VER_MSG)
#elif  __GNUC__
#warning NO_VER_MSG
#endif

#define IFX_SDK_VERSION_STR               "0.0.0"
#define IFX_SDK_VERSION_STR_FULL          "v0.0.0-unknown+0.000000000"
#define IFX_SDK_VERSION_HASH              "000000000"
#endif

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

const char* ifx_sdk_get_version_string(void)
{
    return IFX_SDK_VERSION_STR;
}

const char* ifx_sdk_get_version_string_full(void)
{
    return IFX_SDK_VERSION_STR_FULL;
}

const char* ifx_sdk_get_version_hash(void)
{
    return IFX_SDK_VERSION_HASH;
}