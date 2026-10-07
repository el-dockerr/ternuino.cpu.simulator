#ifndef GATES_NETLIST_H
#define GATES_NETLIST_H

#include <stdint.h>
#include <stdbool.h>

// Gate-level netlist for the T-CMOS model (and its binary CMOS baseline).
//
// Every gate drives exactly one net, and that net's id is the gate's index.
// Gates may only reference nets that already exist, so the insertion order is
// a valid topological order and evaluation is a single forward pass.
//
// Ternary nets carry balanced trits {-1, 0, +1}, mapped to T-CMOS voltage
// levels 0 V, VDD/2, VDD. Binary nets carry {0, 1}. Ternary and binary gates
// cannot be mixed in one netlist.

typedef int32_t net_t;

#define NET_NONE ((net_t)-1)

typedef enum {
    // Primary input (no cell).
    G_INPUT,

    // Ternary primitives.
    G_TIE_NEG,   // constant -1 (0 V)
    G_TIE_ZERO,  // constant  0 (VDD/2)
    G_TIE_POS,   // constant +1 (VDD)
    G_STI,       // standard ternary inverter:  -1->+1, 0-> 0, +1->-1
    G_PTI,       // positive ternary inverter:  -1->+1, 0->+1, +1->-1
    G_NTI,       // negative ternary inverter:  -1->+1, 0->-1, +1->-1
    G_MIN,       // MIN(a, b), same as TAND
    G_MAX,       // MAX(a, b), same as TOR

    // Binary baseline primitives.
    G_TIE_LO,    // constant 0
    G_TIE_HI,    // constant 1
    G_INV,
    G_NAND,
    G_NOR,

    G_TYPE_COUNT
} gate_type_t;

typedef enum {
    DOMAIN_TERNARY,
    DOMAIN_BINARY
} logic_domain_t;

typedef struct {
    gate_type_t type;
    net_t in[2];   // NET_NONE for unused inputs
} gate_t;

typedef struct {
    char name[64];
    logic_domain_t domain;

    gate_t *gates;
    int32_t gate_count;
    int32_t gate_cap;

    net_t *inputs;
    int32_t input_count;
    int32_t input_cap;

    net_t *outputs;
    int32_t output_count;
    int32_t output_cap;

    // One shared tie cell per constant, created on first use.
    net_t tie[3];
} netlist_t;

void netlist_init(netlist_t *nl, const char *name, logic_domain_t domain);
void netlist_free(netlist_t *nl);

net_t netlist_add_input(netlist_t *nl);
void netlist_add_output(netlist_t *nl, net_t net);

// Adds a gate and returns the net it drives. Aborts on invalid input
// (unknown net, wrong arity, gate from the other logic domain).
net_t netlist_gate1(netlist_t *nl, gate_type_t type, net_t a);
net_t netlist_gate2(netlist_t *nl, gate_type_t type, net_t a, net_t b);

// Constant net. Ternary: value in {-1, 0, +1}. Binary: value in {0, 1}.
net_t netlist_const(netlist_t *nl, int value);

// Evaluates the whole netlist. in_values has input_count entries in input
// order; net_values must hold gate_count entries and receives every net value.
// Aborts if any gate sees a value outside its logic domain.
void netlist_eval(const netlist_t *nl, const int8_t *in_values, int8_t *net_values);

// Single-gate evaluation by truth table (exposed for primitive unit tests).
int8_t gate_eval1(gate_type_t type, int8_t a);
int8_t gate_eval2(gate_type_t type, int8_t a, int8_t b);

const char *gate_type_name(gate_type_t type);
bool gate_is_tie(gate_type_t type);
logic_domain_t gate_domain(gate_type_t type);
int gate_arity(gate_type_t type);

#endif // GATES_NETLIST_H
