import sys
import os
from library.ternuino import Ternuino
from library.tritword import TritWord
from library.assembler import TernuinoAssembler


def run_program_file(filename):
    """Load and run a program from an assembly file"""
    if not os.path.exists(filename):
        print(f"Error: File '{filename}' not found.")
        return
    
    try:
        # Parse the assembly file
        assembler = TernuinoAssembler()
        program = assembler.parse_file(filename)
        
        print(f"=== Running program: {filename} ===")
        print(f"Loaded {len(program)} instructions")
        
        # Display the parsed program
        print("\nParsed program:")
        for i, instr in enumerate(program):
            print(f"  {i:2}: {instr}")
        print()
        
        # Create and run the CPU
        cpu = Ternuino()
        cpu.load_program(program)
        
        print("Initial registers:", cpu.registers)
        cpu.run()
        print("Final registers:  ", cpu.registers)
        print()
        
    except Exception as e:
        print(f"Error running program: {e}")


def list_available_programs():
    """List all available program files"""
    programs_dir = "programs"
    if not os.path.exists(programs_dir):
        print("No programs directory found.")
        return []
    
    programs = [f for f in os.listdir(programs_dir) if f.endswith('.asm')]
    return programs


def interactive_mode():
    """Interactive mode for selecting and running programs"""
    print("=== Ternuino CPU Interpreter ===")
    print("Ternary Computer Simulator with Assembly Language Support\n")
    
    while True:
        print("Available programs:")
        programs = list_available_programs()
        
        if not programs:
            print("  No assembly programs found in 'programs/' directory")
            break
        
        for i, program in enumerate(programs, 1):
            print(f"  {i}. {program}")
        
        print("  0. Exit")
        
        try:
            choice = input("\nSelect a program to run (number): ").strip()
            
            if choice == '0':
                print("Goodbye!")
                break
            
            choice_num = int(choice)
            if 1 <= choice_num <= len(programs):
                selected_program = programs[choice_num - 1]
                program_path = os.path.join("programs", selected_program)
                run_program_file(program_path)
            else:
                print("Invalid selection. Please try again.")
                
        except ValueError:
            print("Please enter a valid number.")
        except KeyboardInterrupt:
            print("\nGoodbye!")
            break


if __name__ == "__main__":
    # Check command line arguments
    if len(sys.argv) > 1:
        # Run specific program file
        program_file = sys.argv[1]
        run_program_file(program_file)
    else:
        # Interactive mode
        interactive_mode()
    
    # Also demonstrate TritWord functionality
    print("\n=== Ternary Number Conversion Demo ===")
    for i in range(10):
        w = TritWord.from_int(i)
        print(f"{i} in ternary: {w}")
        # print(f"Back to decimal: {w.to_int()}")