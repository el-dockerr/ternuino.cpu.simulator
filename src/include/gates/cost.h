#ifndef GATES_COST_H
#define GATES_COST_H

#include <stdbool.h>
#include <stddef.h>
#include "gates/netlist.h"

// Per-gate cost parameters.
//
// The compiled-in defaults (cost.c) are PLACEHOLDERS. They only encode rough
// relative sizes (transistor counts, "a MIN/MAX is a TNAND/TNOR plus an STI").
// Real values are meant to come from UNIST T-CMOS data and are loaded at run
// time from a text file, see cost_model_load() and src/config/gate_costs.cfg.

typedef struct {
    double delay_ps;     // propagation delay
    double energy_fj;    // dynamic energy per full-swing output transition
    double area_um2;     // cell area
    double leakage_nw;   // average static power
    int transistors;     // optional, informational
} gate_cost_t;

typedef struct {
    gate_cost_t gate[G_TYPE_COUNT];

    // Energy of a ternary half-swing transition (-1<->0 or 0<->+1, i.e. a
    // VDD/2 step) relative to a full-swing transition (-1<->+1).
    double half_swing_factor;

    // True while the values are placeholders; printed prominently in reports.
    bool placeholder;

    char source[256];
} cost_model_t;

void cost_model_default(cost_model_t *cm);

// Overrides values from a text file. Format (one entry per line, '#' comments):
//   placeholder        yes|no
//   half_swing_factor  <value>
//   <GATE> <delay_ps> <energy_fj> <area_um2> <leakage_nw> [transistors]
// GATE is a name as printed by gate_type_name() (STI, PTI, MIN, NAND, TIE0 ...).
// Gates not listed keep their current values. Returns false and fills err on
// parse errors.
bool cost_model_load(cost_model_t *cm, const char *path, char *err, size_t err_size);

#endif // GATES_COST_H
