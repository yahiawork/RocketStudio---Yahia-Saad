@echo off
setlocal
cd /d "%~dp0"
if not exist build mkdir build
cd build
cmake -G Ninja ../cpp
if errorlevel 1 exit /b 1
ninja
if errorlevel 1 exit /b 1
RocketStudio.exe %*
