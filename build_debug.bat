@echo off
setlocal
set VCPKG_ROOT=C:\dev\vcpkg
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64;%PATH%
cd /d "%~dp0"
echo ===== Building GW2Radial Debug =====
msbuild.exe GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:normal > build_output.txt 2>&1
type build_output.txt
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===== Build Succeeded! =====
    if exist bin\x64\Debug\gw2addon_gw2radial.dll (
        echo DLL created at: bin\x64\Debug\gw2addon_gw2radial.dll
    )
) else (
    echo.
    echo ===== Build Failed with error code %ERRORLEVEL% =====
)
