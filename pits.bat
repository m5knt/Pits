@echo off
pushd .
cd /d %~dp0
call :MAIN %*
popd
exit /b

:MAIN
if "%1" == "clean" goto :CLEAN
if "%1" == "build" goto :BUILD
if "%1" == "ide" goto :IDE
if "%1" == "idesu" goto :IDESU
if "%1" == "help" goto :HELP
if "%1" == "" goto :BUILD
echo.Illigal Command "%1"
exit /b 1

:HELP
echo.[usage] ./pits [clean^|build^|ide^|idesu^|help]
echo.help  ... this message
echo.clean ... clean cmake out
echo.build ... cmake
echo.ide   ... launch ide
echo.idesu ... launch ide (runas admin)
exit /b

:CLEAN
rmdir /Q /S build >nul 2>&1
exit /b

:BUILD
if not exist build mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
exit /b

:IDE
start /b build/Pits.sln
exit /b

:IDESU
powershell start-process cmd -ArgumentList '/c start /b %CD%/build/Pits.sln""' -verb runas
exit /b
