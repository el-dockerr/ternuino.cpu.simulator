# 06 – The Assembly Language

Assembly files live in `programs/*.asm` and support labels (# or ; for comments). The assembler does a simple two‑pass parse and label resolution.

Core instructions:
- `MOV A, 1` (or `MOV A, B`)
- `ADD A, B`, `SUB A, B`, `MUL A, B`, `DIV A, B` (truncates toward zero)
- `TAND A, B`, `TOR A, B`, `TNOT A`
- `JMP LABEL`, `TJZ A, LABEL`, `TJN A, LABEL`, `TJP A, LABEL`
- `TSIGN A`, `TABS A`, `TSHL3 A`, `TSHR3 A`, `TCMPR A, B`
- `HLT`, `NOP`

Labels:
```
LOOP:
  ADD A, B
  TJZ A, DONE
  JMP LOOP
DONE:
  HLT
```

Tips:
- Use comments generously to keep intent clear
- Keep programs tiny and visual; the GUI shines when you can step and see state change
