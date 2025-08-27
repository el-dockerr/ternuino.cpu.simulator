# Example 4: Fibonacci-like sequence in ternary
# Calculate first few numbers of a ternary sequence

MOV A, 0        # First number
MOV B, 1        # Second number
MOV C, 0        # Temporary storage

# Calculate next numbers in sequence
# C = A + B, then shift: A = B, B = C

# Step 1: 0, 1, 1
ADD C, A        # C = 0 + 0 = 0
ADD C, B        # C = 0 + 1 = 1
MOV A, B        # A = 1
MOV B, C        # B = 1

# Step 2: 1, 1, 2  
MOV C, 0        # Reset C
ADD C, A        # C = 0 + 1 = 1
ADD C, B        # C = 1 + 1 = 2
MOV A, B        # A = 1
MOV B, C        # B = 2

# Step 3: 1, 2, 3
MOV C, 0        # Reset C  
ADD C, A        # C = 0 + 1 = 1
ADD C, B        # C = 1 + 2 = 3
MOV A, B        # A = 2
MOV B, C        # B = 3

HLT             # Final: A=2, B=3, C=3
