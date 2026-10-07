#ifndef GATES_GATE_REPORT_H
#define GATES_GATE_REPORT_H

#include <stdio.h>

#define GATE_REPORT_ACTIVITY_VECTORS 2000

// Runs the gate-level verification, then prints gate counts, area, depth,
// delay and energy for every circuit and the equal-range ternary vs binary
// comparison. cost_file may be NULL to use the built-in placeholder costs.
// Returns 0 on success, non-zero if verification failed or the cost file
// could not be loaded.
int gate_report_run(FILE *out, const char *cost_file);

#endif // GATES_GATE_REPORT_H
