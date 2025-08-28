@echo off
REM Simple Windows build script for GitHub Actions
REM This script builds without requiring make

setlocal enabledelayedexpansion

echo Building Ternuino CPU Simulator (GitHub Actions)
echo.

REM Check if GCC is available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: GCC not found in PATH
    exit /b 1
)

REM Create build directory structure
if not exist "build" mkdir build
if not exist "build\obj" mkdir build\obj

REM Remove old build files
del /q build\obj\*.o 2>nul
del /q build\ternuino.exe 2>nul

REM Compiler settings
set CC=gcc
set CFLAGS=-Wall -Wextra -std=c99 -O2 -Iinclude
set TARGET=build\ternuino.exe

echo Compiling source files...
echo   Compiling src\main.c...
%CC% %CFLAGS% -c src\main.c -o build\obj\main.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\ternuino.c...
%CC% %CFLAGS% -c src\ternuino.c -o build\obj\ternuino.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\assembler.c...
%CC% %CFLAGS% -c src\assembler.c -o build\obj\assembler.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\tritlogic.c...
%CC% %CFLAGS% -c src\tritlogic.c -o build\obj\tritlogic.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\tritarith.c...
%CC% %CFLAGS% -c src\tritarith.c -o build\obj\tritarith.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\tritword.c...
%CC% %CFLAGS% -c src\tritword.c -o build\obj\tritword.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\ternio.c...
%CC% %CFLAGS% -c src\ternio.c -o build\obj\ternio.o
if !errorlevel! neq 0 exit /b 1

echo   Compiling src\devices.c...
%CC% %CFLAGS% -c src\devices.c -o build\obj\devices.o
if !errorlevel! neq 0 exit /b 1

echo Linking executable...
%CC% build\obj\*.o -o %TARGET%
if !errorlevel! neq 0 exit /b 1

echo.
echo Build successful! Executable created: %TARGET%

REM Test that executable was created
if exist "%TARGET%" (
    echo ✅ Build completed successfully
    dir build\ternuino.exe
) else (
    echo ❌ Build failed - executable not found
    exit /b 1
)