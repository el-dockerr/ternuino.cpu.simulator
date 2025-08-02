class TernuinoAssembler:
    """Simple assembler for Ternuino assembly language"""
    
    def __init__(self):
        self.labels = {}
        self.program = []
    
    def parse_file(self, filename):
        """Parse an assembly file and return a program list"""
        self.labels = {}
        self.program = []
        
        # First pass: collect labels and convert instructions
        with open(filename, 'r', encoding='utf-8') as file:
            lines = file.readlines()
        
        address = 0
        for line_num, line in enumerate(lines, 1):
            line = line.strip()
            
            # Skip empty lines and comments
            if not line or line.startswith('#') or line.startswith(';'):
                continue
            
            # Handle labels (format: "label:")
            if line.endswith(':'):
                label = line[:-1].strip()
                self.labels[label] = address
                continue
            
            # Parse instruction
            try:
                instruction = self._parse_instruction(line)
                if instruction:
                    self.program.append(instruction)
                    address += 1
            except Exception as e:
                raise ValueError(f"Error on line {line_num}: {line}\n{str(e)}")
        
        # Second pass: resolve labels
        self._resolve_labels()
        
        return self.program
    
    def _parse_instruction(self, line):
        """Parse a single instruction line"""
        # Remove comments from end of line
        if '#' in line:
            line = line[:line.index('#')].strip()
        if ';' in line:
            line = line[:line.index(';')].strip()
        
        if not line:
            return None
        
        # Check if this line contains a label (ends with :)
        if ':' in line:
            # This should have been handled in the first pass
            # If we get here, it might be a label on the same line as an instruction
            parts = line.split(':', 1)
            if len(parts) == 2 and parts[1].strip():
                # Label followed by instruction on same line
                line = parts[1].strip()
            else:
                # Just a label, skip it
                return None
        
        # Split by comma and whitespace
        parts = []
        for part in line.replace(',', ' ').split():
            parts.append(part.strip())
        
        if not parts:
            return None
        
        op = parts[0].upper()
        
        # Handle different instruction formats
        if op == "NOP":
            return ("NOP",)
        elif op == "HLT":
            return ("HLT",)
        elif op in ["MOV", "ADD", "SUB", "MUL", "DIV", "TAND", "TOR", "TJZ"]:
            if len(parts) < 3:
                raise ValueError(f"Instruction {op} requires 2 arguments")
            arg1 = self._parse_argument(parts[1])
            arg2 = self._parse_argument(parts[2])
            return (op, arg1, arg2)
        elif op in ["TNOT"]:
            if len(parts) < 2:
                raise ValueError(f"Instruction {op} requires 1 argument")
            arg1 = self._parse_argument(parts[1])
            return (op, arg1)
        elif op == "JMP":
            if len(parts) < 2:
                raise ValueError(f"Instruction {op} requires 1 argument")
            arg1 = self._parse_argument(parts[1])
            return (op, arg1)
        else:
            raise ValueError(f"Unknown instruction: {op}")
    
    def _parse_argument(self, arg):
        """Parse an instruction argument (register, number, or label)"""
        arg = arg.strip()
        
        # Register (A, B, C)
        if arg.upper() in ['A', 'B', 'C']:
            return arg.upper()
        
        # Number (including negative)
        try:
            return int(arg)
        except ValueError:
            pass
        
        # Label (will be resolved in second pass)
        return arg
    
    def _resolve_labels(self):
        """Replace label references with addresses"""
        for i, instruction in enumerate(self.program):
            if len(instruction) > 1:
                new_instruction = list(instruction)
                for j in range(1, len(instruction)):
                    arg = instruction[j]
                    if isinstance(arg, str) and arg not in ['A', 'B', 'C']:
                        if arg in self.labels:
                            new_instruction[j] = self.labels[arg]
                        else:
                            raise ValueError(f"Undefined label: {arg}")
                self.program[i] = tuple(new_instruction)
