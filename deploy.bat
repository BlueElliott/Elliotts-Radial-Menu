@echo off
echo ========================================
echo GW2Radial - Deploy to Guild Wars 2
echo ========================================
echo.

REM Check if GW2 is running
tasklist /FI "IMAGENAME eq Gw2-64.exe" 2>NUL | find /I /N "Gw2-64.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo ERROR: Guild Wars 2 is currently running!
    echo Please close GW2 before deploying.
    echo.
    pause
    exit /b 1
)

echo Deploying DLL...
echo From: bin\x64\Debug\gw2addon_gw2radial.dll
echo To:   C:\Program Files\Guild Wars 2\addons\gw2radial\
echo.

copy /Y "bin\x64\Debug\gw2addon_gw2radial.dll" "C:\Program Files\Guild Wars 2\addons\gw2radial\gw2addon_gw2radial.dll"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo SUCCESS! DLL deployed successfully.
    echo ========================================
    echo.
    echo You can now launch Guild Wars 2.
    echo.
    echo To test the Chat Commands feature:
    echo   1. Launch GW2
    echo   2. Press your radial menu keybind
    echo   3. Click the settings icon
    echo   4. Scroll to the bottom
    echo   5. Look for "Chat Commands" section
    echo.
) else (
    echo.
    echo ========================================
    echo ERROR: Deployment failed!
    echo ========================================
    echo.
    echo Make sure you have administrator privileges.
    echo.
)

pause
