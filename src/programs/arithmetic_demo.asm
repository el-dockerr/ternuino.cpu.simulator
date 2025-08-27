# Example 2: Arithmetic operations
# This program demonstrates the new arithmetic instructions

MOV A, 1        # A = +1
MOV B, -1       # B = -1
MOV C, 0        # C = 0

# Test addition and subtraction
ADD A, B        # A = 1 + (-1) = 0
SUB B, A        # B = (-1) - 0 = -1

# Test multiplication
MOV A, -1       # Reset A to -1
MUL A, A        # A = (-1) * (-1) = 1

# Test division
MOV B, 1        # B = 1
DIV A, B        # A = 1 / 1 = 1

HLT             # Stop execution
