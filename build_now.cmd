@echo off
cd /d "%~dp0"
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
