# ===========================================================================
# Copyright (C) 2021-2022 Infineon Technologies AG
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

"""Python wrapper for Infineon the LTR11 Radar device.

The package expects the library (radar_sdk.dll on Windows, libradar_sdk.so on
Linux, libradar_sdk.dylib on macOS) either in the same directory as this file
(ifxRadarSDK.py) or in a subdirectory ../../libs/ARCH/ relative to this file
where ARCH is depending on the platform either win32_x86, win32_x64, raspi,
linux_x64 or macOS.
"""

from ctypes import *
import numpy as np
import typing

from .Error import *
from .Common import *
from .CommonTypes import *
from .DopplerLTR11Types import *

# by default,
#   from DopplerLTR11 import *
# would import all objects, including the ones from ctypes. To avoid name space
# pollution, we list what symbols should be exported.
error_class_list = list(error_mapping_exception.values())
__all__ = ["DopplerLTR11", "get_version", "get_version_full", "DopplerLTR11Config", "RadarSensor",
           "ShieldType", "SensorInfo", "ShieldInfo", "FirmwareInfo", *error_class_list]


@call_once  # load DLL only once and then return cached value
def dll() -> CDLL:
    """Initialize the module and return ctypes handle"""
    # find and load shared library
    lib = load_library("radar_sdk")

    # declare prototypes such that ctypes knows the arguments and return types
    declare_prototype(lib, "ifx_ltr11_create", None, c_void_p)
    declare_prototype(lib, "ifx_ltr11_destroy", [c_void_p], None)
    declare_prototype(lib, "ifx_ltr11_create_by_uuid", [c_void_p], c_void_p)
    declare_prototype(lib, "ifx_ltr11_get_list", None, c_void_p)
    declare_prototype(lib, "ifx_ltr11_get_config_defaults", [
                      c_void_p, POINTER(DopplerLTR11Config)], None)
    declare_prototype(lib, "ifx_ltr11_get_config", [
                      c_void_p, POINTER(DopplerLTR11Config)], None)
    declare_prototype(lib, "ifx_ltr11_set_config", [
                      c_void_p, POINTER(DopplerLTR11Config)], None)
    declare_prototype(lib, "ifx_ltr11_start_acquisition", [c_void_p], c_bool)
    declare_prototype(lib, "ifx_ltr11_stop_acquisition", [c_void_p], c_bool)
    declare_prototype(lib, "ifx_ltr11_get_next_frame", [
                      c_void_p, POINTER(VectorComplex)], POINTER(VectorComplex))
    declare_prototype(lib, "ifx_ltr11_get_next_frame_timeout", [
                      c_void_p, POINTER(VectorComplex)], POINTER(VectorComplex))
    declare_prototype(lib, "ifx_ltr11_get_sensor_information", [
                      c_void_p], POINTER(SensorInfo))
    declare_prototype(lib, "ifx_ltr11_get_firmware_information", [
                      c_void_p], POINTER(FirmwareInfo))
    declare_prototype(lib, "ifx_ltr11_get_active_mode_power", [
                      c_void_p, POINTER(DopplerLTR11Config)], c_float)
    declare_prototype(lib, "ifx_ltr11_get_sampling_frequency", [
                      c_void_p, c_int], c_uint32)
    declare_prototype(lib, "ifx_ltr11_check_config", [
                      c_void_p, POINTER(DopplerLTR11Config)], c_bool)
    declare_prototype(lib, "ifx_ltr11_register_dump_to_file",
                      [c_void_p, c_char_p], None)
    declare_prototype(lib, "ifx_ltr11_get_limits", [
                      c_void_p, POINTER(DopplerLTR11ConfigLimits)], None)

    return lib


def get_version(full: bool = False) -> str:
    """Return SDK version string

    If full is False, the major, minor, and patch number of the SDK version 
    is returned as a string in the format "X.Y.Z". If full is true, the full
    version information including git tag and git commit is returned.
    """
    return get_sdk_version(dll(), full)


def get_version_full() -> str:
    """Return full SDK version string including git tag from which it was build"""
    return get_sdk_version(dll(), full=True)


class DopplerLTR11():
    @staticmethod
    def get_list() -> typing.List[str]:
        """Return list of UUIDs of connected boards

        The function returns a list of unique ids (uuids) that correspond to
        available boards. If sensor_type is specified only boards with this particular
        sensor are returned.

        **Examples**
            uuids_all   = Device.get_list()
            uuids_tr13c = Device.get_list(RadarSensor.BGT60TR13C)

        Parameters:
            sensor_type: Sensor of type RadarSensor
        """
        uuids = []

        ifx_list = dll().ifx_ltr11_get_list()

        size = dll().ifx_list_size(ifx_list)
        for i in range(size):
            p = dll().ifx_list_get(ifx_list, i)
            entry = cast(p, POINTER(DeviceListEntry))

            uuid_str = (c_char*64)()
            dll().ifx_uuid_to_string(entry.contents.uuid, uuid_str)
            uuids.append(uuid_str.value.decode("ascii"))
        dll().ifx_list_destroy(ifx_list)

        return uuids

    @handle_errors(dll())
    def __init__(self, uuid : typing.Optional[str] = None):
        """Connect to a board

        Search for an Infineon radar sensor board connected to the host machine
        and connect to the first found sensor device.

        The device is automatically closed by the destructor. If you want to
        close the device yourself, you can use the keyword del:
            ltr11 = DopplerLTR11()
            # do something with device
            ...
            # close ltr11
            del ltr11

        However, we suggest to use a context manager:
            with open DopplerLTR11() as ltr11:
                # do something with ltr11

        If uuid is given the radar device with the given uuid is opened. If
        no parameters are given, the first found radar device will be opened.

        Examples:
          - Open first found radar device:
            ltr11 = DopplerLTR11()
          - Open ltr11 doppler radar device with uuid 0123456789abcdef0123456789abcdef
            ltr11 = DopplerLTR11(uuid="0123456789abcdef0123456789abcdef")

        Optional parameters:
            uuid:       open the ltr11 doppler radar device with unique id given by uuid.
                        The uuid is represented as a 32 character string of hexadecimal
                        characters. In addition, the uuid may contain dash characters (-),
                        which will be ignored.
                        Both examples are valid and correspond to the same
                        uuid:
                            0123456789abcdef0123456789abcdef
                            01234567-89ab-cdef-0123-456789abcdef
        """
        # The Python garbage collector does not guarantee in which order objects
        # are destroyed when Python is closed. The ctypes instance obtained from
        # dll() is saved in a global variable. If the Python destructor decides to
        # destroy the ctypes instance before this object, then the destructor cannot
        # properly close the connection to the board anymore. This problem for example
        # occurs if a user aborts the script using ctrl+c.
        # To prevent this problem, save the CDLL instance as a member of this object
        # to prevent the garbage collector of destroying the CDLL instance before
        # destroying this object.
        self._dll = dll()

        if uuid:
            h = self._dll.ifx_ltr11_create_by_uuid(uuid.encode("ascii"))
        else:
            h = self._dll.ifx_ltr11_create()

        self.handle = c_void_p(h) # Reason of that cast HMI-2896

    @handle_errors(dll())
    def set_config(self, config: DopplerLTR11Config) -> None:
        """Set LTR11 configuration"""
        self._dll.ifx_ltr11_set_config(self.handle, byref(config))

    @handle_errors(dll())
    def get_config(self) -> DopplerLTR11Config:
        """Get current LTR11 configuration"""
        config = DopplerLTR11Config(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, False)
        self._dll.ifx_ltr11_get_config(self.handle, byref(config))
        return config

    @handle_errors(dll())
    def get_config_defaults(self) -> DopplerLTR11Config:
        """Get default device configuration"""
        config = DopplerLTR11Config(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, False)
        self._dll.ifx_ltr11_get_config_defaults(self.handle, byref(config))
        return config

    @handle_errors(dll())
    def get_limits(self) -> DopplerLTR11ConfigLimits:
        """Get LTR11 configuration limits"""
        limits = DopplerLTR11ConfigLimits(RFCenterFrequencyHzLimits(
            0, 0), GenericLimits(0, 0), GenericLimits(0, 0))
        self._dll.ifx_ltr11_get_limits(self.handle, byref(limits))
        return limits

    @handle_errors(dll())
    def check_config(self, config: DopplerLTR11Config) -> bool:
        """ Check if the config is valid, and return true if the config 
            is valid or false otherwise.
        """
        return self._dll.ifx_ltr11_check_config(self.handle, byref(config))

    @handle_errors(dll())
    def start_acquisition(self) -> bool:
        """Start acquisition of time domain data

        Start the acquisition of time domain data from the connected device.
        If the data acquisition is already running the function has no effect.
        """
        return self._dll.ifx_ltr11_start_acquisition(self.handle)

    @handle_errors(dll())
    def stop_acquisition(self) -> bool:
        """Stop acquisition of time domain data

        Stop the acquisition of time domain data from the connected device.
        If the data acquisition is already stopped the function has no effect.
        """
        return self._dll.ifx_ltr11_stop_acquisition(self.handle)

    # no decorator
    def get_next_frame(self, frame: typing.Optional[VectorComplex] = None, metadata: typing.Optional[DopplerLTR11Metadata] = None, timeout_ms: typing.Optional[int] = None) -> typing.Tuple[np.ndarray, DopplerLTR11Metadata]:
        """Retrieve next frame of time domain data from LTR11 device.

        Retrieve the next complete frame of time domain data from the connected
        device. The samples from all chirps and all enabled RX antennas will be
        copied to the provided data structure frame.

        The frame is returned as numpy array with dimensions the num_of_samples, 
        which is a member variable in the DopplerLTR11Config structure. 
        The metadata of type DopplerLTR11Metadata is returned. 

        If timeout_ms is given, the exception ErrorTimeout is raised if a
        complete frame is not available within timeout_ms milliseconds.
        """
        if metadata is None:
            metadata = DopplerLTR11Metadata()

        if timeout_ms:
            frame = self._dll.ifx_ltr11_get_next_frame_timeout(
                self.handle, frame, byref(metadata), timeout_ms)
        else:
            frame = self._dll.ifx_ltr11_get_next_frame(
                self.handle, frame, byref(metadata))

        check_rc(self._dll)

        frame_numpy = frame.contents.to_numpy()
        self._dll.ifx_vec_destroy_c(frame)

        return frame_numpy, metadata

    # no decorator
    def get_firmware_information(self) -> dict:
        """Get information about the firmware of a connected device"""
        info_p = self._dll.ifx_ltr11_get_firmware_information(self.handle)
        check_rc(self._dll)
        return info_p.contents.to_dict(True)

    # no decorator
    def get_sensor_information(self) -> dict:
        """Get information about the connected LTR11"""
        info_p = self._dll.ifx_ltr11_get_sensor_information(self.handle)
        check_rc(self._dll)
        return info_p.contents.to_dict(True)

    @handle_errors(dll())
    def get_active_mode_power(self, config: DopplerLTR11Config) -> float:
        """ Return the power in active mode for a given configuration. 
            i.e when the APRT (Adaptive prt LTR11 feature) is disabled,
            or when the latter feature is enabled, and a target is detected. 
            In case, the APRT is enabled and no target a detected, the power
            consumption is lower, hence, the chip is in low-power mode.
            The average power (avg_power, member of the DopplerLTR11Metadata) 
        """
        return self._dll.ifx_ltr11_get_active_mode_power(self.handle, config)

    @handle_errors(dll())
    def get_sampling_frequency(self, prt_index: int) -> int:
        """ Return the the sampling frequency in Hz for the given prt_index 
            
            In case the APRT feature of the chip is enabled and no target is detected, 
            the pulse repetition time is multiplied by the aprt_factor.
             Hence, the sampling frequency is divided by this factor. 
            This function does not include this specific case, and only returns the sampling
            frequency value computed from the prt when the chip is in active mode.
        """
        return self._dll.ifx_ltr11_get_sampling_frequency(self.handle, prt_index)

    @handle_errors(dll())
    def register_dump_to_file(self, file_name: str) -> None:
        """Dump registers to a file which name is specified in argument."""
        file_name_buffer = file_name.encode("ascii")
        file_name_buffer_p = c_char_p(file_name_buffer)
        self._dll.ifx_ltr11_register_dump_to_file(self.handle, file_name_buffer_p)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._close()

    def _close(self):
        """Destroy device handle"""
        if hasattr(self, "handle") and self.handle:
            dll().ifx_ltr11_destroy(self.handle)
            self.handle = None

    def __del__(self):
        try:
            self._close()
        except:
            # just like in C++ exceptions raised in a destructor cannot be catched
            # for this reason we ignore them
            pass
