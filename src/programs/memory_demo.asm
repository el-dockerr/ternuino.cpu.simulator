# Memory demo: LD/ST/LEA with .data/.text and indirect [REG]

.data
x:      .word 1
y:      .word -1
sum:    .word 0
scratch:.zero 3

.text
        LEA A, x        # A = &x (address)
        LD  B, [A]      # B = *A (load x)
        LEA A, y
        LD  C, [A]      # C = y
        ADD B, C        # B = x + y
        ST  B, sum      # sum = B
        HLT
