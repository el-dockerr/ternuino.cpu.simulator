# 03 – Ternary Logic: TAND, TOR, TNOT

Ternary logic mirrors intuition:
- TAND(a, b) = min(a, b)
- TOR(a, b)  = max(a, b)
- TNOT(a)    = −a

Read them as “most pessimistic wins” (AND) and “most optimistic wins” (OR).

Playground: in the GUI, open the “Ternary Logic” tab. Flip a and b between −1/0/+1 and watch outputs.

Beyond the basics:
- Three‑way compare and sign are very natural in ternary:
  - `sign(x) ∈ {−1,0,+1}` in one step
  - Compare returns a trit instead of a boolean
- This makes multi‑way branching (negative/zero/positive) straightforward.
