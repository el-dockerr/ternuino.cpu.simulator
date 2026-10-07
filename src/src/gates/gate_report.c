#include "gates/gate_report.h"
#include "gates/gate_verify.h"
#include "gates/netlist.h"
#include "gates/cost.h"
#include "gates/metrics.h"
#include "gates/ternary_circuits.h"
#include "gates/binary_circuits.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum { C_HALF_ADDER, C_FULL_ADDER, C_RIPPLE } circuit_kind_t;

typedef struct {
    const char *label;
    logic_domain_t domain;
    circuit_kind_t kind;
    tadd_version_t version;  // ternary only
    int digits;
} circuit_spec_t;

static const circuit_spec_t CIRCUITS[] = {
    { "T half adder v1-sop",    DOMAIN_TERNARY, C_HALF_ADDER, TADD_V1_SOP,    1 },
    { "T half adder v2-sorted", DOMAIN_TERNARY, C_HALF_ADDER, TADD_V2_SORTED, 1 },
    { "T full adder v1-sop",    DOMAIN_TERNARY, C_FULL_ADDER, TADD_V1_SOP,    1 },
    { "T full adder v2-sorted", DOMAIN_TERNARY, C_FULL_ADDER, TADD_V2_SORTED, 1 },
    { "B full adder 9-NAND",    DOMAIN_BINARY,  C_FULL_ADDER, TADD_V1_SOP,    1 },
    { "T ripple 27t v1-sop",    DOMAIN_TERNARY, C_RIPPLE,     TADD_V1_SOP,    27 },
    { "T ripple 27t v2-sorted", DOMAIN_TERNARY, C_RIPPLE,     TADD_V2_SORTED, 27 },
    { "B ripple 43b",           DOMAIN_BINARY,  C_RIPPLE,     TADD_V1_SOP,    43 },
    { "T ripple 41t v1-sop",    DOMAIN_TERNARY, C_RIPPLE,     TADD_V1_SOP,    41 },
    { "T ripple 41t v2-sorted", DOMAIN_TERNARY, C_RIPPLE,     TADD_V2_SORTED, 41 },
    { "B ripple 64b",           DOMAIN_BINARY,  C_RIPPLE,     TADD_V1_SOP,    64 },
};
#define CIRCUIT_COUNT ((int)(sizeof(CIRCUITS) / sizeof(CIRCUITS[0])))

typedef struct {
    netlist_metrics_t m;
    int32_t carry_depth;    // cin -> cout, -1 if not applicable
    double carry_delay_ps;
    double act_energy_fj;   // average switching energy per operation
    double leak_energy_fj;  // leakage power x critical-path delay
} circuit_result_t;

static void build_circuit(netlist_t *nl, const circuit_spec_t *c) {
    netlist_init(nl, c->label, c->domain);
    if (c->domain == DOMAIN_TERNARY) {
        switch (c->kind) {
            case C_HALF_ADDER: tern_build_half_adder(nl, c->version); break;
            case C_FULL_ADDER: tern_build_full_adder(nl, c->version); break;
            case C_RIPPLE: tern_build_ripple_adder(nl, c->version, c->digits); break;
        }
    } else {
        if (c->kind == C_RIPPLE) bin_build_ripple_adder(nl, c->digits);
        else bin_build_full_adder(nl);
    }
}

static uint64_t xorshift_state;

static uint64_t xorshift_next(void) {
    uint64_t x = xorshift_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return xorshift_state = x;
}

// Average switching energy per operation over random, independent input
// vectors (fixed seed). The first vector only establishes the initial state.
static double activity_energy(const netlist_t *nl, const cost_model_t *cm) {
    int radix = (nl->domain == DOMAIN_TERNARY) ? 3 : 2;
    int8_t offset = (int8_t)(nl->domain == DOMAIN_TERNARY ? -1 : 0);
    int8_t *in = malloc((size_t)nl->input_count);
    int8_t *prev = malloc((size_t)nl->gate_count);
    int8_t *cur = malloc((size_t)nl->gate_count);
    activity_t act;
    activity_init(&act);
    xorshift_state = 0x5EED5EED12345678ull;

    for (int v = 0; v <= GATE_REPORT_ACTIVITY_VECTORS; v++) {
        for (int i = 0; i < nl->input_count; i++) {
            in[i] = (int8_t)((int)(xorshift_next() % (uint64_t)radix) + offset);
        }
        netlist_eval(nl, in, cur);
        if (v > 0) activity_accumulate(&act, nl, cm, prev, cur);
        int8_t *t = prev;
        prev = cur;
        cur = t;
    }

    free(in);
    free(prev);
    free(cur);
    return act.steps ? act.energy_fj / (double)act.steps : 0.0;
}

static void analyze(const circuit_spec_t *c, const cost_model_t *cm, circuit_result_t *r) {
    netlist_t nl;
    build_circuit(&nl, c);
    netlist_compute_metrics(&nl, cm, &r->m);

    if (c->kind == C_HALF_ADDER) {
        r->carry_depth = -1;
        r->carry_delay_ps = -1.0;
    } else {
        net_t cin = nl.inputs[nl.input_count - 1];
        net_t cout = nl.outputs[nl.output_count - 1];
        r->carry_depth = netlist_path_depth(&nl, cin, cout);
        r->carry_delay_ps = netlist_path_delay(&nl, cm, cin, cout);
    }

    r->act_energy_fj = activity_energy(&nl, cm);
    // nW * ps = 1e-21 J = 1e-6 fJ
    r->leak_energy_fj = r->m.leakage_nw * r->m.delay_ps * 1e-6;
    netlist_free(&nl);
}

static void print_rule(FILE *out, int width) {
    for (int i = 0; i < width; i++) fputc('-', out);
    fputc('\n', out);
}

static void print_gate_counts(FILE *out, const circuit_result_t *res) {
    static const gate_type_t cols[] = { G_STI, G_PTI, G_NTI, G_MIN, G_MAX, G_INV, G_NAND, G_NOR };
    const int ncols = (int)(sizeof(cols) / sizeof(cols[0]));

    fprintf(out, "Gate counts\n");
    fprintf(out, "%-24s", "circuit");
    for (int k = 0; k < ncols; k++) fprintf(out, " %5s", gate_type_name(cols[k]));
    fprintf(out, " %5s %7s %7s\n", "ties", "gates", "xtors");
    print_rule(out, 24 + 6 * ncols + 22);
    for (int i = 0; i < CIRCUIT_COUNT; i++) {
        const netlist_metrics_t *m = &res[i].m;
        fprintf(out, "%-24s", CIRCUITS[i].label);
        for (int k = 0; k < ncols; k++) fprintf(out, " %5d", m->count[cols[k]]);
        fprintf(out, " %5d %7d %7d\n", m->tie_cells, m->logic_gates, m->transistors);
    }
    fprintf(out, "  gates = logic cells without ties; xtors = transistor count from the cost model\n\n");
}

static void print_costs(FILE *out, const circuit_result_t *res) {
    fprintf(out, "Area, timing and energy\n");
    fprintf(out, "%-24s %9s %6s %9s %6s %9s %9s %9s %9s %9s\n",
            "circuit", "area_um2", "depth", "delay_ps", "c-dep", "c-del_ps",
            "E_all_fJ", "E_act_fJ", "leak_nW", "E_leak_fJ");
    print_rule(out, 24 + 10 + 7 + 10 + 7 + 10 * 5);
    for (int i = 0; i < CIRCUIT_COUNT; i++) {
        const circuit_result_t *r = &res[i];
        fprintf(out, "%-24s %9.2f %6d %9.1f ", CIRCUITS[i].label,
                r->m.area_um2, r->m.depth, r->m.delay_ps);
        if (r->carry_depth >= 0) fprintf(out, "%6d %9.1f ", r->carry_depth, r->carry_delay_ps);
        else fprintf(out, "%6s %9s ", "-", "-");
        fprintf(out, "%9.2f %9.3f %9.1f %9.4f\n",
                r->m.energy_all_fj, r->act_energy_fj, r->m.leakage_nw, r->leak_energy_fj);
    }
    fprintf(out,
        "  depth/delay   = critical path over all outputs (static timing, no wire load)\n"
        "  c-dep/c-del   = carry-in -> carry-out path\n"
        "  E_all         = sum of per-gate energy (every gate toggles once, full swing)\n"
        "  E_act         = average switching energy per operation, %d random vectors\n"
        "  E_leak        = total leakage power x critical-path delay (one op per period)\n\n",
        GATE_REPORT_ACTIVITY_VECTORS);
}

static int find_circuit(const char *label) {
    for (int i = 0; i < CIRCUIT_COUNT; i++) {
        if (strcmp(CIRCUITS[i].label, label) == 0) return i;
    }
    return -1;
}

static double ipow(double base, int exp) {
    double r = 1.0;
    for (int i = 0; i < exp; i++) r *= base;
    return r;
}

static void print_compare_row(FILE *out, const char *metric, double t, double b, int nt, int nb) {
    double ratio = (b != 0.0) ? t / b : 0.0;
    double per_digit = (b != 0.0) ? (t / nt) / (b / nb) : 0.0;
    fprintf(out, "  %-22s %12.2f %12.2f %8.3f %10.3f   %s\n",
            metric, t, b, ratio, per_digit, ratio < 1.0 ? "ternary" : "binary");
}

static void print_comparison(FILE *out, const circuit_result_t *res,
                             const char *tern_label, const char *bin_label) {
    int ti = find_circuit(tern_label);
    int bi = find_circuit(bin_label);
    const circuit_result_t *t = &res[ti];
    const circuit_result_t *b = &res[bi];
    int nt = CIRCUITS[ti].digits, nb = CIRCUITS[bi].digits;

    // Unsigned range of n balanced trits is 3^n values, of n bits 2^n values.
    fprintf(out, "%d trits (3^%d = %.3g values)  vs  %d bits (2^%d = %.3g values)\n",
            nt, nt, ipow(3.0, nt), nb, nb, ipow(2.0, nb));
    fprintf(out, "  %-22s %12s %12s %8s %10s   %s\n",
            "metric", "ternary", "binary", "T/B", "T/B/digit", "better");
    print_compare_row(out, "logic gates", t->m.logic_gates, b->m.logic_gates, nt, nb);
    print_compare_row(out, "transistors", t->m.transistors, b->m.transistors, nt, nb);
    print_compare_row(out, "area [um2]", t->m.area_um2, b->m.area_um2, nt, nb);
    print_compare_row(out, "critical depth", t->m.depth, b->m.depth, nt, nb);
    print_compare_row(out, "critical delay [ps]", t->m.delay_ps, b->m.delay_ps, nt, nb);
    print_compare_row(out, "E_act/op [fJ]", t->act_energy_fj, b->act_energy_fj, nt, nb);
    print_compare_row(out, "E_leak/op [fJ]", t->leak_energy_fj, b->leak_energy_fj, nt, nb);
    print_compare_row(out, "E_total/op [fJ]", t->act_energy_fj + t->leak_energy_fj,
                      b->act_energy_fj + b->leak_energy_fj, nt, nb);
    fprintf(out, "  break-even: ternary wins a metric when T/B < 1, i.e. when one trit stage costs\n"
                 "  less than %.3f bit stages (T/B/digit < %.3f; log2(3) = 1.585)\n\n",
            (double)nb / nt, (double)nb / nt);
}

static void print_placeholder_banner(FILE *out, const cost_model_t *cm) {
    if (cm->placeholder) {
        fprintf(out,
            "!!! COST VALUES ARE PLACEHOLDERS - NOT MEASURED T-CMOS / CMOS DATA.            !!!\n"
            "!!! Gate counts, depths and verification are exact; area/delay/energy are not. !!!\n");
    }
}

int gate_report_run(FILE *out, const char *cost_file) {
    cost_model_t cm;
    cost_model_default(&cm);
    if (cost_file) {
        char err[512];
        if (!cost_model_load(&cm, cost_file, err, sizeof(err))) {
            fprintf(out, "Error loading gate costs: %s\n", err);
            return 2;
        }
    }

    fprintf(out, "==================================================================================\n");
    fprintf(out, " Ternuino gate-level report: T-CMOS ternary model vs binary CMOS baseline\n");
    fprintf(out, "==================================================================================\n");
    print_placeholder_banner(out, &cm);
    fprintf(out, "Cost source: %s (half-swing energy factor %.2f)\n", cm.source, cm.half_swing_factor);
    fprintf(out, "Encoding: -1 = 0 V, 0 = VDD/2, +1 = VDD.  Primitives: STI PTI NTI MIN MAX + ties\n\n");

    int failed = gate_verify_all(out);

    circuit_result_t res[CIRCUIT_COUNT];
    for (int i = 0; i < CIRCUIT_COUNT; i++) analyze(&CIRCUITS[i], &cm, &res[i]);

    print_gate_counts(out, res);
    print_costs(out, res);

    fprintf(out, "Equal-range comparison (best ternary version vs binary)\n");
    print_placeholder_banner(out, &cm);
    print_comparison(out, res, "T ripple 27t v2-sorted", "B ripple 43b");
    print_comparison(out, res, "T ripple 41t v2-sorted", "B ripple 64b");

    if (failed) {
        fprintf(out, "VERIFICATION FAILED: %d check(s) failed, see above.\n", failed);
        return 1;
    }
    return 0;
}
