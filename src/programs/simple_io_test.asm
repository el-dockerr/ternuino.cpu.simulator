# Simple I/O test
# Test only file write operations

MOV A, 0        # File ID 0  
MOV B, 1        # Write mode
TOPEN A, B      # Open file for writing

MOV B, 42       # Write the answer to everything
TWRITE A, B

TCLOSE A        # Close file

MOV A, 1        # Success indicator
HLT             # Done
