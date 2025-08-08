# 10 – FAQ and Glossary

## FAQ

Q: Why ternary instead of binary?
A: It’s more information‑dense and maps naturally to negative/zero/positive. It also makes three‑way logic and sign/compare elegant.

Q: Is this “realistic” hardware?
A: It’s a didactic simulator. The conceptual transistor tab is for intuition, not electrical accuracy.

Q: Can I write my own instructions?
A: Yes! Update the assembler and CPU, and consider adding helpers in `tritarith.py` or `tritlogic.py`.

## Glossary
- Trit: A ternary digit, value from {−1, 0, +1}
- Balanced ternary: A base‑3 numeral system using −1, 0, +1 as digits
- TAND/TOR/TNOT: Ternary logic ops (min/max/negate)
- TSIGN/TCMPR: Sign and 3‑way compare producing a trit
- TSHL3/TSHR3: Multiply/divide by 3 in base‑3
- TritWord: A fixed‑length sequence of trits with to/from int conversion
