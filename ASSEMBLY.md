# Ternuino Assembly Language Reference

## Overview
The Ternuino assembler supports a simple assembly language for writing programs for the ternary CPU. Programs are saved as `.asm` files and can be run using the interpreter.

## Language Syntax

### Comments
- `#` - Line comment (everything after # is ignored)
- `;` - Alternative line comment syntax

### Labels
- `label:` - Define a label for jumps
- Labels must be on their own line or at the beginning of a line

### Sections and Data
- `.data` – start data section
- `.text` – start code section (default)
- `.word N` – emit a single data word with integer value N
- `.zero K` – reserve K zero-initialized data words

Data labels declared in `.data` can be referenced by memory instructions and `LEA`.

### Instructions

#### Data Movement
- `MOV reg, value` - Move immediate value to register
- `MOV reg1, reg2` - Copy value from reg2 to reg1

#### Memory Instructions
- `LD reg, addr|[REG]` – Load from data memory at `addr` or at address in `[REG]` into `reg`.
- `ST reg, addr|[REG]` – Store value of `reg` into data memory at `addr` or at address in `[REG]`.
- `LEA reg, label|addr` – Load effective address (integer) of a data or code label, or an immediate address, into `reg`.

Notes:
- Indirect addressing uses square brackets around a register, e.g. `[A]`, `[B]`, `[C]`.
- Addresses wrap modulo the data memory size (default 27).

#### Arithmetic Operations
- `ADD reg1, reg2` - Add reg2 to reg1 (reg1 = reg1 + reg2)
- `SUB reg1, reg2` - Subtract reg2 from reg1 (reg1 = reg1 - reg2)
- `MUL reg1, reg2` - Multiply reg1 by reg2 (reg1 = reg1 * reg2)
- `DIV reg1, reg2` - Divide reg1 by reg2 (reg1 = reg1 / reg2)

#### Ternary Logic Operations
- `TAND reg1, reg2` - Ternary AND (reg1 = min(reg1, reg2))
- `TOR reg1, reg2` - Ternary OR (reg1 = max(reg1, reg2))
- `TNOT reg` - Ternary NOT (reg = -reg)

#### Control Flow
- `JMP label` - Unconditional jump to label
- `TJZ reg, label` - Jump to label if register equals zero
- `HLT` - Halt execution
- `NOP` - No operation

### Registers
- `A` - General purpose register
- `B` - General purpose register  
- `C` - General purpose register

### Values
- Ternary values: `-1`, `0`, `1`
- Larger integers are supported but will be converted to standard integers

## Example Programs

### Basic Logic
```assembly
# Simple ternary logic demonstration
MOV A, 1        # A = +1
MOV B, -1       # B = -1
TAND A, B       # A = min(1, -1) = -1
TNOT A          # A = -(-1) = 1
HLT
```

### Loop with Labels
```assembly
# Count from 1 to 3
MOV A, 1        # Counter
MOV B, 1        # Increment
MOV C, 3        # Target

loop:
ADD A, B        # Increment counter
SUB C, B        # Decrement target
TJZ C, end      # Jump if done
JMP loop        # Continue loop

end:
HLT             # Stop
```

### Arithmetic Operations
```assembly
# Demonstrate arithmetic
MOV A, 1
MOV B, -1
ADD A, B        # A = 1 + (-1) = 0
MUL B, B        # B = (-1) * (-1) = 1
DIV B, A        # B = 1 / 0 = 0 (safe division)
HLT
```

### Memory Demo
```assembly
.data
x:      .word 1
y:      .word -1
sum:    .word 0

.text
LEA A, x     # A = &x
LD  B, [A]   # B = x
LEA A, y
LD  C, [A]   # C = y
ADD B, C     # B = x + y
ST  B, sum   # sum = B
HLT
```

## Running Programs

### Command Line
```bash
python interpreter.py programs/your_program.asm
```

### Interactive Mode
```bash
python interpreter.py
```
This will show a menu of available programs to choose from.

## Error Handling
- Syntax errors will show the line number and error description
- Undefined labels will be reported during assembly
- Division by zero sets the result to 0 (safe handling)
- Unknown instructions will cause assembly to fail

## Tips
- Use meaningful label names
- Add comments to explain your logic
- Remember that ternary values are -1, 0, and +1
- The TJZ instruction only jumps when the register is exactly 0
- All arithmetic operations modify the first register
