@echo off
setlocal
echo ============================================
echo   GW2Radial - Deploy to Game
echo ============================================
echo.

set "GW2_DIR=C:\Program Files\Guild Wars 2"
set "ADDON_DIR=%GW2_DIR%\addons\gw2radial"
set "SOURCE_DLL=%~dp0bin\x64\Debug\gw2addon_gw2radial.dll"
set "SOURCE_PDB=%~dp0bin\x64\Debug\gw2addon_gw2radial.pdb"

REM Check if source DLL exists
if not exist "%SOURCE_DLL%" (
    echo [ERROR] Build output not found!
    echo Expected: %SOURCE_DLL%
    echo.
    echo Please build the project first:
    echo   - Open GW2Radial.sln in Visual Studio
    echo   - Press F7 to build
    echo   - Or run: build_debug.bat
    echo.
    pause
    exit /b 1
)

REM Check if GW2 directory exists
if not exist "%GW2_DIR%" (
    echo [ERROR] Guild Wars 2 not found at: %GW2_DIR%
    echo.
    echo Please edit this script and set the correct path.
    pause
    exit /b 1
)

REM Check if addon directory exists
if not exist "%ADDON_DIR%" (
    echo [WARNING] GW2Radial addon folder not found!
    echo Creating: %ADDON_DIR%
    mkdir "%ADDON_DIR%"
    if errorlevel 1 (
        echo [ERROR] Failed to create directory. Run as Administrator?
        pause
        exit /b 1
    )
)

echo Source DLL: %SOURCE_DLL%
echo Target:     %ADDON_DIR%\gw2addon_gw2radial.dll
echo.

REM Create backup if DLL exists
if exist "%ADDON_DIR%\gw2addon_gw2radial.dll" (
    echo Creating backup...
    copy /Y "%ADDON_DIR%\gw2addon_gw2radial.dll" "%ADDON_DIR%\gw2addon_gw2radial_BACKUP.dll" >nul
    if errorlevel 1 (
        echo [ERROR] Failed to create backup. Is GW2 running?
        echo Please close Guild Wars 2 and try again.
        pause
        exit /b 1
    )
    echo Backup created: gw2addon_gw2radial_BACKUP.dll
)

REM Copy DLL
echo Copying DLL...
copy /Y "%SOURCE_DLL%" "%ADDON_DIR%\gw2addon_gw2radial.dll" >nul
if errorlevel 1 (
    echo [ERROR] Failed to copy DLL!
    echo.
    echo Possible reasons:
    echo   1. Guild Wars 2 is running (close it first)
    echo   2. Insufficient permissions (run as Administrator)
    echo   3. File is locked by another program
    pause
    exit /b 1
)

echo DLL deployed successfully!

REM Copy PDB if exists (debug symbols)
if exist "%SOURCE_PDB%" (
    echo Copying debug symbols...
    copy /Y "%SOURCE_PDB%" "%ADDON_DIR%\gw2addon_gw2radial.pdb" >nul 2>&1
)

echo.
echo ============================================
echo   Deployment Complete!
echo ============================================
echo.
echo Your custom build is now in GW2!
echo.
echo Next steps:
echo   1. Launch Guild Wars 2
echo   2. Check: %GW2_DIR%\gw2radial.log
echo   3. Press Shift+Alt+M in-game to verify
echo.
echo To restore backup if needed:
echo   copy "%ADDON_DIR%\gw2addon_gw2radial_BACKUP.dll" "%ADDON_DIR%\gw2addon_gw2radial.dll"
echo.
pause
