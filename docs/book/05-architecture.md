# 05 – The Ternuino CPU: Architecture and Memory

The Ternuino CPU is a unique computer architecture designed around balanced ternary computation. It combines the elegance of ternary logic with practical computing capabilities, creating a system that's both educational and functional.

## Core Architecture Overview

Ternuino is intentionally compact, making it easy to understand while demonstrating the power of ternary computing principles.

### CPU Specifications

- **Architecture**: 32-bit balanced ternary CPU
- **Registers**: 3 general-purpose registers (A, B, C) + Program Counter (PC) + Stack Pointer (SP)
- **Instruction Memory**: 27 slots (3³), perfect for small programs and demos
- **Data Memory**: 27 words (configurable), separate from instruction memory
- **I/O Capabilities**: Device-based I/O with interrupt support
- **File System**: Native balanced ternary file format (.t3)

## Register Architecture

### General Purpose Registers
- **Register A**: Primary accumulator and result register
- **Register B**: Secondary operand and temporary storage
- **Register C**: Tertiary register for complex operations

### System Registers
- **PC (Program Counter)**: Points to the current instruction (0-26)
- **SP (Stack Pointer)**: Manages interrupt stack for nested calls
- **Interrupt State**: Global interrupt enable/disable flag

All registers store 32-bit signed integers, allowing representation of large ternary numbers while maintaining compatibility with standard arithmetic.

## Memory Organization

### Instruction Memory (Harvard Architecture)
```
Address    Purpose
0-26       Program instructions (27 total slots = 3³)
```

### Data Memory
```
Address    Purpose
0-26       Data storage (configurable size)
```

The Harvard architecture separates instruction and data memory, providing:
- **Security**: Code cannot modify itself accidentally
- **Performance**: Simultaneous instruction fetch and data access
- **Clarity**: Clear separation between program logic and data

### Memory Addressing Modes

1. **Immediate**: `MOV A, 5` - Direct value
2. **Register**: `MOV A, B` - Register-to-register
3. **Direct**: `LD A, 10` - Direct memory address
4. **Indirect**: `LD A, [B]` - Address stored in register B

## Instruction Set Architecture

### Data Movement Instructions
- `MOV reg, value` - Move immediate value or register content
- `LD reg, addr|[REG]` - Load from data memory
- `ST reg, addr|[REG]` - Store to data memory  
- `LEA reg, label|addr` - Load effective address

### Arithmetic Operations
- `ADD reg1, reg2` - Addition: reg1 = reg1 + reg2
- `SUB reg1, reg2` - Subtraction: reg1 = reg1 - reg2
- `MUL reg1, reg2` - Multiplication: reg1 = reg1 × reg2
- `DIV reg1, reg2` - Division: reg1 = reg1 ÷ reg2 (safe, returns 0 on divide-by-zero)

### Ternary Logic Operations
- `TAND reg1, reg2` - Ternary AND: min(reg1, reg2)
- `TOR reg1, reg2` - Ternary OR: max(reg1, reg2)  
- `TNOT reg` - Ternary NOT: -reg
- `NEG reg` - Arithmetic negation

### Ternary-Specific Arithmetic
- `TSIGN reg` - Extract ternary sign (-1, 0, or +1)
- `TABS reg` - Ternary absolute value
- `TSHL3 reg` - Shift left by one ternary position (multiply by 3)
- `TSHR3 reg` - Shift right by one ternary position (divide by 3)
- `TCMPR reg1, reg2` - Three-way comparison (sets reg1 to -1, 0, or +1)

### Control Flow
- `JMP label` - Unconditional jump
- `TJZ reg, label` - Jump if register equals zero
- `TJN reg, label` - Jump if register is negative  
- `TJP reg, label` - Jump if register is positive
- `HLT` - Halt execution
- `NOP` - No operation

### I/O and Device Operations
- `TOPEN file_id, mode` - Open device/file (mode: 0=read, 1=write)
- `TREAD file_id, register` - Read value from device into register
- `TWRITE file_id, value` - Write value to device
- `TCLOSE file_id` - Close device/file

### Interrupt System
- `IRQ vector` - Software interrupt
- `IRET` - Return from interrupt
- `EI` - Enable interrupts globally
- `DI` - Disable interrupts globally

## Device Architecture

The Ternuino CPU supports a sophisticated device system for I/O operations:

### Device Types
1. **Terminal Device**: Interactive text I/O with interrupt support
2. **File Device**: Balanced ternary file operations (.t3 format)
3. **Custom Devices**: Extensible architecture for new device types

### Device Interface
Each device provides standardized operations:
- **Read**: Get data from device
- **Write**: Send data to device  
- **Open**: Initialize device for operation
- **Close**: Finalize and cleanup device
- **Tick**: Per-cycle device processing
- **Interrupt**: Asynchronous event notification

### Interrupt System
- **8 Interrupt Vectors**: Configurable interrupt handlers (0-7)
- **Priority-based**: Lower vector numbers have higher priority
- **Maskable**: Individual interrupts can be enabled/disabled
- **Nested**: Interrupts can be nested with proper stack management

## Balanced Ternary File System

### .t3 File Format
Ternuino introduces a native balanced ternary file format:

**Header Structure (12 bytes):**
```
Offset  Size  Description
0-5     6     Magic: "T3FMT\0"
6       1     Version number (currently 1)
7-10    4     Data size (number of values, little-endian)
```

**Data Format:**
- Each value: length byte + balanced ternary string
- Characters: 'T'/'-' for -1, '0' for 0, '1'/'+' for +1

**Examples:**
| Decimal | Balanced Ternary | Encoding |
|---------|------------------|----------|
| 0       | 0               | 1 byte + "0" |
| 1       | 1               | 1 byte + "1" |
| -1      | T               | 1 byte + "T" |
| 13      | 111             | 3 bytes + "111" |
| -5      | T1T             | 3 bytes + "T1T" |

## Programming Model

### Assembly Language Structure
```assembly
# Comments start with # or ;
.data                    # Data section
variable: .word 42       # Define variables
array:    .zero 5        # Reserve space

.text                    # Code section  
start:                   # Labels for jumps
    MOV A, 1            # Instructions
    JMP loop            # Control flow
loop:
    HLT                 # End program
```

### Error Handling
- **Division by Zero**: Returns 0 (safe handling)
- **Invalid Memory Access**: Wraps modulo memory size
- **Device Errors**: Return -1 in register A
- **Interrupt Overflow**: Disabled interrupts prevent stack overflow

### Memory Management
- **No Dynamic Allocation**: Fixed memory layout for predictability
- **Automatic Wrapping**: Address calculations wrap at memory boundaries
- **Data Initialization**: .zero and .word directives for data setup

## Example: Complete Program Structure

```assembly
# Fibonacci sequence with file output
.data
fib_file:   .word 0      # File handle storage
count:      .word 10     # Number of fibonacci numbers

.text
start:
    # Open file for writing
    MOV A, 0             # File ID 0
    MOV B, 1             # Write mode
    TOPEN A, B           # Open file
    ST A, fib_file       # Store file handle
    
    # Initialize fibonacci sequence
    MOV A, 0             # F(0) = 0
    MOV B, 1             # F(1) = 1
    LD C, count          # Loop counter
    
fib_loop:
    # Write current fibonacci number
    LD A, fib_file       # Load file handle
    TWRITE A, B          # Write F(n)
    
    # Calculate next fibonacci number
    ADD A, B             # F(n+2) = F(n) + F(n+1)
    MOV B, A             # Shift values
    
    # Decrement counter and loop
    SUB C, 1
    TJP C, fib_loop      # Continue if positive
    
    # Close file and exit
    LD A, fib_file
    TCLOSE A
    HLT
```

This architecture combines the mathematical elegance of balanced ternary with practical computing needs, creating a unique and educational platform for exploring alternative number systems in computer science.


