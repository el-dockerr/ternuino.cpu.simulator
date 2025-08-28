@echo off
REM GitHub Actions build script for Windows (Debug mode)
REM This script compiles the Ternuino CPU without requiring make
REM Using debug flags: -g -O0

echo Building Ternuino CPU (Debug Mode)...

REM Clean previous build
if exist build rmdir /s /q build
mkdir build
mkdir build\obj

echo Compiling source files with debug info...

REM Compile each source file with debug flags
gcc -Wall -Wextra -std=c99 -g -O0 -Iinclude -c src/main.c -o build/obj/main.o
if errorlevel 1 (
    echo Error compiling main.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -g -O0 -Iinclude -c src/ternuino.c -o build/obj/ternuino.o
if errorlevel 1 (
    echo Error compiling ternuino.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -g -O0 -Iinclude -c src/assembler.c -o build/obj/assembler.o
if errorlevel 1 (
    echo Error compiling assembler.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -g -O0 -Iinclude -c src/tritlogic.c -o build/obj/tritlogic.o
if errorlevel 1 (
    echo Error compiling tritlogic.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -g -O0 -Iinclude -c src/tritarith.c -o build/obj/tritarith.o
if errorlevel 1 (
    echo Error compiling tritarith.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -g -O0 -Iinclude -c src/tritword.c -o build/obj/tritword.o
if errorlevel 1 (
    echo Error compiling tritword.c
    exit /b 1
)

echo Linking executable...

REM Link all object files into the final executable
gcc build/obj/*.o -o build/ternuino.exe
if errorlevel 1 (
    echo Error linking executable
    exit /b 1
)

echo Build completed successfully!
echo Executable: build/ternuino.exe

REM Verify the executable was created
if exist build\ternuino.exe (
    echo Build verification: SUCCESS
    dir build\ternuino.exe
) else (
    echo Build verification: FAILED
    exit /b 1
)