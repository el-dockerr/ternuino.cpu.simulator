#include "gates/cost.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// PLACEHOLDER cost values. NOT measured data.
//
// Only the transistor counts follow a published structure: T-CMOS builds the
// STI from a single CMOS pair whose band-to-band tunnelling current sets the
// VDD/2 level, PTI/NTI are skewed-threshold inverters, and MIN/MAX are taken
// as TNAND/TNOR followed by an STI. Everything else (delay, energy, area,
// leakage) is a rough guess so that the report has non-zero numbers. Replace
// via src/config/gate_costs.cfg once UNIST data is available.
// ---------------------------------------------------------------------------
static const gate_cost_t DEFAULT_COSTS[G_TYPE_COUNT] = {
    //                delay  energy  area   leak  transistors
    [G_INPUT]    = {  0.0,   0.00,   0.00,  0.0,  0 },

    [G_TIE_NEG]  = {  0.0,   0.00,   0.20,  0.1,  2 },
    [G_TIE_ZERO] = {  0.0,   0.00,   0.25,  5.0,  2 },  // STI with output fed back to input
    [G_TIE_POS]  = {  0.0,   0.00,   0.20,  0.1,  2 },
    [G_STI]      = { 10.0,   0.12,   0.25,  5.0,  2 },  // mid-state tunnelling current
    [G_PTI]      = {  8.0,   0.10,   0.25,  0.5,  2 },
    [G_NTI]      = {  8.0,   0.10,   0.25,  0.5,  2 },
    [G_MIN]      = { 20.0,   0.30,   0.65,  6.0,  6 },  // TNAND + STI
    [G_MAX]      = { 20.0,   0.30,   0.65,  6.0,  6 },  // TNOR + STI

    [G_TIE_LO]   = {  0.0,   0.00,   0.20,  0.1,  2 },
    [G_TIE_HI]   = {  0.0,   0.00,   0.20,  0.1,  2 },
    [G_INV]      = {  8.0,   0.10,   0.25,  0.5,  2 },
    [G_NAND]     = { 12.0,   0.18,   0.40,  0.8,  4 },
    [G_NOR]      = { 14.0,   0.20,   0.40,  0.8,  4 },
};

static const double DEFAULT_HALF_SWING_FACTOR = 0.5;  // PLACEHOLDER

void cost_model_default(cost_model_t *cm) {
    memcpy(cm->gate, DEFAULT_COSTS, sizeof(cm->gate));
    cm->half_swing_factor = DEFAULT_HALF_SWING_FACTOR;
    cm->placeholder = true;
    snprintf(cm->source, sizeof(cm->source), "built-in defaults");
}

static int find_gate(const char *name) {
    for (int t = 0; t < G_TYPE_COUNT; t++) {
        if (t != G_INPUT && strcmp(gate_type_name((gate_type_t)t), name) == 0) return t;
    }
    return -1;
}

bool cost_model_load(cost_model_t *cm, const char *path, char *err, size_t err_size) {
    FILE *f = fopen(path, "r");
    if (!f) {
        snprintf(err, err_size, "cannot open '%s'", path);
        return false;
    }

    char line[512];
    int line_no = 0;
    bool ok = true;
    while (ok && fgets(line, sizeof(line), f)) {
        line_no++;
        char *hash = strchr(line, '#');
        if (hash) *hash = '\0';

        char key[64];
        int consumed = 0;
        if (sscanf(line, "%63s%n", key, &consumed) != 1) continue;  // blank line
        const char *rest = line + consumed;

        if (strcmp(key, "placeholder") == 0) {
            char val[16];
            if (sscanf(rest, "%15s", val) != 1) {
                ok = false;
            } else {
                cm->placeholder = (strcmp(val, "yes") == 0 || strcmp(val, "true") == 0 || strcmp(val, "1") == 0);
            }
        } else if (strcmp(key, "half_swing_factor") == 0) {
            if (sscanf(rest, "%lf", &cm->half_swing_factor) != 1) ok = false;
        } else {
            int t = find_gate(key);
            gate_cost_t c;
            int n = sscanf(rest, "%lf %lf %lf %lf %d",
                           &c.delay_ps, &c.energy_fj, &c.area_um2, &c.leakage_nw, &c.transistors);
            if (t < 0 || n < 4) {
                ok = false;
            } else {
                if (n == 4) c.transistors = cm->gate[t].transistors;
                cm->gate[t] = c;
            }
        }
        if (!ok) snprintf(err, err_size, "%s:%d: cannot parse '%s'", path, line_no, key);
    }
    fclose(f);

    if (ok) snprintf(cm->source, sizeof(cm->source), "%s", path);
    return ok;
}
