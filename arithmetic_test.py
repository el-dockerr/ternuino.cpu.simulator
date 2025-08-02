from library.ternuino import Ternuino

def test_arithmetic_operations():
    """Test the new arithmetic operations (SUB, MUL, DIV)"""
    
    print("=== Testing Ternary Arithmetic Operations ===\n")
    
    # Test 1: Subtraction
    print("Test 1: Subtraction")
    program_sub = [
        ("MOV", "A", 1),      # A = +1
        ("MOV", "B", -1),     # B = -1
        ("SUB", "A", "B"),    # A = A - B = 1 - (-1) = 2
        ("HLT",)
    ]
    
    cpu = Ternuino()
    cpu.load_program(program_sub)
    cpu.run()
    print(f"1 - (-1) = {cpu.registers['A']}")  # Should be 2
    print(f"Final registers: {cpu.registers}\n")
    
    # Test 2: Multiplication
    print("Test 2: Multiplication")
    program_mul = [
        ("MOV", "A", -1),     # A = -1
        ("MOV", "B", 1),      # B = +1
        ("MUL", "A", "B"),    # A = A * B = (-1) * 1 = -1
        ("HLT",)
    ]
    
    cpu = Ternuino()
    cpu.load_program(program_mul)
    cpu.run()
    print(f"(-1) * 1 = {cpu.registers['A']}")  # Should be -1
    print(f"Final registers: {cpu.registers}\n")
    
    # Test 3: Division
    print("Test 3: Division")
    program_div = [
        ("MOV", "A", 1),      # A = +1
        ("MOV", "B", -1),     # B = -1
        ("DIV", "A", "B"),    # A = A / B = 1 / (-1) = -1
        ("HLT",)
    ]
    
    cpu = Ternuino()
    cpu.load_program(program_div)
    cpu.run()
    print(f"1 / (-1) = {cpu.registers['A']}")  # Should be -1
    print(f"Final registers: {cpu.registers}\n")
    
    # Test 4: Division by zero handling
    print("Test 4: Division by zero")
    program_div_zero = [
        ("MOV", "A", 1),      # A = +1
        ("MOV", "B", 0),      # B = 0
        ("DIV", "A", "B"),    # A = A / B = 1 / 0 = 0 (safe handling)
        ("HLT",)
    ]
    
    cpu = Ternuino()
    cpu.load_program(program_div_zero)
    cpu.run()
    print(f"1 / 0 = {cpu.registers['A']} (safe handling)")  # Should be 0
    print(f"Final registers: {cpu.registers}\n")
    
    # Test 5: Complex arithmetic sequence
    print("Test 5: Complex arithmetic sequence")
    program_complex = [
        ("MOV", "A", 1),      # A = +1
        ("MOV", "B", -1),     # B = -1
        ("MOV", "C", 1),      # C = +1
        ("ADD", "A", "B"),    # A = 1 + (-1) = 0
        ("SUB", "B", "C"),    # B = (-1) - 1 = -2
        ("MUL", "C", "B"),    # C = 1 * (-2) = -2
        ("HLT",)
    ]
    
    cpu = Ternuino()
    cpu.load_program(program_complex)
    cpu.run()
    print("Complex sequence:")
    print("A = 1 + (-1) =", cpu.registers['A'])  # Should be 0
    print("B = (-1) - 1 =", cpu.registers['B'])  # Should be -2
    print("C = 1 * (-2) =", cpu.registers['C'])  # Should be -2
    print(f"Final registers: {cpu.registers}\n")

def demonstrate_ternary_arithmetic_advantages():
    """Demonstrate advantages of ternary arithmetic"""
    
    print("=== Ternary Arithmetic Advantages ===\n")
    
    # Demonstrate symmetric operations
    print("Symmetric operations in ternary:")
    for a in [-1, 0, 1]:
        for b in [-1, 0, 1]:
            print(f"{a:2} + {b:2} = {a+b:2}")
    print()
    
    print("Multiplication table for ternary values:")
    for a in [-1, 0, 1]:
        for b in [-1, 0, 1]:
            print(f"{a:2} * {b:2} = {a*b:2}")
    print()

if __name__ == "__main__":
    test_arithmetic_operations()
    demonstrate_ternary_arithmetic_advantages()
