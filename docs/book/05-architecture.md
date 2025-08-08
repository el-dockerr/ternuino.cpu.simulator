# 05 – The Ternuino CPU: Architecture and Memory

Ternuino is intentionally tiny so you can see everything.

- Registers: A, B, C (integers), PC (program counter)
- Memory: 27 instruction slots (3³), enough for small demos
- Execution: `step()` executes one instruction; `run()` loops until HLT or safety stop

Supported instructions (high level):
- Data & arithmetic: `MOV`, `ADD`, `SUB`, `MUL`, `DIV`
- Logic: `TAND`, `TOR`, `TNOT`
- Control flow: `JMP`, `TJZ`, `TJN`, `TJP`, `HLT`, `NOP`
- Ternary‑friendly arithmetic: `TSIGN`, `TABS`, `TSHL3`, `TSHR3`, `TCMPR`

Design idea: single‑trit intuition first. Extending to multi‑trit words is a natural next step.
