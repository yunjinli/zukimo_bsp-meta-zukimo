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

#include "LTR11RegisterManipulator.hpp"
#include "ifxBase/Exception.hpp"

#include <components/interfaces/IRadarLtr11.hpp>
#include <components/interfaces/IRegisters.hpp>
#include "registers_LTR11.hpp"

#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>

namespace
{
    const std::vector<LTR11RegisterManipulator::BatchType> defaultRegistersList = {{
        {DAR_REG0_REG_ADDR, 0x0000},
        {DAR_REG1_REG_ADDR, 0x0000},
        {PLL_CONFIG1_REG4_REG_ADDR, 0x0F3A},
        {PLL_CONFIG3_REG6_REG_ADDR, 0x6800},
        {DC_TMG_PD_MPA_REG7_REG_ADDR, 0x0457},
        {DIV_REG8_REG_ADDR, 0x0000},
        {BB_REG9_REG_ADDR, 0x0068},
        {ALGO2_REG14_REG_ADDR, 0x4000},
    }};

    /* coefficient of the line equation: y = m *x + n */
    const float m = 4.16667E-07f;
    const float n = -21504.0F;

}  // end of anonymous namespace

LTR11RegisterManipulator::LTR11RegisterManipulator(IRegisters<address_t, value_t> *registers) :
 m_registers(registers)
{

}

void LTR11RegisterManipulator::addSetDefaultRegistersList()
{
    m_registerQueue.insert(m_registerQueue.end(), defaultRegistersList.begin(), defaultRegistersList.end());
}

void LTR11RegisterManipulator::addSetPulseConfig(const ifx_LTR11_PRT_t prt, const ifx_LTR11_PulseWidth_t pulseWidth, const ifx_LTR11_TxPowerLevel_t powerLevel)
{
    value_t value = DC_TMG_PD_MPA_REG7_REG_RST;

    value &= ~dc_on_pulse_len_MASK;
    value |= (pulseWidth << dc_on_pulse_len_BITS);
    value &= ~dc_rep_rate_MASK;
    value |= (prt << dc_rep_rate_BITS);
    value &=  ~mpa_ctrl_MASK;
    value |=  powerLevel;

    addSetRegister({DC_TMG_PD_MPA_REG7_REG_ADDR, value});
}

ifx_LTR11_PulseWidth_t LTR11RegisterManipulator::getPulseWidth() const
{
    const auto regAddrVal = getRegisterInternal(DC_TMG_PD_MPA_REG7_REG_ADDR);
    auto pulseWidth = ((regAddrVal.value & dc_on_pulse_len_MASK) >> dc_on_pulse_len_BITS);
    return static_cast<ifx_LTR11_PulseWidth_t>(pulseWidth);
}

ifx_LTR11_PRT_t LTR11RegisterManipulator::getPulseRepetitionTime() const
{
    const auto regAddVal = getRegisterInternal(DC_TMG_PD_MPA_REG7_REG_ADDR);
    auto prt = ((regAddVal.value & dc_rep_rate_MASK) >> dc_rep_rate_BITS);
    return static_cast<ifx_LTR11_PRT_t>(prt);
}

void LTR11RegisterManipulator::addSetHoldTime(const ifx_LTR11_Hold_Time_t holdTime)
{
    BatchType regHoldTime = {HT_REG10_REG_ADDR, static_cast<value_t>(holdTime)};
    addSetRegister(regHoldTime);
}

ifx_LTR11_Hold_Time_t LTR11RegisterManipulator::getHoldTime() const
{
    return static_cast<ifx_LTR11_Hold_Time_t>(getRegisterInternal(HT_REG10_REG_ADDR).value);
}

void LTR11RegisterManipulator::addSetDetectorThreshold_AdaptivPRT(const value_t internalDetectorThreshold, const ifx_LTR11_APRT_Factor_t aprtFactor)
{
    const auto aptr_enabled = (aprtFactor != IFX_LTR11_APRT_FACTOR_1);

    value_t reg2Value = THOLD_REG2_REG_RST;

    reg2Value &= ~thres_MASK;
    reg2Value |= (internalDetectorThreshold & thres_MASK);
    reg2Value &=  ~aprt_MASK;
    reg2Value |=  static_cast<int>(aptr_enabled) << aprt_BITS;

    if (aptr_enabled)
    {
        value_t reg13Value = ALGO1_REG13_REG_RST;
        reg13Value &=  ~prt_mult_MASK;
        reg13Value |=  aprtFactor;
        addSetRegister({ALGO1_REG13_REG_ADDR, reg13Value});
    }
    addSetRegister({THOLD_REG2_REG_ADDR, reg2Value});
}

LTR11RegisterManipulator::value_t LTR11RegisterManipulator::getInternalDetectorThreshold() const
{
    const auto internalDetectorThreshold = (getRegisterInternal(THOLD_REG2_REG_ADDR).value & thres_MASK);
    return internalDetectorThreshold;
}

ifx_LTR11_TxPowerLevel_t LTR11RegisterManipulator::getTxPowerLevel() const
{
    const auto powerLevel = (getRegisterInternal(DC_TMG_PD_MPA_REG7_REG_ADDR).value & mpa_ctrl_MASK);
    return static_cast<ifx_LTR11_TxPowerLevel_t>(powerLevel);
}

void LTR11RegisterManipulator::addSetRX_IF_Gain(const ifx_LTR11_RxIFGain_t rxIfGain)
{
    value_t value = BB_REG9_REG_RST;
    value &=  ~bb_ctrl_gain_MASK;
    value |=  rxIfGain;

    addSetRegister({BB_REG9_REG_ADDR, value});
}

ifx_LTR11_RxIFGain_t LTR11RegisterManipulator::getRX_IF_Gain() const
{
    const auto rx_if_gain = (getRegisterInternal(BB_REG9_REG_ADDR).value & bb_ctrl_gain_MASK);

    return static_cast<ifx_LTR11_RxIFGain_t>(rx_if_gain);
}

void LTR11RegisterManipulator::addSetRFCenterFrequency(const uint64_t freq_Hz)
{
    auto freq_reg = convertRFCFreq_toRegisterValue(freq_Hz);

    value_t value = PLL_CONFIG2_REG5_REG_RST;

    value &=  ~pll_fcw_MASK;
    value |=  freq_reg;

    addSetRegister({PLL_CONFIG2_REG5_REG_ADDR, value});
}

uint64_t LTR11RegisterManipulator::getRFCenterFrequency() const
{
    const auto freq_reg = (getRegisterInternal(PLL_CONFIG2_REG5_REG_ADDR).value & pll_fcw_MASK);
    return convertToRfCFreq(freq_reg);
}

ifx_LTR11_APRT_Factor_t LTR11RegisterManipulator::getAdaptivePulseRepetitionTime() const
{
    const auto aprt_enabled = (getRegisterInternal(2).value & aprt_MASK) >> aprt_BITS;

    if (!aprt_enabled)
    {
        return IFX_LTR11_APRT_FACTOR_1;
    }

    return static_cast<ifx_LTR11_APRT_Factor_t>(getRegisterInternal(ALGO1_REG13_REG_ADDR).value & prt_mult_MASK);
}


LTR11RegisterManipulator::BatchType LTR11RegisterManipulator::getRegisterInternal(address_t addr) const
{
    if (m_registers == nullptr)
    {
        throw rdk::exception::missing_interface();
    }

    uint16_t val =  m_registers->read(addr);
    LTR11RegisterManipulator::BatchType reg_val{addr, val};
    return reg_val;
}

// ================
// register getters
// ================
void LTR11RegisterManipulator::addSetPulseMode(bool enabled)
{
    value_t value = DIGI_CTRL_REG15_REG_RST;

    value &= ~digital_control_pulse_mode_MASK;

    const value_t start_pm_value = 1;
    const value_t start_cw_value = (enabled ? 0 : 1);  // false when in pulse mode
    const value_t miso_drv_value = 1;
    const value_t stat_mux_value = status_multiplexer_amplitude;

    value |= (start_pm_value << start_pm_BITS);
    value |= (start_cw_value << start_cw_BITS);
    value |= (miso_drv_value << miso_drv_BITS);
    value |= stat_mux_value;

    addSetRegister({DIGI_CTRL_REG15_REG_ADDR, value});
}

LTR11RegisterManipulator::value_t LTR11RegisterManipulator::convertRFCFreq_toRegisterValue(uint64_t freq_Hz)
{
    auto f_freq_Hz = static_cast<float>(freq_Hz);
    float value    = m * f_freq_Hz + n;

    auto ret_value = static_cast<value_t>(std::floor(value + 0.5));  // round to closest

    return ret_value;
}

uint64_t LTR11RegisterManipulator::convertToRfCFreq(uint16_t freq_reg)
{
    float value_Hz = (static_cast<float>(freq_reg) - n) / m;

    auto ret_value_Hz = static_cast<uint64_t>(std::floor(value_Hz));

    return ret_value_Hz;
}

void LTR11RegisterManipulator::dumpRegisters(const char *filename) const
{
    std::ofstream f;
    f.open(filename);
    if (!f)
    {
        IFX_LOG_ERROR("Could not open file for dump");
        return;
    }

    for (auto address = 0; address < LTR11_REGISTERS_NUMB; address++)
    {
        auto regAddrVal = getRegisterInternal(address);

        f << "REG_" << address << " ";
        f << std::hex << std::setfill('0') << std::setw(sizeof(address)) << address << " ";
        f << std::dec << regAddrVal.value << "\n";
    }
    f.close();
}

bool LTR11RegisterManipulator::isJapanBand() const
{
    const uint16_t japanMode = 0;
    const uint16_t registerBit = getRegisterInternal(E_FUSES_REG55_REG_ADDR).value & (1 << pll_japan_mode_BIT);
    return registerBit == japanMode;
}

void LTR11RegisterManipulator::disableInternalDetector()
{
     value_t value = DAR_REG1_REG_RST;

    /* The value to set to Register 1 is xored with the register value set by the main controller.
       Since register 1 is initialized to 0, in order to disable the internal detector, we need to write 1
       to the bb_dig_det bit.
    */
    value &= ~bb_dig_det_en_MASK;
    value |= 1 << bb_dig_det_en_BITS;

    addSetRegister({DAR_REG1_REG_ADDR, value});
}

void LTR11RegisterManipulator::addSetRegister(const BatchType& reg){
    m_registerQueue.push_back(reg);
}

void LTR11RegisterManipulator::flushEnqRegisters()
{
    if (m_registers == nullptr)
    {
        throw rdk::exception::missing_interface();
    }
    m_registers->writeBatch(m_registerQueue .data(), m_registerQueue .size());
    m_registerQueue.clear();
}