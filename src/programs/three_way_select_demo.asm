# Three-way decision using TSIGN + ternary jumps
# If value in A is -1, 0, or +1, set C to -1/0/+1 accordingly using TJN/TJZ/TJP

    MOV A, -1      # try changing to 0 or 1 to see different branches
    TSIGN A        # normalize A to {-1,0,+1}

    TJN A, NEG
    TJZ A, ZERO
    TJP A, POS

NEG:
    MOV C, -1
    JMP END
ZERO:
    MOV C, 0
    JMP END
POS:
    MOV C, 1
END:
    HLT
