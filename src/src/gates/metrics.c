#include "gates/metrics.h"
#include <stdlib.h>
#include <string.h>

static double max_d(double a, double b) { return a > b ? a : b; }

// Longest-path arrival times in a single forward pass (insertion order is
// topological). Inputs and tie cells arrive at 0. With `from` set, only nets
// reachable from it get a valid (>= 0) arrival; all others stay at -1.
static void arrival_times(const netlist_t *nl, const double *gate_delay, net_t from, double *arr) {
    for (int32_t i = 0; i < nl->gate_count; i++) {
        const gate_t *g = &nl->gates[i];
        int arity = gate_arity(g->type);
        if (arity == 0) {
            arr[i] = (from == NET_NONE || from == i) ? 0.0 : -1.0;
            continue;
        }
        double in_arr = -1.0;
        for (int k = 0; k < arity; k++) in_arr = max_d(in_arr, arr[g->in[k]]);
        arr[i] = (in_arr < 0.0) ? -1.0 : in_arr + gate_delay[g->type];
    }
}

static double path_metric(const netlist_t *nl, const double *gate_delay, net_t from, net_t to) {
    double *arr = malloc(sizeof(double) * (size_t)(nl->gate_count ? nl->gate_count : 1));
    arrival_times(nl, gate_delay, from, arr);
    double result = -1.0;
    if (to == NET_NONE) {
        for (int32_t i = 0; i < nl->output_count; i++) result = max_d(result, arr[nl->outputs[i]]);
    } else {
        result = arr[to];
    }
    free(arr);
    return result;
}

static void unit_delays(double *d) {
    for (int t = 0; t < G_TYPE_COUNT; t++) d[t] = 1.0;
}

static void cost_delays(const cost_model_t *cm, double *d) {
    for (int t = 0; t < G_TYPE_COUNT; t++) d[t] = cm->gate[t].delay_ps;
}

void netlist_compute_metrics(const netlist_t *nl, const cost_model_t *cm, netlist_metrics_t *m) {
    memset(m, 0, sizeof(*m));
    for (int32_t i = 0; i < nl->gate_count; i++) {
        gate_type_t t = nl->gates[i].type;
        if (t == G_INPUT) continue;
        const gate_cost_t *c = &cm->gate[t];
        m->count[t]++;
        if (gate_is_tie(t)) m->tie_cells++;
        else m->logic_gates++;
        m->transistors += c->transistors;
        m->area_um2 += c->area_um2;
        m->energy_all_fj += c->energy_fj;
        m->leakage_nw += c->leakage_nw;
    }

    double d[G_TYPE_COUNT];
    unit_delays(d);
    m->depth = (int32_t)path_metric(nl, d, NET_NONE, NET_NONE);
    cost_delays(cm, d);
    m->delay_ps = path_metric(nl, d, NET_NONE, NET_NONE);
}

int32_t netlist_path_depth(const netlist_t *nl, net_t from, net_t to) {
    double d[G_TYPE_COUNT];
    unit_delays(d);
    return (int32_t)path_metric(nl, d, from, to);
}

double netlist_path_delay(const netlist_t *nl, const cost_model_t *cm, net_t from, net_t to) {
    double d[G_TYPE_COUNT];
    cost_delays(cm, d);
    return path_metric(nl, d, from, to);
}

void activity_init(activity_t *a) {
    memset(a, 0, sizeof(*a));
}

void activity_accumulate(activity_t *a, const netlist_t *nl, const cost_model_t *cm,
                         const int8_t *prev_values, const int8_t *cur_values) {
    for (int32_t i = 0; i < nl->gate_count; i++) {
        gate_type_t t = nl->gates[i].type;
        if (t == G_INPUT || gate_is_tie(t)) continue;
        int8_t p = prev_values[i];
        int8_t c = cur_values[i];
        if (p == c) continue;

        // Ternary: a step between adjacent levels swings VDD/2, -1<->+1 swings VDD.
        int full_swing = (nl->domain == DOMAIN_BINARY) || (p == -c);
        double w = full_swing ? 1.0 : cm->half_swing_factor;
        a->weighted_transitions += w;
        a->energy_fj += w * cm->gate[t].energy_fj;
    }
    a->steps++;
}
