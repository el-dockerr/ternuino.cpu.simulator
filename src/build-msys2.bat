@echo off
REM Ternuino CPU Simulator - C Version
REM MSYS2-specific build script

setlocal enabledelayedexpansion

echo MSYS2 Build Script for Ternuino CPU Simulator
echo.

REM Common MSYS2 installation paths
set MSYS2_PATHS=C:\msys64;D:\msys64;%USERPROFILE%\msys64

REM Find MSYS2 installation
set MSYS2_ROOT=
for %%p in (%MSYS2_PATHS%) do (
    if exist "%%p\msys2.exe" (
        set MSYS2_ROOT=%%p
        goto :found_msys2
    )
)

echo Error: MSYS2 installation not found in common locations.
echo Please ensure MSYS2 is installed and try again.
echo Download from: https://www.msys2.org/
pause
exit /b 1

:found_msys2
echo Found MSYS2 at: %MSYS2_ROOT%

REM Check for GCC in different MSYS2 environments
set GCC_PATH=
set ENV_NAME=

REM Check UCRT64 (recommended)
if exist "%MSYS2_ROOT%\ucrt64\bin\gcc.exe" (
    set GCC_PATH=%MSYS2_ROOT%\ucrt64\bin
    set ENV_NAME=UCRT64
    goto :found_gcc
)

REM Check MINGW64
if exist "%MSYS2_ROOT%\mingw64\bin\gcc.exe" (
    set GCC_PATH=%MSYS2_ROOT%\mingw64\bin
    set ENV_NAME=MINGW64
    goto :found_gcc
)

REM Check CLANG64
if exist "%MSYS2_ROOT%\clang64\bin\gcc.exe" (
    set GCC_PATH=%MSYS2_ROOT%\clang64\bin
    set ENV_NAME=CLANG64
    goto :found_gcc
)

echo Error: GCC not found in MSYS2 installation.
echo Please install GCC by opening MSYS2 terminal and running:
echo   pacman -S mingw-w64-ucrt-x86_64-gcc
echo   pacman -S mingw-w64-ucrt-x86_64-make
pause
exit /b 1

:found_gcc
echo Found GCC in %ENV_NAME% environment: %GCC_PATH%
set "PATH=%GCC_PATH%;%PATH%"

REM Test GCC
gcc --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: GCC is not working properly
    pause
    exit /b 1
)

echo GCC is working. Building project...
echo.

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
        pause
        exit /b 1
    )
)

REM Link the executable
echo Linking executable...
%CC% build\obj\*.o -o %TARGET%
if %errorlevel% neq 0 (
    echo Error linking executable
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build successful!
echo ========================================
echo Executable created: %TARGET%
echo Using %ENV_NAME% environment from MSYS2
echo.
echo To run the program:
echo   %TARGET%
echo.

REM Copy some example programs if they don't exist
if not exist "programs" (
    mkdir programs
    echo Created programs directory. Copy .asm files here to test.
) else (
    echo Found programs directory with the following files:
    dir /b programs\*.asm 2>nul
    if %errorlevel% neq 0 (
        echo   No .asm files found in programs directory.
    ) else (
        echo.
        echo You can now run: %TARGET%
    )
)

echo.
echo Note: If you want to use GCC from Windows Command Prompt in the future,
echo add this to your Windows PATH environment variable:
echo   %GCC_PATH%

pause
