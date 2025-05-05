% ===========================================================================
% Copyright (C) 2021-2022 Infineon Technologies AG
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are met:
%
% 1. Redistributions of source code must retain the above copyright notice,
%    this list of conditions and the following disclaimer.
% 2. Redistributions in binary form must reproduce the above copyright
%    notice, this list of conditions and the following disclaimer in the
%    documentation and/or other materials provided with the distribution.
% 3. Neither the name of the copyright holder nor the names of its
%    contributors may be used to endorse or promote products derived from
%    this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
% SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
% INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
% CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
% ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
% POSSIBILITY OF SUCH DAMAGE.
% ===========================================================================

clear all;
addpath('../../../sdk/matlab/RadarSDKMEXWrapper');
% load package if this is not matlab
if((exist ("OCTAVE_VERSION", "builtin") > 0))
    pkg load communications
end

disp(['Radar SDK Version: ' RadarDevice.get_version_full()]);

%##########################################################################
% STEP 1: create one DeviceConfig instance. Please refer to DeviceConfig.m
% for more details about TR13C or ATR24C (MIMO) configuration. The
% following example configs are also provided.
%##########################################################################

% set device configuration for presence sensing
oDevConf = DeviceConfig(1e6, ... % sample_rate_Hz
                       1, ... % rx_mask
                       1, ... % tx_mask
                       31, ... % tx_power_level
                       33, ... % if_gain_dB
                       59133931281, ... % start_frequency_Hz
                       62366068720, ... % end_frequency_Hz
                       64, ... % num_samples_per_chirp
                       32, ... % num_chirps_per_frame
                       0.000411238, ... % chirp_repetition_time_s
                       0.125, ... % frame_repetition_time_s
                       80000, ... % hp_cutoff_Hz
                       500000, ... % aaf_cutoff_Hz
                       0); % mimo_mode

%##########################################################################
% STEP 2: Create a RadarDevice object and connect to the Radar
%##########################################################################
Dev = RadarDevice();

%##########################################################################
% STEP 3: configure the radar device with the required config values
%##########################################################################
Dev.set_config(oDevConf);

%##########################################################################
% STEP 4: Fetch Radar Data
%##########################################################################

fdata = Dev.get_next_frame();
% fetching frame data for the first configured antenna
mat = squeeze(fdata(1,:,:));

% shape of frame data for one antenna
[num_chirps, chirp_samples] = size(mat);

% Presence sensing algorithm parameters
detect_start_sample = round(chirp_samples/8);       % detection start sample corresponding to 20cm
detect_end_sample = round(chirp_samples/2);         % detection end sample corresponding to 80cm
threshold_presence = 0.0007;                        % threshold for object detection
alpha_slow = 0.001;                                 % slow average update coefficient
alpha_med = 0.05;                                   % medium average update coefficient
alpha_fast = 0.6;                                   % fast average update coefficient
presence_status = 0;

result = zeros(1,12);

hFig = figure;
% In this example, show frames till figure is closed.
frame_number = 0;
%##########################################################################
% STEP 5: Continue fetching Radar data and run desired algorithm on it.
%##########################################################################
while ishandle(hFig)
    fdata = Dev.get_next_frame();
    if (isempty(fdata))
        continue;
    end
    % fetching frame data for the first configured antenna
    mat = squeeze(fdata(1,:,:));
    %% run algorithm
    w = window(@blackmanharris,chirp_samples);
    avg_windowed = (mat - mean(mat,2)).*w';

    zero_padded = [ avg_windowed zeros(size(avg_windowed)) ];

    range_fft = (fft(zero_padded'))/chirp_samples;
    range_fft = 2*range_fft.';
    % ignore negative spectrum
    range_fft = range_fft(:,1:chirp_samples);

    fft_abs = abs(range_fft);
    fft_avg = mean(fft_abs);

    if frame_number == 0  % initialize average updates
        slow_avg = fft_avg;
        fast_avg = fft_avg;
    end

    % Choose slow coefficient
    if ~presence_status
        alpha_used = alpha_med;
    else
        alpha_used = alpha_slow;
    end
    slow_avg = slow_avg*(1-alpha_used) + fft_avg*alpha_used;
    fast_avg = fast_avg*(1-alpha_fast) + fft_avg*alpha_fast;
    data = (fast_avg-slow_avg);

    %% Detection
    presence_status = max(data(detect_start_sample:detect_end_sample))>threshold_presence;
    %% Display result
    result = [result(2:end) presence_status];
    bar(result,1);
    axis([1 length(result) 0 1 ]);
    frame_number = frame_number+1;
    title(sprintf('Frame %d',frame_number));
    drawnow
end

%##########################################################################
% STEP 6: Clear the RadarDevice object. It also stops Radar data and
% disconnects the device from the handle.
%##########################################################################
clear Dev
