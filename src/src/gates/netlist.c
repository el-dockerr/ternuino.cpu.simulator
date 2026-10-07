#include "gates/netlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Truth tables. Ternary tables are indexed by trit + 1, binary by bit value.
// Gate behaviour is defined only by these tables; no arithmetic is used.
static const int8_t LUT_STI[3] = { +1,  0, -1 };
static const int8_t LUT_PTI[3] = { +1, +1, -1 };
static const int8_t LUT_NTI[3] = { +1, -1, -1 };

static const int8_t LUT_MIN[3][3] = {
    { -1, -1, -1 },
    { -1,  0,  0 },
    { -1,  0, +1 },
};
static const int8_t LUT_MAX[3][3] = {
    { -1,  0, +1 },
    {  0,  0, +1 },
    { +1, +1, +1 },
};

static const int8_t LUT_INV[2] = { 1, 0 };
static const int8_t LUT_NAND[2][2] = { { 1, 1 }, { 1, 0 } };
static const int8_t LUT_NOR[2][2]  = { { 1, 0 }, { 0, 0 } };

static const char *GATE_NAMES[G_TYPE_COUNT] = {
    "INPUT",
    "TIE-", "TIE0", "TIE+", "STI", "PTI", "NTI", "MIN", "MAX",
    "TIE_LO", "TIE_HI", "INV", "NAND", "NOR",
};

static void fail(const char *msg, const netlist_t *nl) {
    fprintf(stderr, "gate layer error (%s): %s\n", nl ? nl->name : "-", msg);
    abort();
}

static void *grow(void *ptr, int32_t *cap, size_t elem) {
    int32_t new_cap = *cap ? *cap * 2 : 64;
    void *p = realloc(ptr, (size_t)new_cap * elem);
    if (!p) {
        fprintf(stderr, "gate layer error: out of memory\n");
        abort();
    }
    *cap = new_cap;
    return p;
}

const char *gate_type_name(gate_type_t type) {
    return (type >= 0 && type < G_TYPE_COUNT) ? GATE_NAMES[type] : "?";
}

bool gate_is_tie(gate_type_t type) {
    return type == G_TIE_NEG || type == G_TIE_ZERO || type == G_TIE_POS ||
           type == G_TIE_LO || type == G_TIE_HI;
}

logic_domain_t gate_domain(gate_type_t type) {
    return (type >= G_TIE_LO) ? DOMAIN_BINARY : DOMAIN_TERNARY;
}

int gate_arity(gate_type_t type) {
    switch (type) {
        case G_STI: case G_PTI: case G_NTI: case G_INV:
            return 1;
        case G_MIN: case G_MAX: case G_NAND: case G_NOR:
            return 2;
        default:
            return 0;
    }
}

static bool value_in_domain(int8_t v, logic_domain_t domain) {
    if (domain == DOMAIN_TERNARY) return v == -1 || v == 0 || v == 1;
    return v == 0 || v == 1;
}

int8_t gate_eval1(gate_type_t type, int8_t a) {
    switch (type) {
        case G_STI: return LUT_STI[a + 1];
        case G_PTI: return LUT_PTI[a + 1];
        case G_NTI: return LUT_NTI[a + 1];
        case G_INV: return LUT_INV[a];
        default:
            fail("gate_eval1 on non-unary gate", NULL);
            return 0;
    }
}

int8_t gate_eval2(gate_type_t type, int8_t a, int8_t b) {
    switch (type) {
        case G_MIN:  return LUT_MIN[a + 1][b + 1];
        case G_MAX:  return LUT_MAX[a + 1][b + 1];
        case G_NAND: return LUT_NAND[a][b];
        case G_NOR:  return LUT_NOR[a][b];
        default:
            fail("gate_eval2 on non-binary-input gate", NULL);
            return 0;
    }
}

static int8_t tie_value(gate_type_t type) {
    switch (type) {
        case G_TIE_NEG: return -1;
        case G_TIE_ZERO: return 0;
        case G_TIE_POS: return 1;
        case G_TIE_LO: return 0;
        case G_TIE_HI: return 1;
        default: return 0;
    }
}

void netlist_init(netlist_t *nl, const char *name, logic_domain_t domain) {
    memset(nl, 0, sizeof(*nl));
    snprintf(nl->name, sizeof(nl->name), "%s", name);
    nl->domain = domain;
    nl->tie[0] = nl->tie[1] = nl->tie[2] = NET_NONE;
}

void netlist_free(netlist_t *nl) {
    free(nl->gates);
    free(nl->inputs);
    free(nl->outputs);
    memset(nl, 0, sizeof(*nl));
}

static net_t append_gate(netlist_t *nl, gate_type_t type, net_t a, net_t b) {
    if (nl->gate_count == nl->gate_cap) {
        nl->gates = grow(nl->gates, &nl->gate_cap, sizeof(gate_t));
    }
    gate_t *g = &nl->gates[nl->gate_count];
    g->type = type;
    g->in[0] = a;
    g->in[1] = b;
    return nl->gate_count++;
}

net_t netlist_add_input(netlist_t *nl) {
    net_t net = append_gate(nl, G_INPUT, NET_NONE, NET_NONE);
    if (nl->input_count == nl->input_cap) {
        nl->inputs = grow(nl->inputs, &nl->input_cap, sizeof(net_t));
    }
    nl->inputs[nl->input_count++] = net;
    return net;
}

void netlist_add_output(netlist_t *nl, net_t net) {
    if (net < 0 || net >= nl->gate_count) fail("output references unknown net", nl);
    if (nl->output_count == nl->output_cap) {
        nl->outputs = grow(nl->outputs, &nl->output_cap, sizeof(net_t));
    }
    nl->outputs[nl->output_count++] = net;
}

static void check_input_net(const netlist_t *nl, net_t net) {
    if (net < 0 || net >= nl->gate_count) fail("gate input references unknown net", nl);
}

static void check_gate(const netlist_t *nl, gate_type_t type, int arity) {
    if (gate_arity(type) != arity) fail("wrong number of gate inputs", nl);
    if (gate_domain(type) != nl->domain) fail("gate from wrong logic domain", nl);
}

net_t netlist_gate1(netlist_t *nl, gate_type_t type, net_t a) {
    check_gate(nl, type, 1);
    check_input_net(nl, a);
    return append_gate(nl, type, a, NET_NONE);
}

net_t netlist_gate2(netlist_t *nl, gate_type_t type, net_t a, net_t b) {
    check_gate(nl, type, 2);
    check_input_net(nl, a);
    check_input_net(nl, b);
    return append_gate(nl, type, a, b);
}

net_t netlist_const(netlist_t *nl, int value) {
    gate_type_t type;
    int slot;
    if (nl->domain == DOMAIN_TERNARY) {
        if (value < -1 || value > 1) fail("ternary constant out of range", nl);
        static const gate_type_t types[3] = { G_TIE_NEG, G_TIE_ZERO, G_TIE_POS };
        slot = value + 1;
        type = types[slot];
    } else {
        if (value != 0 && value != 1) fail("binary constant out of range", nl);
        slot = value;
        type = value ? G_TIE_HI : G_TIE_LO;
    }
    if (nl->tie[slot] == NET_NONE) {
        nl->tie[slot] = append_gate(nl, type, NET_NONE, NET_NONE);
    }
    return nl->tie[slot];
}

void netlist_eval(const netlist_t *nl, const int8_t *in_values, int8_t *net_values) {
    int32_t next_input = 0;
    for (int32_t i = 0; i < nl->gate_count; i++) {
        const gate_t *g = &nl->gates[i];
        int8_t v;
        switch (gate_arity(g->type)) {
            case 1: {
                int8_t a = net_values[g->in[0]];
                if (!value_in_domain(a, nl->domain)) fail("gate input outside logic domain", nl);
                v = gate_eval1(g->type, a);
                break;
            }
            case 2: {
                int8_t a = net_values[g->in[0]];
                int8_t b = net_values[g->in[1]];
                if (!value_in_domain(a, nl->domain) || !value_in_domain(b, nl->domain)) {
                    fail("gate input outside logic domain", nl);
                }
                v = gate_eval2(g->type, a, b);
                break;
            }
            default:
                if (g->type == G_INPUT) {
                    v = in_values[next_input++];
                    if (!value_in_domain(v, nl->domain)) fail("primary input outside logic domain", nl);
                } else {
                    v = tie_value(g->type);
                }
                break;
        }
        net_values[i] = v;
    }
}
