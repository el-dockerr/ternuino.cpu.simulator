@echo off
REM Ternuino CPU Simulator - C Version
REM Windows build script

setlocal enabledelayedexpansion

REM Check if we have GCC available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo GCC not found in PATH. Checking for MSYS2 installation...
    
    REM Check common MSYS2 installation paths
    set MSYS2_PATHS=C:\msys64\mingw64\bin;C:\msys64\ucrt64\bin;C:\msys64\clang64\bin;D:\msys64\mingw64\bin;D:\msys64\ucrt64\bin
    set GCC_FOUND=0
    
    for %%p in (%MSYS2_PATHS%) do (
        if exist "%%p\gcc.exe" (
            echo Found GCC in %%p
            set "PATH=%%p;%PATH%"
            set GCC_FOUND=1
            goto :gcc_found
        )
    )
    
    :gcc_found
    if !GCC_FOUND! equ 0 (
        echo Error: GCC compiler not found. Please:
        echo   1. If you have MSYS2 installed, open MSYS2 terminal and run:
        echo      pacman -S mingw-w64-ucrt-x86_64-gcc
        echo   2. Or add MSYS2 bin directory to your Windows PATH
        echo   3. Or install MinGW-w64: https://www.mingw-w64.org/downloads/
        echo   4. Or install Visual Studio with C++ support
        echo.
        echo Common MSYS2 paths to add to PATH:
        echo   C:\msys64\mingw64\bin
        echo   C:\msys64\ucrt64\bin
        pause
        exit /b 1
    )
)

REM Create build directory structure
if not exist "build" mkdir build
if not exist "build\obj" mkdir build\obj

REM Compiler settings
set CC=gcc
set CFLAGS=-Wall -Wextra -std=c99 -O2 -Iinclude
set SOURCES=src\main.c src\ternuino.c src\assembler.c src\tritlogic.c src\tritarith.c src\tritword.c src\ternio.c
set TARGET=build\ternuino.exe

echo Building Ternuino CPU Simulator...

REM Compile all source files
echo Compiling source files...
for %%f in (%SOURCES%) do (
    echo   Compiling %%f...
    %CC% %CFLAGS% -c %%f -o build\obj\%%~nf.o
    if !errorlevel! neq 0 (
        echo Error compiling %%f
        exit /b 1
    )
)

REM Link the executable
echo Linking executable...
%CC% build\obj\*.o -o %TARGET%
if %errorlevel% neq 0 (
    echo Error linking executable
    exit /b 1
)

echo.
echo Build successful! Executable created: %TARGET%
echo.
echo To run the program:
echo   %TARGET%
echo.
echo Or copy some .asm files to the programs\ directory and run interactively.

REM Copy some example programs if they don't exist
if not exist "programs" (
    mkdir programs
    echo Created programs directory. Copy .asm files here to test.
) else (
    echo Found programs directory with the following files:
    dir /b programs\*.asm 2>nul
    if %errorlevel% neq 0 (
        echo   No .asm files found in programs directory.
    )
)

pause
