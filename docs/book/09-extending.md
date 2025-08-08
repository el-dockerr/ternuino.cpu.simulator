# 09 – Extending the CPU and the Assembler

Everything here is small on purpose so you can extend it.

Ideas:
- Add `TSEL dest, cond, x, y, z` (3‑way select) or `TMUX3`
- Introduce simple memory read/write and a data memory space
- Define flags (e.g., Z/N) and a status register for more branches
- Grow registers into multi‑trit words for word‑level ops

Where to look in the code:
- `library/ternuino.py` – CPU step/run loop
- `library/assembler.py` – instruction parsing and label resolution
- `library/tritlogic.py` – TAND/TOR/TNOT
- `library/tritarith.py` – TSIGN/TABS/TSHL3/TSHR3/TCMPR helpers
- `library/tritword.py` – multi‑trit representation
