@echo off
REM =====================================================
REM Build Script for SmartCity Project using Qt .pro file
REM =====================================================

REM Set Qt paths
set QT_PATH=C:\Qt\6.7.3\mingw_64
set QT_BIN=%QT_PATH%\bin
set MINGW_PATH=C:\Qt\Tools\mingw1310_64\bin

REM Add paths to environment
set PATH=%QT_BIN%;%MINGW_PATH%;%PATH%

REM Create build directory if it doesn't exist
if not exist "build_pro" mkdir build_pro
cd build_pro

REM Clean previous build
echo Cleaning previous build...
del /Q *.o *.a Makefile* 2>nul

REM Run qmake to generate Makefile
echo Running qmake...
qmake ..\projetsmartcity.pro -spec win32-g++

REM Build the project
echo Building project...
mingw32-make

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo.
    echo =====================================================
    echo Build SUCCESSFUL!
    echo Executable: build_pro\Projet smartcity.exe
    echo =====================================================
    pause
) else (
    echo.
    echo =====================================================
    echo Build FAILED!
    echo Check errors above.
    echo =====================================================
    pause
    exit /b 1
)
