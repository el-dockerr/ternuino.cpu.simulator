import unittest
from library.assembler import TernuinoAssembler
from library.ternuino import Ternuino


def assemble(src: str):
    import tempfile, os
    with tempfile.NamedTemporaryFile("w", delete=False, suffix=".asm") as f:
        f.write(src)
        fname = f.name
    try:
        asm = TernuinoAssembler()
        prog = asm.parse_file(fname)
        data = asm.get_data_image()
        return prog, data
    finally:
        os.remove(fname)


class TestNeg(unittest.TestCase):
    def test_neg_mnemonic(self):
        prog, data = assemble(".text\nMOV A, -5\nNEG A\nHLT\n")
        cpu = Ternuino()
        cpu.load_program(prog, data)
        cpu.run()
        self.assertEqual(cpu.registers["A"], 5)

    def test_neg_zero(self):
        prog, data = assemble(".text\nMOV B, 0\nNEG B\nHLT\n")
        cpu = Ternuino()
        cpu.load_program(prog, data)
        cpu.run()
        self.assertEqual(cpu.registers["B"], 0)

    def test_neg_positive(self):
        prog, data = assemble(".text\nMOV C, 7\nNEG C\nHLT\n")
        cpu = Ternuino()
        cpu.load_program(prog, data)
        cpu.run()
        self.assertEqual(cpu.registers["C"], -7)


if __name__ == "__main__":
    unittest.main()
