@echo off
setlocal
echo ============================================
echo   GW2Radial - Build and Deploy
echo ============================================
echo.

set VCPKG_ROOT=C:\dev\vcpkg
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64;%PATH%
cd /d "%~dp0"

echo Step 1/2: Building project...
echo.
msbuild.exe GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ============================================
    echo   Build FAILED!
    echo ============================================
    echo.
    echo Check the errors above and fix them before deploying.
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Build Succeeded!
echo ============================================
echo.
echo Step 2/2: Deploying to Guild Wars 2...
echo.

call "%~dp0deploy_to_gw2.bat"
