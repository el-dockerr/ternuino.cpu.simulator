# Example 1: Basic ternary logic operations
# This program demonstrates ternary AND, NOT operations

MOV A, 1        # A = +1 (positive)
MOV B, -1       # B = -1 (negative)
TAND A, B       # A = min(+1, -1) = -1
TNOT A          # A = -(-1) = +1
HLT             # Stop execution
