@echo off
setlocal
echo ============================================
echo   GW2Radial - Restore Backup
echo ============================================
echo.

set "ADDON_DIR=C:\Program Files\Guild Wars 2\addons\gw2radial"
set "BACKUP_DLL=%ADDON_DIR%\gw2addon_gw2radial_BACKUP.dll"
set "TARGET_DLL=%ADDON_DIR%\gw2addon_gw2radial.dll"

REM Check if backup exists
if not exist "%BACKUP_DLL%" (
    echo [ERROR] Backup not found!
    echo Expected: %BACKUP_DLL%
    echo.
    echo No backup available. You may need to reinstall GW2Radial
    echo or download from: https://github.com/Friendly0Fire/GW2Radial/releases
    pause
    exit /b 1
)

echo Backup found: %BACKUP_DLL%
echo Restoring to:  %TARGET_DLL%
echo.
echo This will restore your previous stable version.
pause

REM Restore backup
copy /Y "%BACKUP_DLL%" "%TARGET_DLL%" >nul
if errorlevel 1 (
    echo [ERROR] Failed to restore backup!
    echo.
    echo Possible reasons:
    echo   1. Guild Wars 2 is running (close it first)
    echo   2. Insufficient permissions (run as Administrator)
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Backup Restored Successfully!
echo ============================================
echo.
echo Your stable version has been restored.
echo You can now launch Guild Wars 2.
echo.
pause
