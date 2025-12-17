@echo off
echo ===============================================
echo   Building GW2Radial with Action Chain Test
echo ===============================================
echo.

REM Set up environment
set VCPKG_ROOT=C:\dev\vcpkg
set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"

cd /d "%~dp0"

echo Checking for MSBuild...
if not exist "%MSBUILD_PATH%" (
    echo ERROR: MSBuild not found at: %MSBUILD_PATH%
    echo Please run this from Developer Command Prompt for VS 2022
    pause
    exit /b 1
)

echo MSBuild found!
echo.
echo Starting build (Debug configuration)...
echo This may take 10-30 minutes on first build due to vcpkg dependencies.
echo.

"%MSBUILD_PATH%" GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:normal /fl /flp:LogFile=build_detailed.log;Verbosity=normal

set BUILD_RESULT=%ERRORLEVEL%

if %BUILD_RESULT% EQU 0 (
    echo.
    echo ===============================================
    echo   BUILD SUCCEEDED!
    echo ===============================================
    echo.

    if exist "bin\x64\Debug\gw2addon_gw2radial.dll" (
        echo DLL created at: bin\x64\Debug\gw2addon_gw2radial.dll
        dir "bin\x64\Debug\gw2addon_gw2radial.dll" | findstr /C:"gw2addon"
        echo.
        echo Ready to deploy! Run: deploy_to_gw2.bat
    ) else (
        echo WARNING: Build succeeded but DLL not found!
    )
) else (
    echo.
    echo ===============================================
    echo   BUILD FAILED (Error Code: %BUILD_RESULT%)
    echo ===============================================
    echo.
    echo See build_detailed.log for full details.
    echo.
    echo Last 30 lines of log:
    powershell -Command "if (Test-Path build_detailed.log) { Get-Content build_detailed.log -Tail 30 }"
)

echo.
pause
