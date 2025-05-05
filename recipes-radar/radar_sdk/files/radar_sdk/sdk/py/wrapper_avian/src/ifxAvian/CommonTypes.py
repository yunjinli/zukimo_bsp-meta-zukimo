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

"""Definitions of the various structures for the LTR11 wrapper

This file contains definitions of enumerations and structures for the LTR11
wrapper.
"""

from enum import IntEnum
from ctypes import *

from .Common import ifxStructure

class RadarSensor(IntEnum):
    """Radar sensors"""
    BGT60TR13C    = 0,   # BGT60TR13C
    BGT60ATR24C   = 1,   # BGT60ATR24C
    BGT60UTR13D   = 2,   # BGT60UTR13D
    BGT60TR12E    = 3,   # BGT60TR12E
    BGT60UTR11    = 4,   # BGT60UTR11
    BGT120UTR13E  = 5,   # BGT120UTR12E
    BGT24LTR24    = 6,   # BGT24LTR24
    BGT120UTR24   = 7,   # BGT120UTR24
    Unknown_Avian = 8,   # Unknown Avian device
    BGT24ATR22    = 128, # BGT24ATR22
    BGT60LTR11AIP = 256, # BGT60LTR11AIP
    Unknown_sensor= 4095 # Unknow sensor

class ShieldType(IntEnum):
    """Shield types

    A shield is the RF board that is plugged onto the Radar Baseboard MCU7.
    There might be several shields for the same radar sensor, e.g., with
    different antenna layouts.
    """
    Missing             = 0,
    Unknown             = 1,
    BGT60TR13AIP        = 0x0200,
    BGT60ATR24AIP       = 0x0201,
    BGT60UTR11          = 0x0202,
    BGT60UTR13D         = 0x0203,
    BGT60LTR11          = 0x0300,
    BGT60LTR11_MONOSTAT = 0x0301,
    BGT60LTR11_B11      = 0x0302,
    BGT24ATR22_ES       = 0x0400,
    BGT24ATR22_PROD     = 0x0401,


class DeviceListEntry(ifxStructure):
    """Device list entry (ifx_Radar_Sensor_List_Entry_t)"""
    _fields_ = (("sensor_type", c_int),
                ("board_type", c_int),
                ("uuid", c_uint8*16))


class SensorInfo(ifxStructure):
    """Sensor info (ifx_Radar_Sensor_Info_t)"""
    _fields_ = (('description', c_char_p),
                ('min_rf_frequency_Hz', c_uint64),
                ('max_rf_frequency_Hz', c_uint64),
                ('num_tx_antennas', c_uint8),
                ('num_rx_antennas', c_uint8),
                ('max_tx_power', c_uint8),
                ('num_temp_sensors', c_uint8),
                ('interleaved_rx', c_uint8),
                ('device_id', c_uint64))


class FirmwareInfo(ifxStructure):
    """Firmware information (ifx_Firmware_Info_t)"""
    _fields_ = (('description', c_char_p),
                ('version_major', c_uint16),
                ('version_minor', c_uint16),
                ('version_build', c_uint16),
                ('extended_version', c_char_p))


class ShieldInfo(ifxStructure):
    """Shield information (ifx_RF_Shield_Info_t)"""
    _fields_ = (('type', c_uint16),)
