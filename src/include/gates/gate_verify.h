#ifndef GATES_GATE_VERIFY_H
#define GATES_GATE_VERIFY_H

#include <stdio.h>

#define GATE_VERIFY_SEED 0x7E57AB1Eu
#define GATE_VERIFY_RANDOM_CASES 20000

// Runs all gate-level checks and prints one PASS/FAIL line per check:
//  - truth table of every primitive (direct and through a netlist)
//  - ternary half and full adders (both versions), exhaustive
//  - ternary ripple-carry adders: N=4 exhaustive, N=27 and N=41 randomized
//    with a fixed seed plus carry-chain edge cases
//  - binary full adder exhaustive, ripple adders N=4 exhaustive, N=43 and
//    N=64 randomized
// Returns the number of failed checks.
int gate_verify_all(FILE *out);

#endif // GATES_GATE_VERIFY_H
