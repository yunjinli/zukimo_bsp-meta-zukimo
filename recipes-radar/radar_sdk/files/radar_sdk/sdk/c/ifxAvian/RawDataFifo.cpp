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
#include "ifxAvian/internal/RawDataFifo.hpp"

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
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

// Push the samples in vec into the buffer.
bool RawDataFifo::push(const std::vector<uint16_t>& vec)
{
    // we need the lock only while copying
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::copy(vec.begin(), vec.end(), std::back_inserter(m_buffer));
    }

    // here we should no longer hold the lock
    m_cond.notify_all();

    return true;
}

/*
    Get num_of_samples samples from the buffer and copy them to vec. Wait
    for timeout_ms of miliseconds.
    On success the function returns true.
    If not enough samples are in the buffer the function returns false.
*/
bool RawDataFifo::pop(std::vector<uint16_t>& vec, size_t num_of_samples, size_t timeout_ms)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait until either the buffer contains at least num_of_samples elements
    // or the timeout_ms timeout expired
    if (timeout_ms > 0)
    {
        auto pred = [&]() { return m_buffer.size() >= num_of_samples; };
        m_cond.wait_for(lock, std::chrono::milliseconds(timeout_ms), pred);
    }

    // check that there are enough samples in the buffer
    if (m_buffer.size() < num_of_samples)
        return false;

    // copy data to target vector and remove samples from internal buffer
    std::copy(m_buffer.begin(), m_buffer.begin() + num_of_samples, std::back_inserter(vec));
    m_buffer.erase(m_buffer.begin(), m_buffer.begin() + num_of_samples);

    return true;
}

/// Empty the buffer
void RawDataFifo::clear()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_buffer.clear();
}

/// Return the number of samples currently in the buffer
size_t RawDataFifo::size()
{
    // the lock is required here: std::vector::size is not thread-safe
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_buffer.size();
}