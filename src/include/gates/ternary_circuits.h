#ifndef GATES_TERNARY_CIRCUITS_H
#define GATES_TERNARY_CIRCUITS_H

#include "gates/netlist.h"

// Balanced-ternary arithmetic circuits built only from STI, PTI, NTI, MIN,
// MAX and tie cells.
//
// Indicator ("literal") signals are binary-valued ternary nets: +1 means
// true, -1 means false.

// Lazily-built literals of one ternary net, so each one is created at most
// once and shared by every term that uses it.
typedef struct {
    net_t x;
    net_t nti;      // NTI(x)        = is_neg(x)
    net_t pti;      // PTI(x)        = not_pos(x)
    net_t sti_nti;  // STI(NTI(x))   = not_neg(x)
    net_t sti_pti;  // STI(PTI(x))   = is_pos(x)
    net_t zero;     // MIN(not_neg, not_pos) = is_zero(x)
} tlit_t;

void tlit_init(tlit_t *l, net_t x);
net_t tlit_is_neg(netlist_t *nl, tlit_t *l);
net_t tlit_is_pos(netlist_t *nl, tlit_t *l);
net_t tlit_not_neg(netlist_t *nl, tlit_t *l);
net_t tlit_not_pos(netlist_t *nl, tlit_t *l);
net_t tlit_is_zero(netlist_t *nl, tlit_t *l);
net_t tlit_equals(netlist_t *nl, tlit_t *l, int value);  // +1 iff x == value

// Balanced reduction tree of 2-input MIN or MAX gates over n nets.
net_t tree_reduce(netlist_t *nl, gate_type_t type, const net_t *nets, int n);

// Generic sum-of-products synthesis of any n-input ternary function:
//   f = MAX over input vectors v with f(v) != -1 of
//         MIN(lit(x0 == v0), ..., lit(xn-1 == vn-1))     if f(v) == +1
//         MIN(that minterm, const 0)                     if f(v) ==  0
// table is indexed by sum_k (x_k + 1) * 3^k. Literals are shared via lits.
net_t tsynth_sop(netlist_t *nl, tlit_t *lits, int n, const int8_t *table);

typedef enum {
    TADD_V1_SOP,      // canonical sum of products (reference)
    TADD_V2_SORTED,   // sorting network + literal terms (reduced)
    TADD_VERSION_COUNT
} tadd_version_t;

const char *tadd_version_name(tadd_version_t v);

// Half adder: a + b = 3 * carry + sum.
void tern_half_adder(netlist_t *nl, tadd_version_t v, net_t a, net_t b, net_t *sum, net_t *carry);

// Full adder: a + b + cin = 3 * cout + sum.
void tern_full_adder(netlist_t *nl, tadd_version_t v, net_t a, net_t b, net_t cin,
                     net_t *sum, net_t *cout);

// Complete netlists with primary inputs/outputs:
//   half adder:  inputs a, b              outputs sum, carry
//   full adder:  inputs a, b, cin         outputs sum, cout
//   ripple:      inputs a[0..n-1], b[0..n-1], cin (index 0 = least significant)
//                outputs s[0..n-1], cout
void tern_build_half_adder(netlist_t *nl, tadd_version_t v);
void tern_build_full_adder(netlist_t *nl, tadd_version_t v);
void tern_build_ripple_adder(netlist_t *nl, tadd_version_t v, int n);

#endif // GATES_TERNARY_CIRCUITS_H
