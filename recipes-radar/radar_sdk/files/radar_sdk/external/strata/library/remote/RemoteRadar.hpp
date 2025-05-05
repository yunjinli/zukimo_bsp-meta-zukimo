/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemoteVendorCommands.hpp"
#include <components/interfaces/IRadar.hpp>
#include <platform/interfaces/IVendorCommands.hpp>

class RemoteRadar :
    virtual public IRadar
{
public:
    RemoteRadar(IVendorCommands *commands, uint8_t id);

    //IRadar
    void initialize() override;
    void reset(bool softReset) override;
    void configure(const IfxRfe_MmicConfig *c) override;
    void loadSequence(IfxRfe_Sequence *s) override;
    void calibrate() override;
    void startSequence() override;
    uint8_t getDataIndex() override;
    void startData() override;
    void stopData() override;
    void enableConstantFrequencyMode(uint16_t txMask, float txPower) override;
    void setConstantFrequency(double frequency) override;

private:
    RemoteVendorCommands m_vendorCommands;
};
