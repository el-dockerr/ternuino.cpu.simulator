#ifndef GATES_BINARY_CIRCUITS_H
#define GATES_BINARY_CIRCUITS_H

#include "gates/netlist.h"

// Binary CMOS baseline built from INV, NAND, NOR and tie cells, using the
// same netlist framework and cost model style as the ternary circuits.

// Full adder from 9 two-input NAND gates: a + b + cin = 2 * cout + sum.
// cin -> cout passes through 2 gates.
void bin_full_adder(netlist_t *nl, net_t a, net_t b, net_t cin, net_t *sum, net_t *cout);

// Complete netlists with primary inputs/outputs (same port order as the
// ternary builders):
//   full adder:  inputs a, b, cin                      outputs sum, cout
//   ripple:      inputs a[0..n-1], b[0..n-1], cin      outputs s[0..n-1], cout
void bin_build_full_adder(netlist_t *nl);
void bin_build_ripple_adder(netlist_t *nl, int n);

#endif // GATES_BINARY_CIRCUITS_H
