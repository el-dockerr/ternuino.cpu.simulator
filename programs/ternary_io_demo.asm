# Ternary I/O Demo Program
# This program demonstrates file I/O operations with balanced ternary format
# It writes some values to a file and then reads them back

.section data
# No data section needed for this demo

.section code
start:
    # Demo 1: Write some ternary values to file
    MOV A, 0        # File ID 0
    MOV B, 1        # Write mode (1 = write, 0 = read)
    TOPEN A, B      # Open file for writing
    
    # Check if file opened successfully (A = 0 for success)
    TCMPR A, 0
    TJN error       # Jump to error if A < 0 (file open failed)
    
    # Write some ternary values
    MOV B, 13       # Write decimal 13 (balanced ternary: 111)
    TWRITE A, B
    
    MOV B, -5       # Write decimal -5 (balanced ternary: T1T)
    TWRITE A, B
    
    MOV B, 0        # Write zero
    TWRITE A, B
    
    MOV B, 27       # Write decimal 27 (balanced ternary: 1000)
    TWRITE A, B
    
    # Close the file
    TCLOSE A
    
    # Demo 2: Read the values back
    MOV A, 0        # File ID 0
    MOV B, 0        # Read mode (0 = read)
    TOPEN A, B      # Open file for reading
    
    # Check if file opened successfully
    TCMPR A, 0
    TJN error       # Jump to error if file open failed
    
    # Read values back into registers
    TREAD A, B      # Read first value into B
    TCMPR A, 0      # Check if read was successful
    TJN read_end    # If error, jump to end
    
    TREAD A, C      # Read second value into C
    TCMPR A, 0
    TJN read_end
    
    # Continue reading (values will overwrite B and C)
    TREAD A, B      # Read third value
    TCMPR A, 0
    TJN read_end
    
    TREAD A, C      # Read fourth value
    TCMPR A, 0
    TJN read_end
    
read_end:
    # Close the file
    MOV A, 0        # File ID 0
    TCLOSE A
    
    # Set success indicator
    MOV A, 1        # Success!
    HLT
    
error:
    MOV A, -1       # Error indicator
    HLT
