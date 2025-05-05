# ===========================================================================
# Copyright (C) 2022 Infineon Technologies AG
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ===========================================================================

import numpy as np
from ifxAvian.ConstantWave import *
from ifxAvian.Avian import *

print("Radar SDK Version: " + get_version_full())

# open device: The device will be closed at the end of the block. Instead of
# the with-block you can also use:
#   device = Device()
# However, the with block gives you better control when the device is closed.
with Device() as device:
    with DeviceConstantWave(device) as device_cw:

        config = device_cw.get_default_config_as_dict()

        adc_configs = create_adc_configs_struct(config["adc_configs"]["samplerate_Hz"],
                                                config["adc_configs"]["tracking"],
                                                config["adc_configs"]["sample_time"],
                                                config["adc_configs"]["double_msb_time"],
                                                config["adc_configs"]["oversampling"])

        baseband_configs = create_baseband_configs_struct(config["baseband_configs"]["vga_gain"],
                                                          config["baseband_configs"]["hp_gain"],
                                                          80_000, # hp_cutoff_Hz
                                                          500_000) # aaf_cutoff_Hz

        test_signal_configs = create_test_signal_configs_struct(config["test_signal_configs"]["mode"],
                                                                config["test_signal_configs"]["frequency_Hz"])

    # setting up the device
        device_cw.set_rf_frequency(config["rf_freq_Hz"]["value"])
    
        device_cw.set_tx_dac_value(config["tx_dac_value"]["value"])

        device_cw.set_tx_dac_value((config["tx_dac_value"]["value"]))

        device_cw.set_num_of_samples_per_antenna(config["num_of_samples"]["value"])

        device_cw.set_num_of_samples_per_antenna((config["num_of_samples"]["value"]))

        device_cw.set_baseband_params(baseband_configs)

        device_cw.set_adc_params(adc_configs)

        device_cw.set_test_signal_generator_config(test_signal_configs)

        print("num samples", config["num_of_samples"]["value"])

        device_cw.start_emission()

        for frame_number in range(10):
            try:
                frame = device_cw.capture_frame()
            except ErrorFifoOverflow:
                print("Fifo Overflow")
                exit(1)
            
            num_rx = np.shape(frame)[0]

            # Do some processing with the obtained frame.
            # In this example we just dump it into the console
            print("Got frame " + format(frame_number) + ", num_antennas={}".format(num_rx))

            for iAnt in range(0, num_rx):
                mat = frame[iAnt, :]
                print("Antenna", iAnt, "\n", mat)

        del frame
