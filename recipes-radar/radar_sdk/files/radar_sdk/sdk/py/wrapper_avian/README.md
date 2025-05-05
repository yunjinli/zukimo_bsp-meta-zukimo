# Avian Python Wrapper for Radar SDK

## Prerequisite

Install the Python wheel using pip. See the Radar SDK documentation for
more information.

## Description:

The Avian Python wrapper allows to acces the Avian radar sensors like BGT60TR13C
or BGT60ATR24 from Python.

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
>>> import ifxAvian
>>> help(ifxAvian)
```

## Examples

The file example.py contains a simple example code which illustrates how to
use the wrapper to connect to a radar device and read data from it.

The file example_cw.py contains a simple example code which illustrates how to
use the wrapper to connect to a radar device and read data from it
in Constant Wave mode.

The file example_recording.py contains a simple example code which illustrates how to
use the wrapper to create and use a device handle created from a saved
Common File Format recording.

Additional application examples can be found in the folder apps/py/examples. For
further details please refer to the corresponding ReadMe.txt file.