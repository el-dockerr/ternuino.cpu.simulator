class TernuinoAssembler:
    """Simple assembler for Ternuino assembly language"""

    def __init__(self):
        self.labels = {}
        self.data_labels = {}
        self.data_image = []
        self.segment = "text"  # 'text' or 'data'
        self.program = []

    def parse_file(self, filename):
        """Parse an assembly file and return a program list"""
        self.labels = {}
        self.data_labels = {}
        self.data_image = []
        self.segment = "text"
        self.program = []

        with open(filename, 'r', encoding='utf-8') as file:
            lines = file.readlines()

        # First pass: collect labels and build program with unresolved symbols
        address = 0        # text address (instruction index)
        data_address = 0   # data address (index into data_image)
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
                        # label belongs to current segment
                        if self.segment == "text":
                            if label in self.labels:
                                raise ValueError(f"Duplicate label '{label}' on line {line_num}")
                            self.labels[label] = address
                        else:
                            if label in self.data_labels:
                                raise ValueError(f"Duplicate data label '{label}' on line {line_num}")
                            self.data_labels[label] = data_address
                    line = right.strip()
                    if not line:
                        break
                else:
                    break

            if not line:
                continue

            # Segment directives
            if line.lower().startswith('.data'):
                self.segment = 'data'
                continue
            if line.lower().startswith('.text'):
                self.segment = 'text'
                continue

            # Data directives
            if self.segment == 'data':
                tokens = [p.strip() for p in line.replace(',', ' ').split() if p.strip()]
                if not tokens:
                    continue
                dot = tokens[0].lower()
                if dot == '.word':
                    if len(tokens) < 2:
                        raise ValueError(f".word expects a value on line {line_num}")
                    try:
                        val = int(tokens[1])
                    except ValueError as e:
                        raise ValueError(f".word expects an integer on line {line_num}") from e
                    self.data_image.append(val)
                    data_address += 1
                    continue
                if dot == '.zero':
                    if len(tokens) < 2:
                        raise ValueError(f".zero expects a count on line {line_num}")
                    try:
                        count = int(tokens[1])
                    except ValueError as e:
                        raise ValueError(f".zero expects an integer count on line {line_num}") from e
                    if count < 0:
                        raise ValueError(f".zero count must be non-negative on line {line_num}")
                    self.data_image.extend([0] * count)
                    data_address += count
                    continue
                # If not a known data directive, error
                raise ValueError(f"Unknown data directive '{tokens[0]}' on line {line_num}")

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

        if op in ("MOV", "ADD", "SUB", "MUL", "DIV", "TAND", "TOR", "TJZ", "TJN", "TJP", "TCMPR"):
            need(2)
            arg1 = self._parse_argument(tokens[1])
            arg2 = self._parse_argument(tokens[2])
            return (op, arg1, arg2)

        if op in ("TNOT", "NEG", "TSIGN", "TABS", "TSHL3", "TSHR3"):
            need(1)
            arg1 = self._parse_argument(tokens[1])
            return (op, arg1)

        if op == "JMP":
            need(1)
            arg1 = self._parse_argument(tokens[1])
            return (op, arg1)

        # Memory ops
        if op in ("LD", "ST"):
            need(2)
            dst_or_src1 = self._parse_argument(tokens[1])
            src_or_dst2 = self._parse_argument(tokens[2])
            # Enforce [REG] for indirect, disallow plain register as address
            if op == "LD":
                if isinstance(src_or_dst2, str) and src_or_dst2 in ("A", "B", "C"):
                    raise ValueError("LD requires an address (label/number) or [REG] for indirect; got register. Use [A]/[B]/[C].")
            else:  # ST
                if isinstance(src_or_dst2, str) and src_or_dst2 in ("A", "B", "C"):
                    raise ValueError("ST requires an address (label/number) or [REG] for indirect; got register. Use [A]/[B]/[C].")
            return (op, dst_or_src1, src_or_dst2)

        if op == "LEA":
            need(2)
            reg = self._parse_argument(tokens[1])
            addr = self._parse_argument(tokens[2])
            return (op, reg, addr)

        raise ValueError(f"Unknown instruction '{op}'")

    def _parse_argument(self, arg):
        """Parse an instruction argument (register, number, or label)."""
        if not arg:
            return None

        # Indirect via register: [A], [B], [C]
        if arg.startswith('[') and arg.endswith(']'):
            inner = arg[1:-1].strip().upper()
            if inner in ("A", "B", "C"):
                return ("IND", inner)
            raise ValueError(f"Invalid indirect operand '[{inner}]' (expected [A], [B], or [C])")

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
        """Replace label references in control flow and memory ops with concrete addresses."""
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
            elif op in ("TJZ", "TJN", "TJP"):
                reg, target = instr[1], instr[2]
                if isinstance(target, str):
                    if target not in self.labels:
                        raise ValueError(f"Undefined label '{target}' used in instruction {idx}")
                    target = self.labels[target]
                resolved.append((op, reg, target))
            elif op in ("LD", "ST"):
                r1, addr = instr[1], instr[2]
                if isinstance(addr, str):
                    if addr not in self.data_labels:
                        raise ValueError(f"Undefined data label '{addr}' used in instruction {idx}")
                    addr = self.data_labels[addr]
                # keep indirect tuples as-is
                resolved.append((op, r1, addr))
            elif op == "LEA":
                r1, addr = instr[1], instr[2]
                if isinstance(addr, str):
                    # Prefer data labels; fall back to text labels if present
                    if addr in self.data_labels:
                        addr = self.data_labels[addr]
                    elif addr in self.labels:
                        addr = self.labels[addr]
                    else:
                        raise ValueError(f"Undefined label '{addr}' used in instruction {idx}")
                resolved.append((op, r1, addr))
            else:
                resolved.append(instr)

        self.program = resolved

    # Accessor for interpreter/runner
    def get_data_image(self):
        return list(self.data_image)
