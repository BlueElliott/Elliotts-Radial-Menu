@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
cd /d "%~dp0"
msbuild GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal
