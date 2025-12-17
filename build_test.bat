@echo off
setlocal
echo ===== Testing Action Chain Build =====
set VCPKG_ROOT=C:\dev\vcpkg
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64;%PATH%
cd /d "%~dp0"

echo Building with MSBuild...
msbuild.exe GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo /fl /flp:LogFile=build_log.txt

echo.
if %ERRORLEVEL% EQU 0 (
    echo ===== Build Succeeded! =====
    if exist bin\x64\Debug\gw2addon_gw2radial.dll (
        echo DLL created successfully at: bin\x64\Debug\gw2addon_gw2radial.dll
        dir bin\x64\Debug\gw2addon_gw2radial.dll
    )
) else (
    echo ===== Build FAILED with error code %ERRORLEVEL% =====
    if exist build_log.txt (
        echo.
        echo Last 50 lines of build log:
        powershell -Command "Get-Content build_log.txt -Tail 50"
    )
)

pause
