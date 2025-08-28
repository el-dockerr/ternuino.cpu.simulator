# Terminal I/O Demo with IRQ Handling
# This program demonstrates real terminal I/O with interrupt handling

# Enable interrupts
EI

# Set up terminal for I/O (device 0, read/write mode)
TOPEN 0, 2

# Print prompt character '>' (ASCII 62)
TWRITE 0, 62

# Print space (ASCII 32)
TWRITE 0, 32

# Try to read input (will put result in register A, data in register B)
TREAD 0, B

# Check if read was successful (A == 0 means success)
TCMPR A, 0
TJN A, 8    # If read failed (A < 0), jump back to TREAD instruction (line 8)

# We got input - echo the character back
TWRITE 0, B

# Check if it was Enter (10) to print newline
TCMPR B, 10
TJZ B, 19   # If Enter, jump to newline handling

# Check if it was 'q' (113) to quit
TCMPR B, 113
TJZ B, 23   # If 'q', jump to quit

# Continue loop - print new prompt
TWRITE 0, 62  # Print '>'
TWRITE 0, 32  # Print space
JMP 11        # Jump back to TREAD

# Handle newline
TWRITE 0, 10  # Print newline
TWRITE 0, 62  # Print new prompt '>'
TWRITE 0, 32  # Print space
JMP 11        # Jump back to TREAD

# Quit sequence
TWRITE 0, 10     # newline
TWRITE 0, 71     # G
TWRITE 0, 111    # o
TWRITE 0, 111    # o
TWRITE 0, 100    # d
TWRITE 0, 98     # b
TWRITE 0, 121    # y
TWRITE 0, 101    # e
TWRITE 0, 33     # !
TWRITE 0, 10     # newline

# Close terminal
TCLOSE 0

# Halt
HLT
