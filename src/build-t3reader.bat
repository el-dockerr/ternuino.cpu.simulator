@echo off
REM Build T3 Reader Utility
REM This script builds the t3reader.exe utility separately

setlocal enabledelayedexpansion

echo Building T3 Reader Utility...
echo.

REM Check if GCC is available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: GCC not found in PATH
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Compiler settings
set CC=gcc
set CFLAGS=-Wall -Wextra -std=c99 -O2 -Iinclude
set TARGET=build\t3reader.exe

echo Compiling T3 Reader...
%CC% %CFLAGS% src\t3reader.c src\ternio.c -o %TARGET%
if !errorlevel! neq 0 (
    echo Error compiling T3 Reader
    exit /b 1
)

echo.
echo Build successful! T3 Reader created: %TARGET%

REM Test that executable was created
if exist "%TARGET%" (
    echo ✅ T3 Reader build completed successfully
) else (
    echo ❌ T3 Reader build failed - executable not found
    exit /b 1
)
