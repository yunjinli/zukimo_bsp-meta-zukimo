@echo off

set "wd=%~dp0"
cd %wd%

rem you might need to adjust the path to cmake
set cmake="c:\Program Files\CMake\bin\cmake.exe"
set build_dir=build_release
set archive=rbb_flash_mcu7_x64.zip
set zip="%wd%\..\..\3rd_party\tools\7-Zip\7z.exe"


IF EXIST %build_dir% (
    rmdir /q /s %build_dir%
)

IF EXIST %archive% (
    rem file exists
    del %archive%
)

mkdir %build_dir%
cd %build_dir%

rem Use toolset v141 of Visual Studio 2017 for compatibility. If we used the
rem toolset of Visual Studio 2019, the generated library would crash when
rem used with Visual Studio 2017.
%cmake% -G "Visual Studio 16 2019" -T v141 -DCMAKE_BUILD_TYPE=Release .. || goto :error
%cmake% --build . --target flash_rbb_mcu7 --config Release               || goto :error
%cmake% --build . --target flash_rbb_mcu7 --config Debug                 || goto :error

cd %wd%

%zip% a %archive% ..\..\3rd_party\libs\bossa\LICENSE_BOSSA_BSD3.txt .\build_release\Release\flash_rbb_mcu7.dll .\build_release\Debug\flash_rbb_mcu7d.dll .\build_release\Release\flash_rbb_mcu7.lib .\build_release\Debug\flash_rbb_mcu7d.lib .\src\FlashRadarBaseboardMCU7.h
goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%