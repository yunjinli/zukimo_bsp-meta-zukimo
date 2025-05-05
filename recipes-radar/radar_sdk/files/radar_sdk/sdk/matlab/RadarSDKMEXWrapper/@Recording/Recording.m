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

classdef Recording
    %RECORDING Class definition for opening recording file and creating a
    %handle for it
    properties (Access = private)
        recording_handle;      % recording handle used for opening recording as device
    end
    methods
        function obj = Recording(path, mode, type, index)
            %RECORDING Construct an instance of this class
            %   The function instantiates a Recording object
            %
            %                     Parameters:
            %             path:   [string] a path to the catalog with a recording (parent directory)
            %             mode:   [int number] mode of the recording file
            %                      Note! Currently supporting only Read mode
            %                      0 - Read mode, 1 - Write mode, 2 - Read/Write mode
            %                      as in C API ifx_Recording_Mode_t
            %             type:   [int number] type of the recording (associated with device type)
            %                      Note! Currently supporting only Avian recording type
            %                      0 - Avian recording type, 1 - Unknown type
            %                      as in C API ifx_Recording_Type_t
            %             index:  [int number] Index/order number of a recording catalog.
            %                     Catalogs with recordings are named: "RadarIfx<RadarType>_XX"
            %                     where XX is a recording index, e.g. RadarIfxAvian_01.
            %                     For numbers lower than 10, one provides just a number 0-9.
            [ec, obj.recording_handle] = DeviceControlM('recording_create', path, mode, type, index);    
            RadarDevice.check_error_code(ec);
        end
        function handle = get_handle(obj)
           handle = obj.recording_handle;
        end
        function delete(obj)
            %RADARDEVICE destroyer for the RadarDevice object
            ec = DeviceControlM('recording_destroy', obj.recording_handle);
            RadarDevice.check_error_code(ec);
            obj.recording_handle = 0;
        end
    end

end

