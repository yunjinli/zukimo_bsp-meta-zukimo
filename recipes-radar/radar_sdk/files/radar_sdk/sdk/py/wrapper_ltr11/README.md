# DopplerLTR11 Python Wrapper for Radar SDK

## Prerequisite

Install the Python wheel using pip. See the Radar SDK documentation for
more information.

## Description:

The Avian Python wrapper allows to acces the BGT60LTR11AIP Doppler Radar sensor
from Python.

The wrapper has been tested with Python versions 3.7, 3.8, and 3.9 and is available
on following platforms:

- Windows 10 (64bit)
- Linux (Ubuntu 20.04, 64bit)
- Raspberry Pi (Raspbian Buster, 32bit)

Notes:
- The wrapper requires a 64-bit Python installation. Following error message
  indicates that you are using a 32-bit Python installation:
  "OSError: [WinError 193] %1 is not a valid Win32 application".

## API and documentation

Functions of the Radar SDK with suffix `_create` correspond to constructors of classes,
the corresponding `_destroy` functions are implicitly called by the corresponding destructors.
All SDK error codes are mapped to a Python exception.

A full documentation is available as docstrings. You can access the
documentation within Python via:
```
>>> import ifxDopplerLTR11
>>> help(ifxDopplerLTR11)
```

## Examples

The file example.py contains a simple example code which illustrates how to
use the wrapper to connect to a BGT60LTR11 and read data from it.
