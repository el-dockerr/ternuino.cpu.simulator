# 02 – Ternary Numbers 101 (Balanced Ternary)

We’ll use balanced ternary: each place holds a trit value from {−1, 0, +1}. We’ll write them as −, 0, + for short. Example: “+0−” means (+1)·3² + 0·3¹ + (−1)·3⁰ = 9 − 1 = 8.

Why balanced?
- Natural sign: negative values don’t need a separate sign bit
- Symmetry: adding and subtracting feel consistent
- Simpler logic for sign/compare operations

Converting integers → balanced ternary (intuition):
- Divide by 3, but allow remainders −1, 0, +1
- “Carry” can be negative or positive

In code, we expose `TritWord` which converts both ways:
- `TritWord.from_int(value, size=6)`
- `TritWord(...).to_int()`

Try it in Python: create a few `TritWord`s and print them; you’ll see sequences of −/0/+.
