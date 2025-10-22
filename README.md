# Ternuino CPU - Ternary Computer Simulator


![Ternuino CPU Wallpaper](assets/ternuino_wallpaper.png)

A ternary (base-3) CPU simulator that demonstrates the advantages of three-state logic over traditional binary systems. It is the virtual Simulation of a planned small Computer that will be build by myself.
The Ternuino CPU uses trits (ternary digits) with values of -1, 0, and +1 instead of binary bits (0, 1).
This program was written by Swen "El Dockerr" Kalski (me) and is in active development. This Repository also contains a beginner friendly introduction of ternary computing what is part of my bachelor thesis in mathematics. 

A friendly, beginner‑oriented guide lives in [`docs/book/`](docs/book/README.md).

For CPU Design I already have a CPU Design tool in place: [ternuino.cpu.visual-desinger](https://github.com/el-dockerr/ternuino.cpu.visual-desinger)*

*yepp - there is a typo. But at least this can be happen. Did you now, that Commodore did these when the build up the 8-bit systems (VIC 20 / C64 ...). They wrote Kernal instead of Kernel. And Google was a typo too. That's a good sign for the future of this project.


```
┌─────────────────────────────────────────────────────────────┐
│                    TERNUINO CPU ARCHITECTURE                │
├─────────────────────────────────────────────────────────────┤
│  REGISTERS           MEMORY              INSTRUCTION SET    │
│  ┌─────────┐        ┌──────────┐        ┌────────────────────────────┐│
│  │ A: trit │        │ 0: instr │        │ MOV  TAND   TSIGN  TSHL3   ││
│  │ B: trit │        │ 1: instr │        │ ADD  TOR    TABS   TSHR3   ││
│  │ C: trit │        │ 2: instr │        │ SUB  TNOT   TCMPR          ││
│  │         │        │    ...   │        │ MUL  JMP    TJN    TJP     ││
│  │ PC: int │        │26: instr │        │ DIV  TJZ    HLT    NOP     ││
│  └─────────┘        └──────────┘        └────────────────────────────┘│
│                                         └──────────────┘    │
│  TERNARY VALUES:  -1 (negative)  0 (neutral)  +1 (positive) │
└─────────────────────────────────────────────────────────────┘
```

![Ternuino CPU Architecture](docs/ternuino-architecture.png)

## What is Ternary Computing?

Ternary computing uses three states instead of two:
- **-1** (negative/false)
- **0** (neutral/unknown)  
- **+1** (positive/true)

This provides a more natural representation for many logical operations and mathematical concepts.

## The Ternuino Book (Docs)
Ternary operations are the subject of my bachelor's thesis in mathematics. Ever since I heard about the Russian Setun computer a few years ago, this topic has been on my mind, and I am fascinated by the fact that we limit ourselves so much by only working with binary computers. That is why I have spun off parts of my bachelor thesis and would like to convince and inspire other makers and interested parties about this technology.


A friendly, beginner‑oriented guide lives in `docs/book/`.

- Start at `docs/book/README.md`
- Chapters cover ternary numbers, logic, TritWord, CPU architecture, assembly, examples, the visualizer, and an FAQ.

## Assembly Language Documentations
- General Ternuino Assembly [`ASSEMBLY.md`](ASSEMBLY.md)
- I/O Operations with Ternuino Assembly [`IO_OPERATIONS.md`](IO_OPERATIONS.md)

## Key Advantages of Ternary Over Binary Systems

### 1. **Information Density**
- **Ternary**: Each trit can represent 3 states, giving log₂(3) ≈ 1.585 bits of information per trit
- **Binary**: Each bit represents only 2 states (1 bit of information)
- **Result**: Ternary is ~58% more information-dense than binary

### 2. **Natural Representation of Signed Numbers**
```
Ternary: -1, 0, +1 (directly represents negative, zero, positive)
Binary:  Requires sign bit + magnitude or two's complement encoding
```

### 3. **Simpler Logic Operations**
The ternary logic operations are more intuitive:
- **TAND (Ternary AND)**: `min(a, b)` - Takes the minimum value
- **TOR (Ternary OR)**: `max(a, b)` - Takes the maximum value  
- **TNOT (Ternary NOT)**: `-a` - Simple negation

### 4. **Balanced Representation**
- Ternary naturally handles positive and negative values symmetrically
- No need for special encoding schemes like two's complement
- Zero has a natural, unique representation

### 5. **More Efficient for Multi-Valued Logic**
- Many real-world problems have three natural states (yes/no/maybe, true/false/unknown)
- Fuzzy logic and probabilistic reasoning map better to ternary
- Database NULL values have a natural representation

### 6. **Computational Efficiency**
- Fewer digits needed to represent the same range of numbers
- Example: Numbers 0-8 in different bases:
  ```
  Decimal: 0, 1, 2, 3, 4, 5, 6, 7, 8
  Binary:  000, 001, 010, 011, 100, 101, 110, 111, 1000 (up to 4 bits)
  Ternary: 00, 0+, +-, +0, ++, +-, +0, ++, +00 (up to 3 trits)
  ```

### Visual Comparison: Binary vs Ternary Logic

![Binary vs Ternary Comparison](docs/binary-vs-ternary.png)

```
BINARY SYSTEM               │  TERNARY SYSTEM
                            │
States: 0, 1                │  States: -1, 0, +1
                            │
Truth Table (AND):          │  Truth Table (TAND):
0 AND 0 = 0                 │  -1 TAND -1 = -1
0 AND 1 = 0                 │  -1 TAND  0 = -1
1 AND 0 = 0                 │  -1 TAND +1 = -1
1 AND 1 = 1                 │   0 TAND  0 =  0
                            │   0 TAND +1 =  0
Encoding signed numbers:    │  +1 TAND +1 = +1
Requires 2's complement     │
or sign bit                 │  Natural signed representation:
                            │  No special encoding needed!
```

## Architecture Overview

### Registers
- **A, B, C**: General-purpose registers (each holds a single trit value)
- **PC**: Program Counter
- **Memory**: 27 memory locations (3³ addressable space)

### Instruction Set

| Instruction | Description | Example |
|-------------|-------------|---------|
| `NOP` | No operation | `("NOP",)` |
| `MOV reg, val` | Move value/register to register | `("MOV", "A", 1)` |
| `ADD reg1, reg2` | Add reg2 to reg1 | `("ADD", "A", "B")` |
| `SUB reg1, reg2` | Subtract reg2 from reg1 | `("SUB", "A", "B")` |
| `MUL reg1, reg2` | Multiply reg1 by reg2 | `("MUL", "A", "B")` |
| `DIV reg1, reg2` | Divide reg1 by reg2 (trunc. to 0) | `("DIV", "A", "B")` |
| `JMP label` | Jump to address | `("JMP", "foo")` |
| `TJZ reg, label` | Jump if reg == 0 | `("TJZ", "A", "foo")` |
| `TJN reg, label` | Jump if reg < 0 | `("TJN", "A", "foo")` |
| `TJP reg, label` | Jump if reg > 0 | `("TJP", "A", "foo")` |
| `TAND reg1, reg2` | Ternary AND (min) | `("TAND", "A", "B")` |
| `TOR reg1, reg2` | Ternary OR (max) | `("TOR", "A", "B")` |
| `TNOT reg` | Ternary NOT (negate) | `("TNOT", "A")` |
| `TSIGN reg` | reg := sign(reg) ∈ {-1,0,+1} | `("TSIGN", "A")` |
| `TABS reg` | reg := abs(reg) | `("TABS", "B")` |
| `TSHL3 reg` | reg := reg * 3 | `("TSHL3", "A")` |
| `TSHR3 reg` | reg := trunc(reg / 3) | `("TSHR3", "A")` |
| `TCMPR reg1, reg2` | reg1 := sign(reg1 - reg2) | `("TCMPR", "A", "B")` |
| `HLT` | Halt execution | `("HLT",)` |
| `LD reg, addr|[REG]` | Load from data memory into reg | `("LD", "A", 5)`, `("LD", "A", ("IND","B"))` |
| `ST reg, addr|[REG]` | Store reg into data memory | `("ST", "A", 7)`, `("ST", "A", ("IND","C"))` |
| `LEA reg, label|addr` | Load effective address into reg | `("LEA", "B", "var")` |

### Ternary Logic Truth Tables

**TAND (min operation)**:
```
  a  |  b  | TAND
 -1  | -1  |  -1
 -1  |  0  |  -1
 -1  | +1  |  -1
  0  | -1  |  -1
  0  |  0  |   0
  0  | +1  |   0
 +1  | -1  |  -1
 +1  |  0  |   0
 +1  | +1  |  +1
```

**TOR (max operation)**:
```
  a  |  b  | TOR
 -1  | -1  |  -1
 -1  |  0  |   0
 -1  | +1  |  +1
  0  | -1  |   0
  0  |  0  |   0
  0  | +1  |  +1
 +1  | -1  |  +1
 +1  |  0  |  +1
 +1  | +1  |  +1
```

### Ternary Arithmetic Operations

The ternary system provides natural and symmetric arithmetic operations:

**Addition/Subtraction**: Works naturally with positive and negative values
```
Ternary addition examples:
 1 + (-1) =  0  (positive + negative = neutral)
-1 + (-1) = -2  (fully symmetric)
 1 +   1  =  2  
```

**Multiplication**: Simple and intuitive
```
Ternary multiplication table:
-1 × -1 =  1  (negative × negative = positive)
-1 ×  0 =  0  (anything × zero = zero)
-1 ×  1 = -1  (negative × positive = negative)
 0 × any =  0
 1 ×  1 =  1
```

**Division**: Handles sign naturally, with safe division by zero
- Division by zero returns 0 (could be configured to halt or set error flag)
- Natural handling of negative results
- Truncation towards zero for fractional results

## Installation Requirements

### What You Need to Install

#### Build
1. Have a modern C++ Compiler available
2. Have Bodge available
3. Run `bodge --platform=your_platform`

#### Windows Users:
**Option 1: MSYS2 (Recommended)**
1. Download MSYS2 from https://www.msys2.org/
2. Install and run MSYS2 terminal
3. Install GCC: `pacman -S mingw-w64-ucrt-x86_64-gcc`
4. Use our `build-msys2.bat` script (auto-detects installation)

**Option 2: MinGW-w64**
1. Download from https://www.mingw-w64.org/downloads/
2. Add to Windows PATH
3. Use `build.bat`

**Option 3: Visual Studio**
1. Install Visual Studio with C++ support
2. Use Developer Command Prompt
3. Use `build.bat`

#### Linux Users:
```bash
# Ubuntu/Debian
sudo apt update && sudo apt install gcc make

# CentOS/RHEL/Fedora  
sudo yum install gcc make
# or: sudo dnf install gcc make
```

#### macOS Users:
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Or via Homebrew
brew install gcc
```

### Available Example Programs
- `logic_demo.asm` - Basic ternary logic operations
- `arithmetic_demo.asm` - Arithmetic operations (ADD, SUB, MUL, DIV)
- `loop_demo.asm` - Loops and jumps with labels
- `fibonacci_demo.asm` - Fibonacci-like sequence calculation
- `ternary_sign_and_branch.asm` - Three-way compare and branching using TSIGN/TCMPR/TJN/TJP
- `ternary_shift_scale.asm` - Base-3 scaling with TSHL3/TSHR3 plus TABS/TSIGN
- `three_way_select_demo.asm` - 3-way decision using sign and ternary branches
- `memory_demo.asm` - LD/ST/LEA and .data/.text with indirect [REG]

## Features

- Complete ternary CPU simulation with all original instructions
- Assembly language parser and assembler
- Ternary logic operations (TAND, TOR, TNOT)
- Ternary arithmetic operations (TSIGN, TABS, TSHL3, TSHR3, TCMPR)
- Memory operations (LD, ST, LEA) with direct and indirect addressing
- Control flow instructions (JMP, TJZ, TJN, TJP)
- TritWord representation for ternary numbers
- Interactive program selection
- Command-line program execution

## Requirements

### Windows
- **GCC Compiler** (one of the following):
  - **MinGW-w64** (recommended): https://www.mingw-w64.org/downloads/
  - **MSYS2**: https://www.msys2.org/
  - **TDM-GCC**: https://jmeubank.github.io/tdm-gcc/
  - **Visual Studio** with C++ support (use Developer Command Prompt)

### Linux/macOS
- **GCC** or **Clang** compiler
- **Make** utility

## Installation

### Windows

1. **Install a C compiler** (if not already installed):
   - Download and install MinGW-w64 or MSYS2
   - Add the compiler to your PATH environment variable

2. **Build the project**:
   ```cmd
   cd c-version
   build.bat
   ```

3. **Run the simulator**:
   ```cmd
   build\ternuino.exe
   ```

### Linux/macOS

1. **Install GCC** (if not already installed):
   ```bash
   # Ubuntu/Debian
   sudo apt update
   sudo apt install gcc make
   
   # macOS (install Xcode Command Line Tools)
   xcode-select --install
   
   # Or install via Homebrew
   brew install gcc
   ```

2. **Build the project**:
   ```bash
   cd c-version
   make
   ```

3. **Run the simulator**:
   ```bash
   ./build/ternuino
   ```

## Usage

### Interactive Mode
Run the simulator without arguments to enter interactive mode:

```bash
./build/ternuino
```

This will:
1. List all `.asm` files in the `programs/` directory
2. Allow you to select a program to run
3. Display the execution results

### Command Line Mode
Run a specific program directly:

```bash
./build/ternuino path/to/program.asm
```

## Building from Source

### Windows (Manual)
```cmd
gcc -Wall -Wextra -std=c99 -O2 -Iinclude -c src/*.c
gcc *.o -o ternuino.exe
```

### Unix/Linux (Manual)
```bash
gcc -Wall -Wextra -std=c99 -O2 -Iinclude src/*.c -o ternuino
```

## Performance

The C version provides significantly better performance compared to the Python version:
- Faster program execution
- Lower memory usage
- Instant startup time
- Better suited for larger programs and extended simulations

## Compatibility

This C implementation maintains full compatibility with the Python version:
- Same assembly language syntax
- Same instruction set
- Same execution behavior
- Same program output format

You can use the same `.asm` program files with both versions.

## Troubleshooting

### "GCC not found" error on Windows
- Install MinGW-w64 or MSYS2
- Add the compiler's bin directory to your PATH environment variable
- Restart your command prompt/terminal

### Build errors
- Ensure you have a C99-compatible compiler
- Check that all header files are present in the `include/` directory
- Verify that all source files are present in the `src/` directory

### Runtime errors
- Ensure the `programs/` directory exists
- Check that your `.asm` files have correct syntax
- Verify file permissions for reading assembly files

## Example Programs

Copy the `.asm` files from the parent `programs/` directory to test the simulator:

```bash
# Copy example programs
cp ../programs/*.asm programs/

# Run a specific program
./build/ternuino programs/fibonacci_demo.asm
```


### Assembly Programs
Place your `.asm` assembly files in the `programs/` directory. The simulator supports all the same assembly language features as the Python version:

- All arithmetic operations: `ADD`, `SUB`, `MUL`, `DIV`
- Ternary logic: `TAND`, `TOR`, `TNOT`
- Ternary arithmetic: `TSIGN`, `TABS`, `TSHL3`, `TSHR3`, `TCMPR`
- Memory operations: `LD`, `ST`, `LEA` with `[A]`, `[B]`, `[C]` indirect addressing
- Control flow: `JMP`, `TJZ`, `TJN`, `TJP`, `HLT`
- Data sections with `.data`, `.word`, `.zero` directives
- Labels for jumps and memory addresses

## Assembly Language

The Ternuino now supports a simple assembly language! Write your programs in `.asm` files and place them in the `programs/` directory. 

### Example Assembly Program
```assembly
# Simple ternary logic demonstration
MOV A, 1        # A = +1 (positive)
MOV B, -1       # B = -1 (negative)
TAND A, B       # A = min(+1, -1) = -1
TNOT A          # A = -(-1) = +1
HLT             # Stop execution
```

See `ASSEMBLY.md` for complete language reference and examples.

## Practical Applications

1. **Database Systems**: Natural representation of NULL values
2. **Fuzzy Logic**: Three-state logic for uncertain reasoning
3. **Signal Processing**: Representation of positive, negative, and zero signals
4. **Control Systems**: On/Off/Standby states
5. **Machine Learning**: Ternary neural networks for efficient computation

## Historical Context

Ternary computers were actually built and used:
- **Setun** (1958): Soviet ternary computer, proved ternary systems are practical
- **Setun 70** (1970s): Improved version with better performance
- Modern research continues into ternary computing for quantum systems and neural networks

## Future Enhancements

- [x] Memory management instructions (LD/ST/LEA, .data/.text, [REG])
- [x] Arithmetic operations (SUB, MUL, DIV)
- [X] Change codebase to C
- [X] I/O operations
- [X] Interrupt handling
- [x] Assembler for human-readable assembly language
- [ ] Debugger with step-by-step execution
- [ ] Performance comparison tools vs. binary systems
- [ ] Implementation of a simple logic on hardware

## References

### Books
- "The Art of Computer Programming" - Knuth, D.E. - discusses ternary number systems
- "Ternary Logic and Circuits" - John Gregg
- "Computing Science: Third Base" - Hayes, Brian

### Web References
- [cabinetmagazine.org - DIALECTICAL COMPUTERS AND COLD WAR METAPHYSICS](https://cabinetmagazine.org/issues/65/weatherby.php)
- [Earl T. Campbell - The Setun Computer](https://earltcampbell.com/2014/12/29/the-setun-computer/)
- [Hayes, Brian - Computing Science: Third Base](https://www.jstor.org/stable/27857554)
