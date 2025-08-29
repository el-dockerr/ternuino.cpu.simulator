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

Below are compact "thruttables" (truth tables for trits) for the three operators. Read each table as inputs → result.

#### TAND (min, "most pessimistic wins")
| a \ b | −1 | 0  | +1 |
|-------|----|----|----|
| −1    | −1 | −1 | −1 |
| 0     | −1 | 0  | 0  |
| +1    | −1 | 0  | +1 |

#### TOR (max, "most optimistic wins")
| a \ b | −1 | 0  | +1 |
|-------|----|----|----|
| −1    | −1 | 0  | +1 |
| 0     | 0  | 0  | +1 |
| +1    | +1 | +1 | +1 |

#### TNOT (negation, flips attitude)
| a  | TNOT(a) |
|----|---------|
| −1 | +1      |
| 0  | 0       |
| +1 | −1      |

Use these tables to quickly look up results or to reason about compositions: TAND picks the lower (more pessimistic) value, TOR the higher (more optimistic), and TNOT simply flips the sign.

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


## Where this leads

Once you internalize −/0/+ as ordered values, ternary logic becomes second nature. You’ll write fewer special‑case branches, compare in one step, and compose decisions with min/max like snapable bricks. In the next chapters we’ll use these primitives to build arithmetic, control flow, and actual programs on the Ternuino.
