# Demonstrate TSHL3 and TSHR3 as base-3 scaling

    MOV A, 1       # A = +1
    TSHL3 A        # A = 3
    TSHL3 A        # A = 9
    TSHR3 A        # A = 3
    TSHR3 A        # A = 1

    # Absolute and sign
    MOV B, -1
    TABS B         # B = 1
    TSIGN B        # B = +1

    HLT
