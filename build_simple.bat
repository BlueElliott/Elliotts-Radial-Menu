@echo off
setlocal
set VCPKG_ROOT=C:\dev\vcpkg
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64;%PATH%
cd /d "%~dp0"
echo Building GW2Radial...
msbuild.exe GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded!
) else (
    echo Build failed with error code %ERRORLEVEL%
)
