# Gate-level T-CMOS layer

Status: **Milestone 1: ternary full adder.** The ISA simulator still does its
arithmetic with C integers. This layer sits underneath it and models balanced
ternary logic at the gate level, using the ternary CMOS (T-CMOS) primitives
published by Prof. Kyung Rok Kim's group at UNIST (Jeong et al.,
*"Tunnelling-based ternary metal-oxide-semiconductor technology"*, Nature
Electronics 2019, and the later 28-nm T-CMOS work). The long-term goal is to
run the ISA on a datapath built from these gates and report cycles, energy and
area next to an equivalent binary design.

> **All delay, energy, area and leakage numbers are currently PLACEHOLDERS.**
> Gate counts, logic depths and the functional verification are exact. The
> physical numbers only become meaningful once real cost data is loaded (see
> [Plugging in real cost data](#plugging-in-real-cost-data)).

## Running it

```bash
cd src
make && ./build/ternuino --gate-report          # Linux/macOS (or: make gate-report)
build-all.bat test                               # Windows
./build/ternuino --gate-report --gate-costs config/gate_costs.cfg
```

`--gate-report` runs every verification check, then prints the metrics
tables. The exit code is non-zero if any check fails (CI runs it). The normal
interactive and `.asm` modes are unchanged.

## Encoding

Balanced ternary, mapped onto the three T-CMOS voltage levels:

| trit | voltage | symbol |
|-----:|:-------:|:------:|
|  -1  | 0 V     | `-`    |
|   0  | VDD/2   | `0`    |
|  +1  | VDD     | `+`    |

## Primitives

These are the only cells a ternary netlist may contain:

| gate | function | -1 | 0 | +1 | placeholder transistors |
|------|----------|:--:|:-:|:--:|:-----------------------:|
| STI  | standard ternary inverter, `-x` | +1 | 0 | -1 | 2 |
| PTI  | positive ternary inverter        | +1 | +1 | -1 | 2 |
| NTI  | negative ternary inverter        | +1 | -1 | -1 | 2 |
| MIN  | `min(a, b)` = ISA `TAND`         | | | | 6 (TNAND + STI) |
| MAX  | `max(a, b)` = ISA `TOR`          | | | | 6 (TNOR + STI) |
| TIE- / TIE0 / TIE+ | constants -1 / 0 / +1 | | | | 2 |

The binary baseline uses INV, NAND, NOR and TIE_LO / TIE_HI.

Gates are evaluated only through the truth tables in
`src/src/gates/netlist.c`. The gate layer itself never does integer
arithmetic.

## Code layout

```
src/include/gates/          src/src/gates/
  netlist.h                   netlist.c            netlist, truth tables, evaluation
  cost.h                      cost.c               cost model, PLACEHOLDER defaults, file loader
  metrics.h                   metrics.c            counts, depth, static timing, activity energy
  ternary_circuits.h          ternary_circuits.c   literals, SOP synthesis, adders
  binary_circuits.h           binary_circuits.c    9-NAND full adder, ripple adder
  gate_verify.h               gate_verify.c        all functional checks
  gate_report.h               gate_report.c        --gate-report output
src/config/gate_costs.cfg                          editable cost data (placeholders)
```

### Netlist model

Each gate drives exactly one net, and the net id is the gate's index. A gate
can only reference nets that already exist, so insertion order is a
topological order and one forward pass evaluates the netlist. Ternary and
binary gates cannot be mixed in one netlist. Gates check their input values
at evaluation time and abort on anything outside their logic domain. Tie
cells are created once per constant per netlist and shared.

## Adder construction

### Literals (indicator signals)

An indicator is a ternary net that is +1 for true and -1 for false:

| literal   | circuit                    | gates |
|-----------|----------------------------|:-----:|
| is_neg(x) | `NTI(x)`                   | 1 |
| not_pos(x)| `PTI(x)`                   | 1 |
| is_pos(x) | `STI(PTI(x))`              | 2 |
| not_neg(x)| `STI(NTI(x))`              | 2 |
| is_zero(x)| `MIN(STI(NTI(x)), PTI(x))` | 3 |

`tlit_t` creates these lazily and shares them between all terms that use
them.

A ternary output is assembled from indicators as
`MAX(pos_ind, MIN(zero_ind, 0))`. That is +1 if `pos_ind` holds, otherwise 0
if `zero_ind` holds, otherwise -1.

### v1-sop: canonical sum of products (reference)

`tsynth_sop()` turns any n-input truth table into
`f = MAX over rows v with f(v) != -1 of MIN(lit(x0==v0), ..., lit(xn-1==vn-1))`.
Rows with output 0 are additionally wrapped in `MIN(term, 0)`. The adders'
truth tables are written out as constants in `ternary_circuits.c`. The netlist
is straightforward and obviously correct, but it's large.

### v2-sorted: sorting network + decoding (reduced)

The full adder's outputs depend only on the multiset `{a, b, cin}`. So v2
first sorts the inputs with a MIN/MAX network (6 gates). `cin` enters last,
which keeps the ripple carry path short:

```
m = MIN(a,b)   M = MAX(a,b)
lo = MIN(m,cin)   hi = MAX(M,cin)   mid = MAX(m, MIN(M,cin))
```

After sorting, only 10 multisets remain, and each output is a short
expression over literals of `lo`, `mid` and `hi`:

* `cout = +1` iff `mid = +1` and `lo >= 0`, and `cout = -1` iff `mid = -1` and `hi <= 0`:
  `cout = MAX(MIN(mid, MAX(not_neg(lo), 0)), MIN(is_pos(hi), 0))`
* `sum = +1` for sorted triples `(-1,-1,0)`, `(-1,+1,+1)`, `(0,0,+1)`
* `sum = 0` for `(-1,-1,-1)`, `(+1,+1,+1)`, `(0,0,0)`, `(-1,0,+1)`

The half adder works the same way on `lo = MIN(a,b)` and `hi = MAX(a,b)`. Its
carry is simply `median(a, b, 0) = MAX(lo, MIN(hi, 0))`.

| circuit                 | logic gates | ties | depth | cin->cout depth |
|-------------------------|:-----------:|:----:|:-----:|:---------------:|
| half adder v1-sop       | 46          | 1    | 8     | -  |
| half adder v2-sorted    | 18          | 1    | 7     | -  |
| full adder v1-sop       | 164         | 1    | 11    | 11 |
| full adder v2-sorted    | 38          | 1    | 10    | 6  |
| binary FA (9 NAND)      | 9           | 0    | 6     | 2  |

Full adder v2 breakdown: 6 STI, 3 PTI, 3 NTI, 15 MIN, 11 MAX.

### Ripple-carry adder

`tern_build_ripple_adder(nl, version, n)` chains n full adders, least
significant digit first. Ports: inputs `a[0..n-1], b[0..n-1], cin`; outputs
`s[0..n-1], cout`. The binary ripple adder has the same port layout.

## Verification

`gate_verify.c` (run by `--gate-report`) checks:

* every primitive's truth table, both directly and through a one-gate
  netlist, with MIN/MAX also checked against the ISA's `trit_and`/`trit_or`
* the tie cells
* half and full adders (both versions): exhaustive over all inputs, checking
  `a + b + cin == 3*cout + sum`
* 4-trit ripple adder: exhaustive (3^9 = 19683 vectors)
* 27- and 41-trit ripple adders: 6 carry-chain edge cases plus 20000
  random vectors from a fixed-seed splitmix64. The reference uses a
  128-bit integer because 41-trit sums exceed `int64_t`.
* the binary full adder (exhaustive), the 4-bit ripple adder (exhaustive),
  and the 43- and 64-bit ripple adders (random)

## Metrics

| column     | meaning |
|------------|---------|
| gates      | logic cells, excluding inputs and tie cells |
| xtors      | sum of per-cell transistor counts from the cost model |
| area       | sum of cell areas (no routing, no utilisation factor) |
| depth      | gate levels on the longest input->output path |
| delay      | static critical-path delay: sum of cell delays, no wire/fan-out load |
| c-dep/c-del| the same, measured only from carry-in to carry-out |
| E_all      | sum of per-cell energies, as if every cell toggled once at full swing |
| E_act      | average switching energy per operation over 2000 random input vectors. A ternary full-swing transition (-1<->+1) counts 1.0, a half swing (one VDD/2 step) counts `half_swing_factor` |
| E_leak     | total leakage power x critical-path delay, i.e. one operation per clock period |

The equal-range comparison sets 27 trits against 43 bits
(3^27 = 7.6e12 vs 2^43 = 8.8e12) and 41 trits against 64 bits
(3^41 = 3.6e19 vs 2^64 = 1.8e19). `T/B` is the total ratio and `T/B/digit`
the per-digit ratio. Ternary wins a metric when `T/B < 1`, i.e. when one trit
stage costs less than about log2(3) ≈ 1.585 bit stages.

### Current results (placeholder costs)

Exact structural numbers (independent of the cost model):

| design | logic gates | transistors | depth | carry depth |
|--------|:-----------:|:-----------:|:-----:|:-----------:|
| 27-trit ripple, v2 | 1026 | 4862 | 166 | 162 |
| 43-bit ripple      | 387  | 1548 | 90  | 86  |
| 41-trit ripple, v2 | 1558 | 7382 | 250 | 246 |
| 64-bit ripple      | 576  | 2304 | 132 | 128 |

Built only from these primitives, a trit stage currently needs about 4.2x
the gates and about 3x the logic depth of a bit stage. That's well above the
1.585 break-even. The placeholder cost model doesn't change this ordering,
but real cell data could narrow the gap (see limitations).

## Plugging in real cost data

1. Copy `src/config/gate_costs.cfg` and fill in one line per cell:

   ```
   # gate  delay_ps  energy_fj  area_um2  leakage_nw  [transistors]
   STI     10.0      0.12       0.25      5.0         2
   ```

   * `delay_ps`: propagation delay at a fixed, documented load (e.g. FO4)
   * `energy_fj`: dynamic energy per **full-swing** output transition
   * `area_um2`: cell area
   * `leakage_nw`: average static power. For T-CMOS cells this includes the
     intentional tunnelling current that holds the VDD/2 state.
   * `half_swing_factor`: energy of a VDD/2 step relative to a full swing

2. Set `placeholder no` once all values are real. This removes the warning
   banners.
3. Run `ternuino --gate-report --gate-costs path/to/file.cfg`. No recompile is
   needed. Cells you don't list keep their built-in values.

Use the same technology node and characterisation conditions for the binary
cells, or the comparison is meaningless.

## Limitations

* **Placeholder costs.** See above. Only the transistor counts follow a
  published structure, and even those assume MIN/MAX = TNAND/TNOR + STI.
* **Simple gates only.** Neither side uses complex cells (binary AOI/mirror
  adders, ternary multi-input or decoder cells). The binary FA is the
  textbook 9-NAND design, not an optimised 28-transistor mirror adder, so both
  sides are "unoptimised standard cells".
* **No electrical modelling.** There's no fan-out or wire load, no slope
  dependence, and no noise margin. The shared tie cells have unbounded
  fan-out.
* **Zero-delay activity.** Transitions are counted between settled states, so
  glitch energy is ignored.
* **State-independent leakage.** A T-CMOS cell leaks most in the VDD/2 state.
  The model uses one average value per cell.
* **Ripple only.** No carry-lookahead or carry-select structures yet.
* Ternary and binary netlists are not mixed. There are no level converters.

## Open questions

* Which cells does the UNIST T-CMOS library actually provide? If TNAND/TNOR
  (inverting) are native, MIN/MAX cost an extra STI each. Allowing TNAND/TNOR
  as primitives would likely remove many STI levels.
* Are PTI/NTI realised as skewed-threshold binary-style inverters (cheap) or
  as T-CMOS cells? This changes their cost and leakage substantially.
* How should the VDD/2 tie cell be implemented (STI with output fed back to
  input, resistive divider, ...) and what does it leak?
* What is the energy of a half-swing transition in T-CMOS? The 0.5 factor is
  a first-order C·V·ΔV guess.

## Next steps

* **Carry-optimised full adder.** Precompute `cout(cin=-1/0/+1)` from
  `(a, b)` and select with literals of `cin`. Alternating carry polarity
  between stages (passing `-cout` on) makes every carry literal a single
  PTI/NTI and should bring the carry path from 6 to about 4 levels per trit.
* Ternary latch / storage cell (Milestone 2), then a register file and the
  datapath that executes the ISA.
* Cycle, energy and area accounting per executed instruction.
