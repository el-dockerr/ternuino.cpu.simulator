#ifndef GATES_METRICS_H
#define GATES_METRICS_H

#include <stdint.h>
#include "gates/netlist.h"
#include "gates/cost.h"

typedef struct {
    int32_t count[G_TYPE_COUNT];
    int32_t logic_gates;   // all cells except primary inputs and tie cells
    int32_t tie_cells;
    int32_t transistors;
    double area_um2;
    int32_t depth;         // logic levels on the deepest input->output path
    double delay_ps;       // critical-path delay (static timing, no wire load)
    double energy_all_fj;  // sum of per-gate energy: every gate toggles once, full swing
    double leakage_nw;     // sum of per-gate static power
} netlist_metrics_t;

void netlist_compute_metrics(const netlist_t *nl, const cost_model_t *cm, netlist_metrics_t *m);

// Logic depth / delay of the longest path from net `from` to net `to`
// (e.g. carry-in to carry-out). Returns -1 if `to` does not depend on `from`.
int32_t netlist_path_depth(const netlist_t *nl, net_t from, net_t to);
double netlist_path_delay(const netlist_t *nl, const cost_model_t *cm, net_t from, net_t to);

// Switching-activity energy, accumulated over consecutive evaluations of the
// same netlist. Only settled values are compared (zero-delay simulation, no
// glitches). Primary inputs and tie cells are not counted.
typedef struct {
    double weighted_transitions;  // full swing = 1.0, ternary half swing = half_swing_factor
    double energy_fj;
    int64_t steps;                // number of (prev, cur) pairs accumulated
} activity_t;

void activity_init(activity_t *a);
void activity_accumulate(activity_t *a, const netlist_t *nl, const cost_model_t *cm,
                         const int8_t *prev_values, const int8_t *cur_values);

#endif // GATES_METRICS_H
