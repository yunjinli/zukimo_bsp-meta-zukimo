# Flashing

This directoy contains a library and a program to flash firmware to a
RadarBaseboardMCU7 board. The library depends on BOSSA
(https://github.com/shumatech/BOSSA) which is under BSD3 license (see
directory 3rd_party\libs).

## Requirements for Compiling

You can compile the program and the library using the provided cmake script.
You will need following tools:
- cmake
- Visual Studio 2019 (some specific toolsets are needed, see below)

Make sure that you have the toolsets v140 and v141 installed. You can install
the toolsets using the Visual Studio Installer. Open the Visual Studio
Installer, click on "Modify" for Visual Studio, and check in the tab
"Idividual components" the boxes for
- MSVC v140 - VS 2015 C++ build tools (v14.00)
- MSVC v141 - VS 2017 C++ build tools (v14.16)

## Compiling

You can use cmake to generate a Visual Studio solution or open the project
directly in Visual Studio.

If you need to create a package for Sii, please use the provided batch script
"build_flash_library_package.bat". The script assumes that cmake is installed
in C:\Program Files\cmake. If this is not the case, you have to adjust the
path. The script will generate the zip archive "rbb_flash_mcu7_x64.zip".

For Sii it is important that an old toolset (compatible to Visual Studio 2017)
is used and that the library uses the dynamic version of the runtime library.
The cmake script takes care of this.


## Content

- src: Source code of library and program.
- CMakeLists.txt: File to build the project using cmake.
- build_flash_library_package.bat: Batch file to generate a zip archive of the
  library including .dll and .lib files for release and debug mode, header
  file, and license. The script requires Visual Studio 2019 and cmake. The
  library is compiled for x64, the debug version contains a suffix d.
- UpdateFirmware.bat: Batch script that shows a dialog chooser and then calls
  flashtool.exe. flashtool.exe must be in the same directory as the batch file.

Start Visual Studio Installer, Modify, Invidividual components tab,
MSVC v140 - VS 2015 C++ build tools (v14.00)
MSVC v141 - VS 2017 C++ build tools (v14.16)