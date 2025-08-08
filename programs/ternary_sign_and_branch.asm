# Demonstrate TSIGN, TCMPR, and ternary branches TJN/TJP
# Goal: classify A vs B and branch to one of three labels

    MOV A, 1       # A = +1
    MOV B, -1      # B = -1

    # r := sign(A - B) in A
    TCMPR A, B     # A = sign(+1 - -1) = sign(2) = +1

    TJN A, LESS    # if A < 0
    TJZ A, EQUAL   # if A == 0
    TJP A, GREATER # if A > 0

LESS:
    # put -1 in C
    MOV C, -1
    JMP DONE

EQUAL:
    MOV C, 0
    JMP DONE

GREATER:
    MOV C, 1

DONE:
    HLT
