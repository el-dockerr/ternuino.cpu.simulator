# 06 – The Assembly Language

The Ternuino Assembly Language provides a human-readable way to write programs for the ternary CPU. This chapter covers everything you need to know about writing, assembling, and running your own ternary programs.

## Getting Started

### Program Structure

Assembly files are saved with the `.asm` extension and should be placed in the `programs/` directory. Every program follows this basic structure:

```assembly
# Comments start with # or ;
# This is a simple program structure

.data                    # Optional data section
variable: .word 42       # Define a variable with initial value
buffer:   .zero 5        # Reserve 5 zero-initialized words

.text                    # Code section (can be omitted, it's default)
start:                   # Program entry point (optional label)
    MOV A, 1            # Your instructions here
    MOV B, variable     # Reference data variables
    HLT                 # Always end with HLT
```

### Comments and Documentation

Use comments liberally to make your code clear:

```assembly
# Calculate the sum of two ternary values
MOV A, 1        # First value: positive
MOV B, -1       # Second value: negative  
ADD A, B        # A = A + B = 1 + (-1) = 0
HLT             # Program complete
```

Both `#` and `;` can be used for comments, and everything after the comment character is ignored.

## Running Programs

### Interactive Mode

Start the Ternuino simulator without arguments to enter interactive mode:

```bash
# Windows
build\ternuino.exe

# Linux/macOS  
./build/ternuino
```

This will:
1. Scan the `programs/` directory for `.asm` files
2. Display a numbered list of available programs
3. Let you select which program to run
4. Show execution results and final register states

### Command Line Mode

Run a specific program directly:

```bash
# Windows
build\ternuino.exe programs\your_program.asm

# Linux/macOS
./build/ternuino programs/your_program.asm
```

### Example Session

```
$ ./build/ternuino

Available programs:
1. arithmetic_demo.asm
2. fibonacci_demo.asm  
3. logic_demo.asm
4. my_program.asm

Select a program (1-4): 3

Loading: programs/logic_demo.asm
Execution complete.

Final register states:
A: 1
B: -1
C: 0
Program Counter: 8
```

## Complete Instruction Reference

### Data Movement Instructions

```assembly
# Move immediate value to register
MOV A, 1        # A = 1
MOV B, -1       # B = -1
MOV C, 0        # C = 0

# Copy between registers
MOV A, B        # A = B (copy B's value to A)
MOV C, A        # C = A (copy A's value to C)
```

### Arithmetic Operations

```assembly
# Basic arithmetic (modifies first operand)
MOV A, 5
MOV B, 3
ADD A, B        # A = A + B = 5 + 3 = 8
SUB A, B        # A = A - B = 8 - 3 = 5  
MUL A, B        # A = A * B = 5 * 3 = 15
DIV A, B        # A = A / B = 15 / 3 = 5

# Division by zero is safe (returns 0)
MOV A, 10
MOV B, 0
DIV A, B        # A = 0 (safe division by zero)
```

### Ternary Logic Operations

```assembly
# Ternary AND (minimum of two values)
MOV A, 1        # A = +1 (positive)
MOV B, -1       # B = -1 (negative)
TAND A, B       # A = min(1, -1) = -1

# Ternary OR (maximum of two values)  
MOV A, 0        # A = 0 (neutral)
MOV B, 1        # B = +1 (positive)
TOR A, B        # A = max(0, 1) = 1

# Ternary NOT (negation)
MOV A, 1        # A = +1
TNOT A          # A = -1
TNOT A          # A = +1 (back to original)
```

### Ternary-Specific Arithmetic

```assembly
# Extract sign of a number
MOV A, -42      # A = -42
TSIGN A         # A = -1 (sign of negative number)

MOV B, 0        # B = 0  
TSIGN B         # B = 0 (sign of zero)

MOV C, 15       # C = 15
TSIGN C         # C = 1 (sign of positive number)

# Absolute value
MOV A, -5       # A = -5
TABS A          # A = 5 (absolute value)

# Ternary shift operations (multiply/divide by 3)
MOV A, 4        # A = 4
TSHL3 A         # A = 4 * 3 = 12 (shift left base-3)
TSHR3 A         # A = 12 / 3 = 4 (shift right base-3)

# Three-way comparison
MOV A, 5        # A = 5
MOV B, 3        # B = 3  
TCMPR A, B      # A = sign(5 - 3) = sign(2) = 1
```

### Control Flow

```assembly
# Unconditional jump
JMP end         # Always jump to 'end' label

# Conditional jumps based on register value
MOV A, 0
TJZ A, zero_handler    # Jump if A == 0

MOV B, -5
TJN B, negative_handler # Jump if B < 0

MOV C, 10  
TJP C, positive_handler # Jump if C > 0

# Labels for jump targets
zero_handler:
    # Handle zero case
    JMP done

negative_handler:
    # Handle negative case  
    JMP done

positive_handler:
    # Handle positive case
    JMP done

done:
    HLT
```

### Memory Operations

```assembly
.data
x:      .word 10        # Define variable x = 10
y:      .word -5        # Define variable y = -5  
result: .word 0         # Define variable result = 0
buffer: .zero 3         # Reserve 3 zero-initialized words

.text
# Load effective address (get memory address of variable)
LEA A, x               # A = address of x
LEA B, result          # B = address of result

# Direct memory access
LD C, x                # C = value at x (C = 10)
ST C, result           # Store C into result variable

# Indirect memory access (using register as address)
LEA A, y               # A = address of y
LD B, [A]              # B = value at address in A (B = -5)
LEA A, result          # A = address of result  
ST B, [A]              # Store B at address in A
```

### I/O Operations

```assembly
# File I/O with balanced ternary format
MOV A, 0        # File ID 0
MOV B, 1        # Write mode (1 = write, 0 = read)
TOPEN A, B      # Open file for writing

# Write values to file
MOV B, 13       # Value to write
TWRITE A, B     # Write to file

MOV B, -5       # Another value  
TWRITE A, B     # Write to file

TCLOSE A        # Close file

# Read values back
MOV A, 0        # File ID 0
MOV B, 0        # Read mode
TOPEN A, B      # Open file for reading

TREAD A, C      # Read value into register C
TCLOSE A        # Close file
```

## Complete Program Examples

### Example 1: Basic Logic Demo

```assembly
# logic_demo.asm - Demonstrates ternary logic operations

# Start with some basic ternary values
MOV A, 1        # A = +1 (positive/true)
MOV B, -1       # B = -1 (negative/false)  
MOV C, 0        # C = 0 (neutral/unknown)

# Ternary AND operations
TAND A, B       # A = min(1, -1) = -1
TAND B, C       # B = min(-1, 0) = -1
TAND C, A       # C = min(0, -1) = -1

# Reset values for OR operations
MOV A, 1        # A = +1
MOV B, 0        # B = 0
MOV C, -1       # C = -1

# Ternary OR operations  
TOR A, B        # A = max(1, 0) = 1
TOR B, C        # B = max(0, -1) = 0
TOR C, A        # C = max(-1, 1) = 1

# Ternary NOT operations
TNOT A          # A = -1 (flip +1 to -1)
TNOT B          # B = 0 (flip 0 stays 0)  
TNOT C          # C = -1 (flip +1 to -1)

HLT             # End program
```

### Example 2: Arithmetic Demo

```assembly
# arithmetic_demo.asm - Basic arithmetic operations

# Initialize values
MOV A, 8        # A = 8  
MOV B, 3        # B = 3

# Arithmetic operations
ADD A, B        # A = 8 + 3 = 11
SUB A, B        # A = 11 - 3 = 8
MUL A, B        # A = 8 * 3 = 24
DIV A, B        # A = 24 / 3 = 8

# Demonstrate ternary-specific operations
TSIGN A         # A = sign(8) = 1
TABS A          # A = abs(1) = 1
TSHL3 A         # A = 1 * 3 = 3
TSHR3 A         # A = 3 / 3 = 1

# Test division by zero safety
MOV B, 0        # B = 0
DIV A, B        # A = 0 (safe division by zero)

HLT
```

### Example 3: Loop with Counter

```assembly
# loop_demo.asm - Counting loop demonstration

# Initialize loop counter and target
MOV A, 0        # Counter starts at 0
MOV B, 1        # Increment value
MOV C, 5        # Target count

loop:
    ADD A, B    # Increment counter (A = A + 1)
    SUB C, B    # Decrement remaining count (C = C - 1)
    TJP C, loop # Continue if C > 0
    
    # Loop finished
    HLT
```

### Example 4: Three-Way Decision Making

```assembly
# three_way_demo.asm - Demonstrates ternary decision making

# Get a value to classify
MOV A, -7       # Test value (try changing this)

# Determine if value is negative, zero, or positive
TSIGN A         # A = sign(-7) = -1

# Branch based on sign
TJN A, negative # Jump if A < 0
TJZ A, zero     # Jump if A == 0  
TJP A, positive # Jump if A > 0

negative:
    MOV B, -1   # Set result to indicate negative
    JMP done

zero:
    MOV B, 0    # Set result to indicate zero
    JMP done
    
positive:
    MOV B, 1    # Set result to indicate positive
    JMP done

done:
    HLT         # B now contains the classification
```

### Example 5: Memory and Data Demo

```assembly
# memory_demo.asm - Working with data memory

.data
value1:    .word 42     # First value
value2:    .word -13    # Second value  
sum:       .word 0      # Result storage
temp:      .word 0      # Temporary storage

.text
# Load values from memory
LD A, value1           # A = 42
LD B, value2           # B = -13

# Calculate sum
ADD A, B               # A = 42 + (-13) = 29

# Store result
ST A, sum              # sum = 29

# Demonstrate indirect addressing
LEA B, temp            # B = address of temp
ST A, [B]              # Store A at address in B (temp = 29)

# Verify indirect load
LD C, [B]              # C = value at address in B (C = 29)

HLT
```

## Tips for Writing Good Assembly Code

### 1. Use Meaningful Labels
```assembly
# Good
calculate_fibonacci:
    # Fibonacci calculation here
    JMP display_result

# Bad  
loop1:
    # Hard to understand purpose
    JMP end1
```

### 2. Comment Your Intent
```assembly
# Good - explains WHY
MOV A, 3        # Set loop counter to 3 iterations
TSHL3 B         # Scale factor by 3 for ternary position

# Bad - explains WHAT (obvious from instruction)
MOV A, 3        # Move 3 to A
TSHL3 B         # Shift B left by 3
```

### 3. Initialize Registers Clearly
```assembly
# Initialize all working registers at program start
MOV A, 0        # Accumulator starts at zero
MOV B, 1        # Increment value  
MOV C, 10       # Loop limit
```

### 4. Handle Edge Cases
```assembly
# Check for division by zero
MOV A, 10
MOV B, 0
TCMPR B, 0      # Compare B with 0
TJZ B, skip_div # Skip division if B is zero
DIV A, B        # Safe to divide
skip_div:
    # Continue with program
```

### 5. Keep Programs Visual and Small

The Ternuino CPU has only 27 memory slots, so keep programs concise. This limitation encourages:
- Clear, focused algorithms
- Efficient use of registers
- Creative problem-solving within constraints

### 6. Use the Data Section Effectively

```assembly
.data
# Group related data together
player_score:   .word 0
player_lives:   .word 3
game_level:     .word 1

# Use meaningful names for constants
max_health:     .word 100
damage_amount:  .word 10
```

## Debugging Your Programs

### Common Errors and Solutions

**1. Undefined Label**
```assembly
# Error: undefined label 'foo'
JMP foo    # foo is never defined

# Solution: define the label
JMP bar
bar:
    HLT
```

**2. Missing HLT Instruction**
```assembly
# Error: program runs past end of code
MOV A, 1
# Missing HLT - program will continue executing garbage

# Solution: always end with HLT
MOV A, 1  
HLT
```

**3. Incorrect Register Names**
```assembly
# Error: invalid register
MOV D, 1   # No register D

# Solution: use A, B, or C
MOV A, 1   # Correct
```

**4. Invalid Immediate Values**
```assembly
# These work fine - Ternuino handles any integer
MOV A, 1000    # Large positive
MOV B, -500    # Large negative
MOV C, 0       # Zero
```

## Building and Running Your Programs

### Building the Simulator

**Windows:**
```cmd
cd src
build.bat
```

**Linux/macOS:**
```bash
cd src  
make
```

### Running Your Program

1. Save your assembly code as `programs/my_program.asm`
2. Run the simulator:
   ```bash
   # Interactive mode
   ./build/ternuino
   
   # Direct execution
   ./build/ternuino programs/my_program.asm
   ```

3. View the results and register states

The assembler will automatically:
- Parse your assembly syntax
- Resolve all labels
- Generate machine code
- Load and execute your program
- Display final register values

Start with the simple examples above, then experiment with your own programs. The 27-instruction limit encourages creativity and helps you understand the fundamental operations of ternary computing!
