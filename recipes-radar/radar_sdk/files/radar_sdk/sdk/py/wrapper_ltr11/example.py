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

from ifxDopplerLTR11 import DopplerLTR11


print ("Radar SDK Version: " + DopplerLTR11.get_version_full())

# open device: The device will be closed at the end of the block. Instead of
# the with-block you can also use:
#   device = BGT60LTR11Device()
# However, the with block gives you better control when the device is closed.
with DopplerLTR11.DopplerLTR11() as ltr11:
    # set device config
    config_defaults = ltr11.get_config_defaults()
    print("Configuration limits", ltr11.get_limits())
    ltr11.set_config(config_defaults)
    print("Power for current set configuration: ", ltr11.get_active_mode_power(config_defaults))
    print("Current Sampling frequency in Hz: ", ltr11.get_sampling_frequency(config_defaults.to_dict()["prt"]))
    ltr11.start_acquisition()
    print("acquisition started")

    # A loop for fetching a finite number of frames comes next..
    for frame_number in range(100):
        frame, metadata = ltr11.get_next_frame()
        metadata_dict = metadata.to_dict()
        print("Chip power mode: ", "active mode " if metadata_dict['active'] else "low power mode")
        print("Target Detected" if (metadata_dict['motion'] == 0) else "No Target Detected")
        if metadata_dict['motion'] == 0:
            print("Approaching " if metadata_dict['direction'] else " Departing")
        print("Average Power Consumption is equal to: ", metadata_dict['avg_power'])

    ltr11.stop_acquisition()
    print("acquisition stopped")

    print("Sensor information: ", ltr11.get_sensor_information())
