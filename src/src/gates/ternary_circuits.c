#include "gates/ternary_circuits.h"
#include <stdlib.h>

// ---------------------------------------------------------------------------
// Literals
// ---------------------------------------------------------------------------

void tlit_init(tlit_t *l, net_t x) {
    l->x = x;
    l->nti = l->pti = l->sti_nti = l->sti_pti = l->zero = NET_NONE;
}

net_t tlit_is_neg(netlist_t *nl, tlit_t *l) {
    if (l->nti == NET_NONE) l->nti = netlist_gate1(nl, G_NTI, l->x);
    return l->nti;
}

net_t tlit_not_pos(netlist_t *nl, tlit_t *l) {
    if (l->pti == NET_NONE) l->pti = netlist_gate1(nl, G_PTI, l->x);
    return l->pti;
}

net_t tlit_not_neg(netlist_t *nl, tlit_t *l) {
    if (l->sti_nti == NET_NONE) l->sti_nti = netlist_gate1(nl, G_STI, tlit_is_neg(nl, l));
    return l->sti_nti;
}

net_t tlit_is_pos(netlist_t *nl, tlit_t *l) {
    if (l->sti_pti == NET_NONE) l->sti_pti = netlist_gate1(nl, G_STI, tlit_not_pos(nl, l));
    return l->sti_pti;
}

net_t tlit_is_zero(netlist_t *nl, tlit_t *l) {
    if (l->zero == NET_NONE) {
        l->zero = netlist_gate2(nl, G_MIN, tlit_not_neg(nl, l), tlit_not_pos(nl, l));
    }
    return l->zero;
}

net_t tlit_equals(netlist_t *nl, tlit_t *l, int value) {
    if (value < 0) return tlit_is_neg(nl, l);
    if (value > 0) return tlit_is_pos(nl, l);
    return tlit_is_zero(nl, l);
}

// ---------------------------------------------------------------------------
// Generic helpers
// ---------------------------------------------------------------------------

net_t tree_reduce(netlist_t *nl, gate_type_t type, const net_t *nets, int n) {
    if (n == 1) return nets[0];
    int half = n / 2;
    net_t left = tree_reduce(nl, type, nets, half);
    net_t right = tree_reduce(nl, type, nets + half, n - half);
    return netlist_gate2(nl, type, left, right);
}

net_t tsynth_sop(netlist_t *nl, tlit_t *lits, int n, const int8_t *table) {
    int rows = 1;
    for (int k = 0; k < n; k++) rows *= 3;

    net_t *terms = malloc(sizeof(net_t) * (size_t)rows);
    net_t factors[8];
    int term_count = 0;

    for (int row = 0; row < rows; row++) {
        int8_t out = table[row];
        if (out == -1) continue;

        int digits = row;
        for (int k = 0; k < n; k++) {
            factors[k] = tlit_equals(nl, &lits[k], digits % 3 - 1);
            digits /= 3;
        }
        net_t term = tree_reduce(nl, G_MIN, factors, n);
        if (out == 0) term = netlist_gate2(nl, G_MIN, term, netlist_const(nl, 0));
        terms[term_count++] = term;
    }

    net_t f = term_count ? tree_reduce(nl, G_MAX, terms, term_count) : netlist_const(nl, -1);
    free(terms);
    return f;
}

// ---------------------------------------------------------------------------
// Truth tables for the V1 (canonical SOP) adders. Specification only; the
// circuits are verified against an integer reference in gate_verify.c.
// Half adder index: (a+1) + 3*(b+1).  Full adder index: + 9*(cin+1).
// ---------------------------------------------------------------------------

static const int8_t HA_SUM[9]   = { +1, -1,  0,   -1,  0, +1,    0, +1, -1 };
static const int8_t HA_CARRY[9] = { -1,  0,  0,    0,  0,  0,    0,  0, +1 };

static const int8_t FA_SUM[27] = {
     0, +1, -1,   +1, -1,  0,   -1,  0, +1,   // cin = -1
    +1, -1,  0,   -1,  0, +1,    0, +1, -1,   // cin =  0
    -1,  0, +1,    0, +1, -1,   +1, -1,  0,   // cin = +1
};
static const int8_t FA_COUT[27] = {
    -1, -1,  0,   -1,  0,  0,    0,  0,  0,
    -1,  0,  0,    0,  0,  0,    0,  0, +1,
     0,  0,  0,    0,  0, +1,    0, +1, +1,
};

const char *tadd_version_name(tadd_version_t v) {
    switch (v) {
        case TADD_V1_SOP: return "v1-sop";
        case TADD_V2_SORTED: return "v2-sorted";
        default: return "?";
    }
}

// ---------------------------------------------------------------------------
// V2 helpers
// ---------------------------------------------------------------------------

static net_t min2(netlist_t *nl, net_t a, net_t b) { return netlist_gate2(nl, G_MIN, a, b); }
static net_t max2(netlist_t *nl, net_t a, net_t b) { return netlist_gate2(nl, G_MAX, a, b); }

// Maps indicator nets to a ternary output: +1 if pos_ind, else 0 if zero_ind,
// else -1. pos_ind and zero_ind must never both be true.
static net_t encode_output(netlist_t *nl, net_t pos_ind, net_t zero_ind) {
    return max2(nl, pos_ind, min2(nl, zero_ind, netlist_const(nl, 0)));
}

// Half adder on the sorted pair lo = MIN(a,b), hi = MAX(a,b):
//   carry = median(a, b, 0) = MAX(lo, MIN(hi, 0))
//   sum   = +1 for (lo,hi) in {(-1,-1), (0,+1)}
//            0 for (lo,hi) in {(-1,+1), (0,0)}
//           -1 otherwise
static void half_adder_v2(netlist_t *nl, net_t a, net_t b, net_t *sum, net_t *carry) {
    net_t lo = min2(nl, a, b);
    net_t hi = max2(nl, a, b);
    tlit_t L, H;
    tlit_init(&L, lo);
    tlit_init(&H, hi);

    net_t pos = max2(nl,
        tlit_is_neg(nl, &H),
        min2(nl, min2(nl, tlit_not_neg(nl, &L), tlit_not_pos(nl, &L)), tlit_is_pos(nl, &H)));
    net_t zero = max2(nl,
        min2(nl, tlit_is_neg(nl, &L), tlit_is_pos(nl, &H)),
        min2(nl, tlit_not_neg(nl, &L), tlit_not_pos(nl, &H)));

    *sum = encode_output(nl, pos, zero);
    *carry = max2(nl, lo, min2(nl, hi, netlist_const(nl, 0)));
}

// Full adder on the sorted triple lo <= mid <= hi of (a, b, cin). cin enters
// the sorting network last so the carry chain of a ripple adder is short.
//   sum  = +1 for (lo,mid,hi) in {(-1,-1,0), (-1,+1,+1), (0,0,+1)}
//           0 for (-1,-1,-1), (+1,+1,+1), (0,0,0), (-1,0,+1)
//          -1 otherwise
//   cout = +1 iff mid = +1 and lo >= 0, -1 iff mid = -1 and hi <= 0, else 0
//        = MAX(MIN(mid, MAX(not_neg(lo), 0)), MIN(is_pos(hi), 0))
static void full_adder_v2(netlist_t *nl, net_t a, net_t b, net_t cin, net_t *sum, net_t *cout) {
    net_t m = min2(nl, a, b);
    net_t M = max2(nl, a, b);
    net_t lo = min2(nl, m, cin);
    net_t hi = max2(nl, M, cin);
    net_t mid = max2(nl, m, min2(nl, M, cin));

    tlit_t L, D, H;
    tlit_init(&L, lo);
    tlit_init(&D, mid);
    tlit_init(&H, hi);

    net_t zero_hi = min2(nl, tlit_not_neg(nl, &H), tlit_not_pos(nl, &H));
    net_t zero_mid = min2(nl, tlit_not_neg(nl, &D), tlit_not_pos(nl, &D));

    net_t pos_terms[3] = {
        min2(nl, tlit_is_neg(nl, &D), zero_hi),                                    // (-1,-1, 0)
        min2(nl, tlit_is_neg(nl, &L), tlit_is_pos(nl, &D)),                        // (-1,+1,+1)
        min2(nl, min2(nl, tlit_not_neg(nl, &L), tlit_not_pos(nl, &D)), tlit_is_pos(nl, &H)), // (0,0,+1)
    };
    net_t zero_terms[4] = {
        tlit_is_neg(nl, &H),                                                       // (-1,-1,-1)
        tlit_is_pos(nl, &L),                                                       // (+1,+1,+1)
        min2(nl, tlit_not_neg(nl, &L), tlit_not_pos(nl, &H)),                      // ( 0, 0, 0)
        min2(nl, min2(nl, tlit_is_neg(nl, &L), tlit_is_pos(nl, &H)), zero_mid),    // (-1, 0,+1)
    };
    // The (-1,0,+1) term arrives last, so it is merged last instead of using a
    // balanced tree (saves one level on the sum path, same gate count).
    net_t zero_any = max2(nl, max2(nl, max2(nl, zero_terms[0], zero_terms[1]), zero_terms[2]),
                          zero_terms[3]);
    *sum = encode_output(nl, tree_reduce(nl, G_MAX, pos_terms, 3), zero_any);

    net_t zero = netlist_const(nl, 0);
    net_t up = min2(nl, mid, max2(nl, tlit_not_neg(nl, &L), zero));
    net_t down = min2(nl, tlit_is_pos(nl, &H), zero);
    *cout = max2(nl, up, down);
}

// ---------------------------------------------------------------------------
// Public builders
// ---------------------------------------------------------------------------

void tern_half_adder(netlist_t *nl, tadd_version_t v, net_t a, net_t b, net_t *sum, net_t *carry) {
    if (v == TADD_V2_SORTED) {
        half_adder_v2(nl, a, b, sum, carry);
        return;
    }
    tlit_t lits[2];
    tlit_init(&lits[0], a);
    tlit_init(&lits[1], b);
    *sum = tsynth_sop(nl, lits, 2, HA_SUM);
    *carry = tsynth_sop(nl, lits, 2, HA_CARRY);
}

void tern_full_adder(netlist_t *nl, tadd_version_t v, net_t a, net_t b, net_t cin,
                     net_t *sum, net_t *cout) {
    if (v == TADD_V2_SORTED) {
        full_adder_v2(nl, a, b, cin, sum, cout);
        return;
    }
    tlit_t lits[3];
    tlit_init(&lits[0], a);
    tlit_init(&lits[1], b);
    tlit_init(&lits[2], cin);
    *sum = tsynth_sop(nl, lits, 3, FA_SUM);
    *cout = tsynth_sop(nl, lits, 3, FA_COUT);
}

void tern_build_half_adder(netlist_t *nl, tadd_version_t v) {
    net_t a = netlist_add_input(nl);
    net_t b = netlist_add_input(nl);
    net_t sum, carry;
    tern_half_adder(nl, v, a, b, &sum, &carry);
    netlist_add_output(nl, sum);
    netlist_add_output(nl, carry);
}

void tern_build_full_adder(netlist_t *nl, tadd_version_t v) {
    net_t a = netlist_add_input(nl);
    net_t b = netlist_add_input(nl);
    net_t cin = netlist_add_input(nl);
    net_t sum, cout;
    tern_full_adder(nl, v, a, b, cin, &sum, &cout);
    netlist_add_output(nl, sum);
    netlist_add_output(nl, cout);
}

void tern_build_ripple_adder(netlist_t *nl, tadd_version_t v, int n) {
    net_t *a = malloc(sizeof(net_t) * (size_t)n);
    net_t *b = malloc(sizeof(net_t) * (size_t)n);
    net_t *s = malloc(sizeof(net_t) * (size_t)n);
    for (int i = 0; i < n; i++) a[i] = netlist_add_input(nl);
    for (int i = 0; i < n; i++) b[i] = netlist_add_input(nl);
    net_t carry = netlist_add_input(nl);

    for (int i = 0; i < n; i++) {
        tern_full_adder(nl, v, a[i], b[i], carry, &s[i], &carry);
    }
    for (int i = 0; i < n; i++) netlist_add_output(nl, s[i]);
    netlist_add_output(nl, carry);

    free(a);
    free(b);
    free(s);
}
