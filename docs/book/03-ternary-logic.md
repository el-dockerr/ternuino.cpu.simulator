# 03 – Ternary Logic: TAND, TOR, TNOT

If binary logic is a strict yes/no, ternary logic adds a comfy middle seat: maybe/neutral. We’ll use three values (trits): −1, 0, +1. Keep this mental model:

- −1 = negative (no, false, pessimistic)
- 0  = neutral (unknown, don’t care)
- +1 = positive (yes, true, optimistic)

Our three core operators are surprisingly simple:

- TAND(a, b) = min(a, b)
- TOR(a, b)  = max(a, b)
- TNOT(a)    = −a

Read them as “most pessimistic wins” (AND) and “most optimistic wins” (OR). Negation flips the attitude.

Playground idea: open the Visualizer’s “Ternary Logic” tab and flip a, b between −1/0/+1 to see results live.

## Quick examples (feel the behavior)

- TAND(+1, 0) = 0        → any uncertainty downgrades AND
- TAND(−1, +1) = −1      → one strong no makes the whole thing no
- TOR(0, −1) = 0         → any open door upgrades OR
- TOR(+1, −1) = +1       → one strong yes makes the whole thing yes
- TNOT(+1) = −1, TNOT(0) = 0, TNOT(−1) = +1

Because we use min/max with the total order −1 < 0 < +1, many beautiful algebraic properties fall out for free.

## Algebra in one page (useful identities)

Let a, b, c ∈ {−1, 0, +1}.

- Commutative: a TAND b = b TAND a; a TOR b = b TOR a
- Associative: (a TAND b) TAND c = a TAND (b TAND c); same for TOR
- Idempotent:  a TAND a = a; a TOR a = a
- Absorption:  a TOR (a TAND b) = a; a TAND (a TOR b) = a
- Identities:  a TAND +1 = a; a TOR −1 = a
- Dominance:   a TAND −1 = −1; a TOR +1 = +1
- De Morgan:   TNOT(TAND(a, b)) = TOR(TNOT(a), TNOT(b)); and dually for TOR

These are exactly the lattice laws for min/max with an order‑reversing negation (a → −a).

## Three‑way decisions (sign and compare)

Ternary shines when you want “negative / zero / positive” in one step.

- sign(x) ∈ {−1, 0, +1}
- compare(a, b) returns sign(a − b) ∈ {−1, 0, +1}

Your Python helpers live in `library.tritarith`:

- `tsign(x)` → −1, 0, +1
- `tcmpr(a, b)` → sign(a − b)
- `tshl3(v)` / `tshr3(v)` → multiply/divide by 3 (base‑3 shift)

You’ll use them a lot when writing higher‑level ternary algorithms.

## Using the Python library (hands‑on)

The building blocks live in `library.tritlogic` and `library.tritarith`. Here are small, copy‑pasteable examples.

### Truth grid for TAND/TOR

```python
from itertools import product
from library.tritlogic import TritLogic

TRIT = (-1, 0, 1)

rows = []
for a, b in product(TRIT, TRIT):
  rows.append((a, b, TritLogic.tand(a, b), TritLogic.tor(a, b), TritLogic.tnot(a)))

for a, b, tand, tor, tnot_a in rows:
  print(f"a={a:>2} b={b:>2} | TAND={tand:>2} TOR={tor:>2} TNOT(a)={tnot_a:>2}")
```

### Verify the core laws quickly

```python
from itertools import product
from library.tritlogic import TritLogic as TL

TRIT = (-1, 0, 1)

def ok(name, cond):
  if not cond: raise AssertionError(name)

for a, b, c in product(TRIT, TRIT, TRIT):
  ok("comm TOR", TL.tor(a,b) == TL.tor(b,a))
  ok("comm TAND", TL.tand(a,b) == TL.tand(b,a))
  ok("assoc TOR", TL.tor(TL.tor(a,b),c) == TL.tor(a, TL.tor(b,c)))
  ok("assoc TAND", TL.tand(TL.tand(a,b),c) == TL.tand(a, TL.tand(b,c)))
  ok("idem TOR", TL.tor(a,a) == a)
  ok("idem TAND", TL.tand(a,a) == a)
  ok("absorb1", TL.tor(a, TL.tand(a,b)) == a)
  ok("absorb2", TL.tand(a, TL.tor(a,b)) == a)
  ok("ident TAND", TL.tand(a, +1) == a)
  ok("ident TOR", TL.tor(a, -1) == a)
  ok("dom TAND", TL.tand(a, -1) == -1)
  ok("dom TOR", TL.tor(a, +1) == +1)
  ok("deMorgan1", TL.tnot(TL.tand(a,b)) == TL.tor(TL.tnot(a), TL.tnot(b)))
  ok("deMorgan2", TL.tnot(TL.tor(a,b)) == TL.tand(TL.tnot(a), TL.tnot(b)))

print("All good ✔")
```

### Three‑way branching with tsign and tcmpr

```python
from library.tritarith import tsign, tcmpr

def describe(x):
  s = tsign(x)
  return { -1:"negative", 0:"zero", 1:"positive" }[s]

print(describe(-42))  # negative
print(describe(0))    # zero
print(describe(7))    # positive

print(tcmpr(3, 10))   # -1 (3 < 10)
print(tcmpr(5, 5))    # 0
print(tcmpr(9, -1))   # +1 (9 > -1)
```

### Aggregating many trits (consensus/majority)

For a quick “majority attitude,” sum trits and take the sign.

```python
from library.tritarith import tsign

def majority(*trits):
  return tsign(sum(trits))

print(majority(1, 1, -1))  # +1 → mostly positive
print(majority(1, 0, -1))  # 0  → balanced
print(majority(-1, -1, 0)) # -1 → mostly negative
```

### Working with TritWord (element‑wise logic)

`TritWord` stores a fixed‑width vector of trits and prints them as “−/0/+” (in code as -/0/+). You can map logic over the digits:

```python
from library.tritword import TritWord
from library.tritlogic import TritLogic as TL

w = TritWord.from_int(37, size=6)   # e.g., ++0+..
print("w:", w, "=", w.to_int())

# Element‑wise NOT
w_not = TritWord([TL.tnot(t) for t in w.trits], size=len(w.trits))
print("TNOT(w):", w_not)

# Element‑wise AND/OR of two words
x = TritWord.from_int(12, size=6)
tand = TritWord([TL.tand(a,b) for a,b in zip(w.trits, x.trits)], size=6)
tor  = TritWord([TL.tor(a,b)  for a,b in zip(w.trits, x.trits)], size=6)
print("w:", w)
print("x:", x)
print("w TAND x:", tand)
print("w TOR  x:", tor)
```

Tip: for arithmetic, see also `tabs`, `tshl3`, `tshr3` in `library.tritarith`.

## From Python to CPU instructions

In the Ternuino instruction set, the same ideas appear as opcodes:

- `TAND A, B` → A := min(A, B)
- `TOR  A, B` → A := max(A, B)
- `TNOT A`    → A := −A
- `TSIGN A`   → A := sign(A)
- `TCMPR A, B`→ A := sign(A − B)

That “three‑way” nature also powers branching: `TJZ` (jump if zero), `TJN` (jump if negative), `TJP` (jump if positive).

## Where this leads

Once you internalize −/0/+ as ordered values, ternary logic becomes second nature. You’ll write fewer special‑case branches, compare in one step, and compose decisions with min/max like snapable bricks. In the next chapters we’ll use these primitives to build arithmetic, control flow, and actual programs on the Ternuino.
