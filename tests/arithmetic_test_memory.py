import unittest
from library.assembler import TernuinoAssembler
from library.ternuino import Ternuino

class TestMemoryOps(unittest.TestCase):
    def assemble(self, src: str):
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

    def test_direct_ld_st(self):
        prog, data = self.assemble(
            ".data\n.zero 10\n.text\nMOV A, 5\nST A, 5\nLD B, 5\nHLT\n"
        )
        cpu = Ternuino()
        cpu.load_program(prog, data)
        cpu.run()
        self.assertEqual(cpu.data_mem[5], 5)
        self.assertEqual(cpu.registers["B"], 5)

    def test_indirect(self):
        prog, data = self.assemble(
            ".data\n.zero 10\n.text\nMOV A, 7\nMOV B, 3\nST A, [B]\nLD C, [B]\nHLT\n"
        )
        cpu = Ternuino()
        cpu.load_program(prog, data)
        cpu.run()
        self.assertEqual(cpu.data_mem[3], 7)
        self.assertEqual(cpu.registers["C"], 7)

    def test_wrap(self):
        prog, data = self.assemble(
            ".data\n.zero 1\n.text\nMOV A, 9\nST A, 28\nLD B, 1\nHLT\n"
        )
        cpu = Ternuino(dmem_size=27)
        cpu.load_program(prog, data)
        cpu.run()
        # 28 % 27 == 1
        self.assertEqual(cpu.data_mem[1], 9)
        self.assertEqual(cpu.registers["B"], 9)

if __name__ == "__main__":
    unittest.main()
