@echo off
REM Ternuino CPU Simulator - non-interactive build script
REM
REM Builds build\ternuino.exe (simulator + gate-level layer) and build\t3reader.exe.
REM Sources are picked up automatically from src\*.c and src\gates\*.c, so new
REM files do not need to be registered here.
REM
REM Usage:
REM   build-all.bat          build everything
REM   build-all.bat test     build, then run the gate-level verification/report
REM   build-all.bat clean    remove the build directory
REM
REM Requires gcc in PATH (MinGW-w64 / MSYS2). Exits non-zero on any error.

setlocal enabledelayedexpansion
cd /d "%~dp0"

if /i "%~1"=="clean" (
    if exist build rmdir /s /q build
    echo Cleaned build directory
    exit /b 0
)

where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: gcc not found in PATH. Install MinGW-w64 or MSYS2 and add its bin directory to PATH.
    exit /b 1
)

set CC=gcc
set CFLAGS=-Wall -Wextra -std=c99 -O2 -Iinclude
set OBJDIR=build\obj

if not exist "%OBJDIR%" mkdir "%OBJDIR%"

set OBJECTS=
for %%f in (src\*.c src\gates\*.c) do (
    if /i not "%%~nf"=="t3reader" (
        echo   Compiling %%f
        %CC% %CFLAGS% -c %%f -o %OBJDIR%\%%~nf.o
        if !errorlevel! neq 0 (
            echo Error compiling %%f
            exit /b 1
        )
        set OBJECTS=!OBJECTS! %OBJDIR%\%%~nf.o
    )
)

echo   Linking build\ternuino.exe
%CC% %OBJECTS% -o build\ternuino.exe
if %errorlevel% neq 0 (
    echo Error linking ternuino.exe
    exit /b 1
)

echo   Linking build\t3reader.exe
%CC% %CFLAGS% src\t3reader.c src\ternio.c -o build\t3reader.exe
if %errorlevel% neq 0 (
    echo Error building t3reader.exe
    exit /b 1
)

echo Build successful.

if /i "%~1"=="test" (
    build\ternuino.exe --gate-report
    exit /b !errorlevel!
)

exit /b 0
