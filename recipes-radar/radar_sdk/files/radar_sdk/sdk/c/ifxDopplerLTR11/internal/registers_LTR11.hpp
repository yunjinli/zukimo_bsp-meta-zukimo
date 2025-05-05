/**
 * @file registers_LTR11.hpp
 *
 * This file contains register and bit field definition for the LTR11
 */
 
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

    constexpr auto LTR11_REGISTERS_NUMB     = 0x0039;

    //Register addresses
    constexpr auto DAR_REG0_REG_ADDR             = 0x0000;
    constexpr auto DAR_REG1_REG_ADDR             = 0x0001;
    constexpr auto THOLD_REG2_REG_ADDR           = 0x0002;
    constexpr auto PLL_CONFIG1_REG4_REG_ADDR     = 0x0004;
    constexpr auto PLL_CONFIG2_REG5_REG_ADDR     = 0x0005;
    constexpr auto PLL_CONFIG3_REG6_REG_ADDR     = 0x0006;
    constexpr auto DC_TMG_PD_MPA_REG7_REG_ADDR   = 0x0007;
    constexpr auto DIV_REG8_REG_ADDR             = 0x0008;
    constexpr auto BB_REG9_REG_ADDR              = 0x0009;
    constexpr auto HT_REG10_REG_ADDR             = 0x000A;
    constexpr auto ALGO1_REG13_REG_ADDR          = 0x000D;
    constexpr auto ALGO2_REG14_REG_ADDR          = 0x000E;
    constexpr auto DIGI_CTRL_REG15_REG_ADDR      = 0x000F;
    constexpr auto E_FUSES_REG55_REG_ADDR        = 0x0037;

    constexpr auto THOLD_REG2_REG_RST           = 0x0000;
    constexpr auto DC_TMG_PD_MPA_REG7_REG_RST   = 0x0000;
    constexpr auto BB_REG9_REG_RST              = 0x0000;
    constexpr auto PLL_CONFIG2_REG5_REG_RST     = 0x0000;
    constexpr auto DIGI_CTRL_REG15_REG_RST      = 0x0000;
    constexpr auto ALGO1_REG13_REG_RST          = 0x0000;
    constexpr auto DAR_REG1_REG_RST             = 0x0000;

    constexpr auto dc_on_pulse_len_BITS = 8;
    constexpr auto dc_on_pulse_len_MASK = (0x03 << dc_on_pulse_len_BITS);  // bits 8,9

    constexpr auto dc_rep_rate_BITS = 10;
    constexpr auto dc_rep_rate_MASK = (0x03 << dc_rep_rate_BITS);  // bits 10,11

    constexpr auto thres_MASK = 0x1FFF;

    constexpr auto mpa_ctrl_MASK = 0x07;

    constexpr auto bb_ctrl_gain_MASK = 0x0F;

    constexpr auto pll_fcw_MASK = 0x0FFF;

    constexpr auto aprt_BITS = 14;
    constexpr auto aprt_MASK = (0x01 << aprt_BITS);

    // digital control (pulse mode and constant wave mode) related - begin
    constexpr auto start_pm_BITS = 14;
    constexpr auto start_pm_MASK = (0x01 << start_pm_BITS);

    constexpr auto start_cw_BITS = 12;
    constexpr auto start_cw_MASK = (0x01 << start_cw_BITS);

    constexpr auto miso_drv_BITS = 6;
    constexpr auto miso_drv_MASK = (0x01 << miso_drv_BITS);

    constexpr auto stat_mux_MASK = 0x0F;

    constexpr auto digital_control_pulse_mode_MASK = (stat_mux_MASK | miso_drv_MASK | start_cw_MASK | start_pm_MASK);
    // digital control related - end

    constexpr auto prt_mult_MASK = 0x03;

    constexpr auto pll_japan_mode_BIT = 15;

    constexpr auto status_multiplexer_amplitude = 6;

    constexpr auto bb_dig_det_en_BITS = 7;
    constexpr auto bb_dig_det_en_MASK = 1 << bb_dig_det_en_BITS;