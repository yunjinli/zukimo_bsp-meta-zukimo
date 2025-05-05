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

/**
 * @file LTR11RegisterManipulator.hpp
 *
 * @brief Header for LTR11RegisterManipulator
 */

#ifndef IFX_DOPPLER_LTR11_REGISTER_MANIPULATOR_LTR11_HPP
#define IFX_DOPPLER_LTR11_REGISTER_MANIPULATOR_LTR11_HPP

#include <components/interfaces/IRegisters.hpp>
#include "ifxBase/internal/NonCopyable.hpp"
#include "ifxDopplerLTR11/Types.h"
#include <cstdint>
#include <tuple>
#include <vector>


class LTR11RegisterManipulator
{
    NONCOPYABLE(LTR11RegisterManipulator);

public:
    using address_t = uint8_t;
    using value_t = uint16_t;
    using BatchType = typename IMemory<address_t, value_t>::BatchType;



    LTR11RegisterManipulator(IRegisters<address_t, value_t>* m_registers);

    void addSetPulseConfig(ifx_LTR11_PRT_t prt, ifx_LTR11_PulseWidth_t pulseWidth, ifx_LTR11_TxPowerLevel_t powerLevel);
    ifx_LTR11_PulseWidth_t getPulseWidth() const;
    ifx_LTR11_PRT_t getPulseRepetitionTime() const;

    void addSetHoldTime(ifx_LTR11_Hold_Time_t holdTime);
    ifx_LTR11_Hold_Time_t getHoldTime() const;

    value_t getInternalDetectorThreshold() const;

    ifx_LTR11_TxPowerLevel_t getTxPowerLevel() const;

    void addSetRX_IF_Gain(ifx_LTR11_RxIFGain_t rxIfGain);
    ifx_LTR11_RxIFGain_t getRX_IF_Gain() const;

    void addSetRFCenterFrequency(uint64_t freq_Hz);
    uint64_t getRFCenterFrequency() const;

    void addSetDetectorThreshold_AdaptivPRT(value_t internalDetectorThreshold, ifx_LTR11_APRT_Factor_t aprtFactor);
    ifx_LTR11_APRT_Factor_t getAdaptivePulseRepetitionTime() const;

    void addSetPulseMode(bool enabled);

    void disableInternalDetector();

    bool isJapanBand() const;

    void addSetDefaultRegistersList();

    void dumpRegisters(const char *filename) const;

    template<std::size_t SIZE>
    void addRegisters(
        const std::array<BatchType, SIZE>& regs);
    void addRegisters(
        BatchType const * array, size_t size);

    void addSetRegister(const BatchType& reg);

    void flushEnqRegisters();

    const std::vector<BatchType>& getQueue()
    {
        return m_registerQueue;
    };

private:
    void resetRegisters();

    IRegisters<address_t, value_t> *m_registers;

    static value_t convertRFCFreq_toRegisterValue(uint64_t freq_Hz);
    static uint64_t convertToRfCFreq(uint16_t freq_reg);

    friend uint16_t testGetRegRFFreq(uint64_t freq_Hz);

    BatchType getRegisterInternal(address_t addr) const;

    std::vector<BatchType> m_registerQueue;
};

template<std::size_t SIZE>
void LTR11RegisterManipulator::addRegisters(
        const std::array<BatchType, SIZE>& regs){
    addRegisters(regs.data(), SIZE);
}

#endif  // IFX_DOPPLER_LTR11_REGISTER_MANIPULATOR_LTR11_HPP
