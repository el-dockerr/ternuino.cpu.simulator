class TernuinoAssembler:
    """Simple assembler for Ternuino assembly language"""

    def __init__(self):
        self.labels = {}
        self.program = []

    def parse_file(self, filename):
        """Parse an assembly file and return a program list"""
        self.labels = {}
        self.program = []

        with open(filename, 'r', encoding='utf-8') as file:
            lines = file.readlines()

        # First pass: collect labels and build program with unresolved symbols
        address = 0
        for line_num, raw in enumerate(lines, 1):
            line = raw.strip()

            # Strip inline comments
            if '#' in line:
                line = line[:line.index('#')].strip()
            if ';' in line:
                line = line[:line.index(';')].strip()

            if not line:
                continue

            # Support label only line or label + instruction on same line
            while True:
                if ':' in line:
                    left, right = line.split(':', 1)
                    label = left.strip()
                    if label:
                        if label in self.labels:
                            raise ValueError(f"Duplicate label '{label}' on line {line_num}")
                        self.labels[label] = address
                    line = right.strip()
                    if not line:
                        break
                else:
                    break

            if not line:
                continue

            try:
                instruction = self._parse_instruction(line)
            except Exception as e:
                raise ValueError(f"Syntax error on line {line_num}: {e}") from e

            if instruction is not None:
                self.program.append(instruction)
                address += 1

        # Second pass: resolve labels inside instructions
        self._resolve_labels()

        return self.program

    def _parse_instruction(self, line):
        """Parse a single instruction line (no comments/labels)."""
        if not line:
            return None

        # Tokenize: split by comma and whitespace
        tokens = [p.strip() for p in line.replace(',', ' ').split() if p.strip()]
        if not tokens:
            return None

        op = tokens[0].upper()

        def need(n):
            if len(tokens) < n + 1:
                raise ValueError(f"'{op}' expects {n} argument(s)")

        if op == "NOP":
            return ("NOP",)
        if op == "HLT":
            return ("HLT",)

        if op in ("MOV", "ADD", "SUB", "MUL", "DIV", "TAND", "TOR", "TJZ"):
            need(2)
            arg1 = self._parse_argument(tokens[1])
            arg2 = self._parse_argument(tokens[2])
            return (op, arg1, arg2)

        if op == "TNOT":
            need(1)
            arg1 = self._parse_argument(tokens[1])
            return (op, arg1)

        if op == "JMP":
            need(1)
            arg1 = self._parse_argument(tokens[1])
            return (op, arg1)

        raise ValueError(f"Unknown instruction '{op}'")

    def _parse_argument(self, arg):
        """Parse an instruction argument (register, number, or label)."""
        if not arg:
            return None

        u = arg.upper()
        if u in ("A", "B", "C"):
            return u

        # Try integer literal
        try:
            return int(arg)
        except ValueError:
            pass

        # Otherwise treat as label to resolve later
        return arg

    def _resolve_labels(self):
        """Replace label references in JMP/TJZ with concrete addresses."""
        resolved = []
        for idx, instr in enumerate(self.program):
            op = instr[0]
            if op == "JMP":
                target = instr[1]
                if isinstance(target, str):
                    if target not in self.labels:
                        raise ValueError(f"Undefined label '{target}' used in instruction {idx}")
                    target = self.labels[target]
                resolved.append((op, target))
            elif op == "TJZ":
                reg, target = instr[1], instr[2]
                if isinstance(target, str):
                    if target not in self.labels:
                        raise ValueError(f"Undefined label '{target}' used in instruction {idx}")
                    target = self.labels[target]
                resolved.append((op, reg, target))
            else:
                resolved.append(instr)

        self.program = resolved
