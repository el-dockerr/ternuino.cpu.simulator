# Example 3: Simple loop with labels and jumps
# This program counts from 1 to 3 using ternary arithmetic

MOV A, 1        # Counter = 1
MOV B, 1        # Increment value
MOV C, 3        # Target value

loop:
ADD A, B        # Increment counter
SUB C, B        # Decrement target
TJZ C, end      # Jump to end if target reaches 0
JMP loop        # Jump back to loop

end:
HLT             # Stop execution

# Final state should be: A=4, B=1, C=0
