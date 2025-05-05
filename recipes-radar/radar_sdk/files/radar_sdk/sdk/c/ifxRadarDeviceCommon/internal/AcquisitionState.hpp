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

#ifndef IFX_RADAR_DEVICE_COMMON_ACQUISITION_STATE_HPP
#define IFX_RADAR_DEVICE_COMMON_ACQUISITION_STATE_HPP

#include <cstdint>
#include <vector>
#include <atomic>


enum class Acquisition_State_t : uint8_t
{
    Stopped,
    Started,
    Error,
    FifoOverflow,
    Starting,
    Stopping
};

using Atomic_Acquisition_State_t = std::atomic<Acquisition_State_t>;

namespace AcquisitionStateLogic
{
    using States = std::vector<Acquisition_State_t>;

    /**
     * \brief Updates the state thread safely.
     *
     * Updates the state with the newState value by previously checking if the current state value is equal to the expected one.
     *
     * \param [in]  state          The state to be updated.
     * \param [in]  expectedState  The exepcted state which the current state value needs to be checked with for equality.
     * \param [in]  newState       The new state value to be set in case that the current state matches the expected state.
     *
     * \return True in case that the current state matches the expected state; False otherwise.
     */
    bool update(Atomic_Acquisition_State_t& state, Acquisition_State_t expectedState, Acquisition_State_t newState);

    /**
     * \brief Updates the state thread safely.
     *
     * Updates the state with the newState value by previously checking if the current state value matches one of those passed into the
     * expectedStates sequence.
     *
     * \param [in]  state          The state to be updated.
     * \param [in]  expectedStates The states or the state sequence (white list) which the current state value needs to be checked for equality.
     * \param [in]  newState       The new state value to be set in case that the current state matches one of the expected states.
     *
     * \return True in case that the current state matches one of the expected states; False otherwise.
     */
    bool update(Atomic_Acquisition_State_t& state, States expectedStates, Acquisition_State_t newState);
}


#endif /* IFX_RADAR_DEVICE_COMMON_ACQUISITION_STATE_HPP */
