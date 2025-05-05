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

"""Python wrapper for Infineon Avian Radar sensors

The wrapper allows to co
"""

from ctypes import *
import numpy as np
import typing

from .Error import *
from .Common import *
from .RadarTypes import *

# by default,
#   from ifxRadarSDK import *
# would import all objects, including the ones from ctypes. To avoid name space
# pollution, we list what symbols should be exported.
error_class_list = list(error_mapping_exception.values())
__all__ = ["Device", "get_version", "get_version_full", "DeviceConfig", "DeviceMetrics", "RadarSensor",
           "ShieldType", "SensorInfo", "ShieldInfo", "FirmwareInfo", *error_class_list]


@call_once # load DLL only once and then return cached value
def dll() -> CDLL:
    """Initialize the module and return ctypes handle"""
    # find and load shared library
    lib = load_library("radar_sdk")

    # declare prototypes such that ctypes knows the arguments and return types
    declare_prototype(lib, "ifx_avian_create", None, c_void_p)
    declare_prototype(lib, "ifx_avian_destroy", [c_void_p], None)
    declare_prototype(lib, "ifx_avian_get_register_list_string", [c_void_p, c_bool], POINTER(c_char))
    declare_prototype(lib, "ifx_avian_create_by_port", [c_char_p], c_void_p)
    declare_prototype(lib, "ifx_avian_get_list", None, c_void_p)
    declare_prototype(lib, "ifx_avian_get_list_by_sensor_type", [c_int], c_void_p)
    declare_prototype(lib, "ifx_avian_create_by_uuid", [c_char_p], c_void_p)
    declare_prototype(lib, "ifx_avian_get_board_uuid", [c_void_p], c_char_p)
    declare_prototype(lib, "ifx_uuid_to_string", [POINTER(c_uint8), c_char_p], None)
    declare_prototype(lib, "ifx_avian_set_config", [c_void_p, POINTER(DeviceConfig)], None)
    declare_prototype(lib, "ifx_avian_get_config", [c_void_p, POINTER(DeviceConfig)], None)
    declare_prototype(lib, "ifx_avian_get_config_defaults", [c_void_p, POINTER(DeviceConfig)], None)
    declare_prototype(lib, "ifx_avian_metrics_get_defaults", [c_void_p, POINTER(DeviceMetrics)], None)
    declare_prototype(lib, "ifx_avian_start_acquisition", [c_void_p], c_bool)
    declare_prototype(lib, "ifx_avian_stop_acquisition", [c_void_p], c_bool)
    declare_prototype(lib, "ifx_avian_get_next_frame", [c_void_p, POINTER(CubeReal)], POINTER(CubeReal))
    declare_prototype(lib, "ifx_avian_get_next_frame_timeout", [c_void_p, POINTER(CubeReal), c_uint16], POINTER(CubeReal))
    declare_prototype(lib, "ifx_avian_get_temperature", [c_void_p, POINTER(c_float)], None)
    declare_prototype(lib, "ifx_avian_get_firmware_information", [c_void_p], POINTER(FirmwareInfo))
    declare_prototype(lib, "ifx_avian_get_shield_information", [c_void_p, POINTER(ShieldInfo)], None)
    declare_prototype(lib, "ifx_avian_get_sensor_information", [c_void_p], POINTER(SensorInfo))
    declare_prototype(lib, "ifx_avian_metrics_to_config", [c_void_p, POINTER(DeviceMetrics), POINTER(DeviceConfig), c_bool], None)
    declare_prototype(lib, "ifx_avian_metrics_from_config", [c_void_p, POINTER(DeviceConfig), POINTER(DeviceMetrics)], None)
    declare_prototype(lib, "ifx_avian_get_hp_cutoff_list", [c_void_p], c_void_p)
    declare_prototype(lib, "ifx_avian_get_aaf_cutoff_list", [c_void_p], c_void_p)
    declare_prototype(lib, "ifx_recording_create", [POINTER(c_char), c_int, c_int, c_int32], c_void_p)
    declare_prototype(lib, "ifx_avian_create_dummy_from_recording", [c_void_p, c_bool], c_void_p)

    return lib

def get_version(full : bool = False) -> str:
    """Return SDK version string

    If full is false, the major, minor, and patch number of the SDK version is
    returned as a string in the format "X.Y.Z". If full is true, the full
    version information including git tag and git commit is returned.
    """
    return get_sdk_version(dll(), full)

def get_version_full() -> str:
    """Return full SDK version string including git tag from which it was build"""
    return get_sdk_version(dll(), full = True)

class Device():
    @staticmethod
    def get_list(sensor_type : typing.Optional[RadarSensor] = None) -> typing.List[str]:
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

        if sensor_type == None:
            ifx_list = dll().ifx_avian_get_list()
        else:
            ifx_list = dll().ifx_avian_get_list_by_sensor_type(int(sensor_type))

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
    def __init__(self, uuid : typing.Optional[str] = None, port : typing.Optional[str] = None, recording : typing.Optional[int] = None, correct_timing : typing.Optional[bool] = None):
        """Connect to a board

        Search for an Infineon radar sensor board connected to the host machine
        and connect to the first found sensor device.

        The device is automatically closed by the destructor. If you want to
        close the device yourself, you can use the keyword del:
            device = Device()
            # do something with device
            ...
            # close device
            del device

        However, we suggest to use a context manager:
            with open Device() as dev:
                # do something with device

        The same suggestion regards creating Device from a saved Recording:
            with Recording("path_to_recording", 0, 0, 0) as test_recording:
                with Device(recording = test_recording, correct_timing = False) as device:
                    #do something with device created from a saved recording

        If port is given, the specific port is opened. If uuid is given and
        port is not given, the radar device with the given uuid is opened. If
        no parameters are given, the first found radar device will be opened.

        Examples:
          - Open first found radar device:
            dev = Device()
          - Open radar device on COM5:
            dev = Device(port="COM5")
          - Open radar device with uuid 0123456789abcdef0123456789abcdef
            dev = Device(uuid="0123456789abcdef0123456789abcdef")
          - Open a dummy radar device from a saved Common File Format Recording:
            recording_handle = Recording("path_to_recording", 0, 0, 0)
            dev = Device(recording = recording_handle, correct_timing = True)

        Optional parameters:
            port:       opens the given port
            uuid:       open the radar device with unique id given by uuid
                        the uuid is represented as a 32 character string of
                        hexadecimal characters. In addition, the uuid may
                        contain dash characters (-) which will be ignored.
                        Both examples are valid and correspond to the same
                        uuid:
                            0123456789abcdef0123456789abcdef
                            01234567-89ab-cdef-0123-456789abcdef
            recording:  a handle to a Recording object created from a saved
                        Common File Format Recording (refer to Recording class)
            correct_timing:     a flag used only when a device is created
                                from the Recording - if set to True, the device
                                will simulate a timing of the real device, e.g.
                                will return a frame using get_next_frame after
                                a proper time passes (specified in config via
                                frame_repetition_time_s). Otherwise, if set to False,
                                it will not simulate any timings and will return
                                the frame immediately.
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
            h = self._dll.ifx_avian_create_by_uuid(uuid.encode("ascii"))
        elif port:
            h = self._dll.ifx_avian_create_by_port(port.encode("ascii"))
        elif recording:
            correct_timing = correct_timing or False
            h = self._dll.ifx_avian_create_dummy_from_recording(recording, correct_timing)
        else:
            h = self._dll.ifx_avian_create()

        self.handle = c_void_p(h) # Reason of that cast HMI-2896

    @handle_errors(dll())
    def metrics_to_config(self, metrics : DeviceMetrics, power_of_two : bool = True) -> DeviceConfig:
        """Convert metrics configuration to device configuration

        Parameter:
            metrics: metrics configuration
            power_of_two: If true the values for num_samples_per_chirp and
                          num_chirps_per_frame are rounded up to a power of 2
        """
        config = DeviceConfig(0,0,0,0,0,0,0,0,0,0,0)
        self._dll.ifx_avian_metrics_to_config(self.handle, byref(metrics), byref(config), power_of_two)
        return config

    @handle_errors(dll())
    def metrics_from_config(self, config : DeviceConfig) -> DeviceMetrics:
        metrics = DeviceMetrics(0,0,0,0,0,0,0,0,0,0,0)
        self._dll.ifx_avian_metrics_from_config(self.handle, byref(config), byref(metrics))
        return metrics

    @handle_errors(dll())
    def set_config(self, config : DeviceConfig) -> None:
        """Set device configuration"""
        self._dll.ifx_avian_set_config(self.handle, byref(config))

    @handle_errors(dll())
    def get_config(self) -> DeviceConfig:
        """Get current device configuration"""
        config = DeviceConfig(0,0,0,0,0,0,0,0,0,0,0)
        self._dll.ifx_avian_get_config(self.handle, byref(config))
        return config

    @handle_errors(dll())
    def get_config_defaults(self) -> DeviceConfig:
        """Get default device configuration"""
        config = DeviceConfig(0,0,0,0,0,0,0,0,0,0,0)
        self._dll.ifx_avian_get_config_defaults(self.handle, byref(config))
        return config

    @handle_errors(dll())
    def get_metrics_defaults(self) -> DeviceMetrics:
        """Get default metrics configuration"""
        metrics = DeviceMetrics(0,0,0,0,0,0,0,0,0,0,0)
        self._dll.ifx_avian_metrics_get_defaults(self.handle, byref(metrics))
        return metrics

    @handle_errors(dll())
    def start_acquisition(self) -> None:
        """Start acquisition of time domain data

        Starts the acquisition of time domain data from the connected device.
        If the data acquisition is already running the function has no effect.
        """
        self._dll.ifx_avian_start_acquisition(self.handle)

    @handle_errors(dll())
    def stop_acquisition(self) -> None:
        """Stop acquisition of time domain data

        Stops the acquisition of time domain data from the connected device.
        If the data acquisition is already stopped the function has no effect.
        """
        self._dll.ifx_avian_stop_acquisition(self.handle)

    # no decorator
    def get_next_frame(self, timeout_ms : typing.Optional[int] = None) -> np.ndarray:
        """Retrieve next frame of time domain data from device

        Retrieve the next complete frame of time domain data from the connected
        device. The samples from all chirps and all enabled RX antennas will be
        copied to the provided data structure frame.

        The frame is returned as numpy array with dimensions
        num_virtual_rx_antennas x num_chirps_per_frame x num_samples_per_frame.

        If timeout_ms is given, the exception ErrorTimeout is raised if a
        complete frame is not available within timeout_ms milliseconds.
        """
        if timeout_ms:
            frame = self._dll.ifx_avian_get_next_frame_timeout(self.handle, None, timeout_ms)
        else:
            frame = self._dll.ifx_avian_get_next_frame(self.handle, None)
        check_rc(self._dll)
        frame_numpy = frame.contents.to_numpy()
        self._dll.ifx_cube_destroy_r(frame)
        return frame_numpy

    # no decorator
    def get_board_uuid(self) -> str:
        """Get the unique id for the radar board"""
        c_uuid = self._dll.ifx_avian_get_board_uuid(self.handle)
        check_rc(self._dll)
        return c_uuid.decode("utf-8")

    @handle_errors(dll())
    def get_temperature(self) -> float:
        """Get the temperature of the device in degrees Celsius

        Note that reading the temperature is not supported for UTR11. An
        exception will be raised in this case.
        """
        temperature = c_float(0)
        self._dll.ifx_avian_get_temperature(self.handle, pointer(temperature))
        return float(temperature.value)

    # no decorator
    def get_firmware_information(self) -> FirmwareInfo:
        """Gets information about the firmware of a connected device"""
        info_p = self._dll.ifx_avian_get_firmware_information(self.handle)
        check_rc(self._dll)
        return info_p.contents.to_dict(True)

    @handle_errors(dll())
    def get_shield_information(self) -> ShieldInfo:
        """Get information about the RF shield by reading its EEPROM

        The shield information is read from the EEPROM on the RF shield. If the RF shield
        does not contain an EEPROM, the EEPROM is broken or not correctly initialized,
        the exception ErrorEeprom is raised.
        """
        info = ShieldInfo(0)
        self._dll.ifx_avian_get_shield_information(self.handle, byref(info))
        return info.to_dict(True)

    @handle_errors(dll())
    def get_hp_cutoff_list(self) -> typing.List[str]:
        """Gets a list of available High Pass Cutoff frequencies in [Hz] for a connected device"""
        frequencies = []
        ifx_list = dll().ifx_avian_get_hp_cutoff_list(self.handle)
        size = dll().ifx_list_size(ifx_list)

        for i in range(size):
            p = dll().ifx_list_get(ifx_list, i)
            freq = cast(p, POINTER(c_uint32)).contents.value
            frequencies.append(freq)
        dll().ifx_list_destroy(ifx_list)

        return frequencies

    @handle_errors(dll())
    def get_aaf_cutoff_list(self) -> typing.List[str]:
        """Gets a list of available Anti Aliasing Filter Cutoff frequencies in [Hz] for a connected device"""
        frequencies = []
        ifx_list = dll().ifx_avian_get_aaf_cutoff_list(self.handle)
        size = dll().ifx_list_size(ifx_list)

        for i in range(size):
            p = dll().ifx_list_get(ifx_list, i)
            freq = cast(p, POINTER(c_uint32)).contents.value
            frequencies.append(freq)
        dll().ifx_list_destroy(ifx_list)

        return frequencies

    # no decorator
    def get_sensor_information(self) -> SensorInfo:
        """Gets information about the connected device"""
        info_p = self._dll.ifx_avian_get_sensor_information(self.handle)
        check_rc(self._dll)
        return info_p.contents.to_dict(True)

    # no decorator
    def get_register_list_string(self, trigger : bool) -> str:
        """Get the exported register list as a hexadecimal string"""
        ptr = self._dll.ifx_avian_get_register_list_string(self.handle,trigger)
        check_rc(self._dll)
        reg_list_string = cast(ptr, c_char_p).value
        reg_list_string_py = reg_list_string.decode('ascii')
        self._dll.ifx_mem_free(ptr)
        return reg_list_string_py

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._close()

    def _close(self):
        """Destroy device handle"""
        if hasattr(self, "handle") and self.handle:
            dll().ifx_avian_destroy(self.handle)
            self.handle = None

    def __del__(self):
        try:
            self._close()
        except:
            # just like in C++ exceptions raised in a destructor cannot be catched
            # for this reason we ignore them
            pass

class Recording():
    @handle_errors(dll())
    def __init__(self, path : str = None, mode : int = None, type : int = None, index : int = None):
        """Open a saved Common File Format Recording file and create a recording handle for it.

        **Examples**
            with Recording("path_ro_recording", 0, 0, 0) as recording:
                # use recording here

        Parameters:
            path:   [string] a path to the catalog with a recording (parent directory)
            mode:   [int number] mode of the recording file
                     Note! Currently supporting only Read mode
                     0 - Read mode, 1 - Write mode, 2 - Read/Write mode
                     as in C API ifx_Recording_Mode_t
            type:   [int number] type of the recording (associated with device type)
                     Note! Currently supporting only Avian recording type
                     0 - Avian recording type, 1 - Unknown type
                     as in C API ifx_Recording_Type_t
            index:  [int number] Index/order number of a recording catalog.
                    Catalogs with recordings are named: "RadarIfx<RadarType>_XX"
                    where XX is a recording index, e.g. RadarIfxAvian_01.
                    For numbers lower than 10, one provides just a number 0-9.
    """
        handle = dll().ifx_recording_create(path.encode("ascii"), mode, type, index)
        self.handle = c_void_p(handle) # Reason of that cast HMI-2896

    def __enter__(self):
        return self.handle

    def __exit__(self, exc_type, exc_value, traceback):
        self._close()

    def _close(self):
        """Destroy recording handle"""
        if hasattr(self, "handle") and self.handle:
            dll().ifx_recording_destroy(self.handle)
            self.handle = None

    def __del__(self):
        try:
            self._close()
        except:
            # just like in C++ exceptions raised in a destructor cannot be catched
            # for this reason we ignore them
            pass