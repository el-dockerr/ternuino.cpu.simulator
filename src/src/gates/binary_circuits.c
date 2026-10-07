#include "gates/binary_circuits.h"
#include <stdlib.h>

static net_t nand2(netlist_t *nl, net_t a, net_t b) { return netlist_gate2(nl, G_NAND, a, b); }

void bin_full_adder(netlist_t *nl, net_t a, net_t b, net_t cin, net_t *sum, net_t *cout) {
    // x = a XOR b from four NANDs.
    net_t n1 = nand2(nl, a, b);
    net_t x = nand2(nl, nand2(nl, a, n1), nand2(nl, b, n1));
    // sum = x XOR cin from four more; cout = (a AND b) OR (x AND cin).
    net_t n5 = nand2(nl, x, cin);
    *sum = nand2(nl, nand2(nl, x, n5), nand2(nl, cin, n5));
    *cout = nand2(nl, n1, n5);
}

void bin_build_full_adder(netlist_t *nl) {
    net_t a = netlist_add_input(nl);
    net_t b = netlist_add_input(nl);
    net_t cin = netlist_add_input(nl);
    net_t sum, cout;
    bin_full_adder(nl, a, b, cin, &sum, &cout);
    netlist_add_output(nl, sum);
    netlist_add_output(nl, cout);
}

void bin_build_ripple_adder(netlist_t *nl, int n) {
    net_t *a = malloc(sizeof(net_t) * (size_t)n);
    net_t *b = malloc(sizeof(net_t) * (size_t)n);
    net_t *s = malloc(sizeof(net_t) * (size_t)n);
    for (int i = 0; i < n; i++) a[i] = netlist_add_input(nl);
    for (int i = 0; i < n; i++) b[i] = netlist_add_input(nl);
    net_t carry = netlist_add_input(nl);

    for (int i = 0; i < n; i++) {
        bin_full_adder(nl, a[i], b[i], carry, &s[i], &carry);
    }
    for (int i = 0; i < n; i++) netlist_add_output(nl, s[i]);
    netlist_add_output(nl, carry);

    free(a);
    free(b);
    free(s);
}
