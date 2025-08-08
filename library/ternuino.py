from library.tritlogic import TritLogic
from library.tritarith import tsign, tabs, tshl3, tshr3, tcmpr


TRIT = (-1, 0, 1)

class Ternuino:
    def __init__(self):
        self.registers = { 'A': 0, 'B': 0, 'C': 0 }
        self.pc = 0
        self.running = True
        self.memory = [None] * 27  # z. B. 27 Speicherzellen

    def load_program(self, program):
        for i, instr in enumerate(program):
            self.memory[i] = instr

    def step(self):
        # Halt if PC out of memory bounds
        if self.pc < 0 or self.pc >= len(self.memory):
            self.running = False
            return

        instr = self.memory[self.pc]
        self.pc += 1
        if instr is None:
            # If we encounter empty memory, stop to avoid running off the end
            if self.pc >= len(self.memory):
                self.running = False
            return

        op = instr[0]

        if op == "NOP":
            pass
        elif op == "MOV":
            reg, val = instr[1], instr[2]
            # Handle both immediate values and register-to-register moves
            if isinstance(val, str) and val in self.registers:
                self.registers[reg] = self.registers[val]
            else:
                self.registers[reg] = val
        elif op == "ADD":
            reg1, reg2 = instr[1], instr[2]
            self.registers[reg1] += self.registers[reg2]
        elif op == "SUB":
            reg1, reg2 = instr[1], instr[2]
            self.registers[reg1] -= self.registers[reg2]
        elif op == "MUL":
            reg1, reg2 = instr[1], instr[2]
            self.registers[reg1] *= self.registers[reg2]
        elif op == "DIV":
            reg1, reg2 = instr[1], instr[2]
            if self.registers[reg2] != 0:
                # Ternary division: truncate towards zero
                self.registers[reg1] = int(self.registers[reg1] / self.registers[reg2])
            else:
                # Division by zero: set result to 0 (could also halt or set error flag)
                self.registers[reg1] = 0
        elif op == "JMP":
            self.pc = instr[1]
        elif op == "HLT":
            self.running = False
        elif op == "TAND":
            reg1, reg2 = instr[1], instr[2]
            self.registers[reg1] = TritLogic.tand(self.registers[reg1], self.registers[reg2])
        elif op == "TOR":
            reg1, reg2 = instr[1], instr[2]
            self.registers[reg1] = TritLogic.tor(self.registers[reg1], self.registers[reg2])
        elif op == "TNOT":
            reg = instr[1]
            self.registers[reg] = TritLogic.tnot(self.registers[reg])
        elif op == "TSIGN":
            reg = instr[1]
            self.registers[reg] = tsign(self.registers[reg])
        elif op == "TABS":
            reg = instr[1]
            self.registers[reg] = tabs(self.registers[reg])
        elif op == "TSHL3":
            reg = instr[1]
            self.registers[reg] = tshl3(self.registers[reg])
        elif op == "TSHR3":
            reg = instr[1]
            self.registers[reg] = tshr3(self.registers[reg])
        elif op == "TCMPR":
            reg1, reg2 = instr[1], instr[2]
            self.registers[reg1] = tcmpr(self.registers[reg1], self.registers[reg2])
        elif op == "TJZ":
            reg, addr = instr[1], instr[2]
            if self.registers[reg] == 0:
                self.pc = addr
        elif op == "TJN":
            reg, addr = instr[1], instr[2]
            if self.registers[reg] < 0:
                self.pc = addr
        elif op == "TJP":
            reg, addr = instr[1], instr[2]
            if self.registers[reg] > 0:
                self.pc = addr


    def run(self):
        while self.running:
            self.step()
