#include "gates/gate_verify.h"
#include "gates/netlist.h"
#include "gates/ternary_circuits.h"
#include "gates/binary_circuits.h"
#include "tritlogic.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Reference arithmetic. A 41-trit balanced operand reaches +-1.8e19, beyond
// int64, so the reference uses a small two's-complement 128-bit integer.
// ---------------------------------------------------------------------------

typedef struct {
    uint64_t lo;
    uint64_t hi;
} wide_t;

static wide_t w_from_int(int64_t v) {
    wide_t w;
    w.lo = (uint64_t)v;
    w.hi = (v < 0) ? ~(uint64_t)0 : 0;
    return w;
}

static wide_t w_add(wide_t a, wide_t b) {
    wide_t r;
    r.lo = a.lo + b.lo;
    r.hi = a.hi + b.hi + (r.lo < a.lo ? 1 : 0);
    return r;
}

static wide_t w_neg(wide_t a) {
    wide_t r = { ~a.lo, ~a.hi };
    return w_add(r, w_from_int(1));
}

static wide_t w_mul_small(wide_t a, int k) {
    wide_t r = w_from_int(0);
    for (int i = 0; i < (k < 0 ? -k : k); i++) r = w_add(r, a);
    return (k < 0) ? w_neg(r) : r;
}

static bool w_eq(wide_t a, wide_t b) {
    return a.lo == b.lo && a.hi == b.hi;
}

// Value of digits d[0..n-1] (index 0 least significant) in the given radix.
static wide_t w_value(const int8_t *d, int n, int radix) {
    wide_t v = w_from_int(0);
    for (int i = n - 1; i >= 0; i--) v = w_add(w_mul_small(v, radix), w_from_int(d[i]));
    return v;
}

// Fixed-seed PRNG (splitmix64) so results are identical on every platform.
static uint64_t rng_state;

static uint64_t rng_next(void) {
    uint64_t z = (rng_state += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

// ---------------------------------------------------------------------------
// Reporting
// ---------------------------------------------------------------------------

typedef struct {
    FILE *out;
    int failed;
} ctx_t;

static void report(ctx_t *ctx, bool ok, const char *what, long cases, const char *detail) {
    fprintf(ctx->out, "  [%s] %-52s %8ld cases%s%s\n",
            ok ? "PASS" : "FAIL", what, cases, detail[0] ? "  " : "", detail);
    if (!ok) ctx->failed++;
}

// ---------------------------------------------------------------------------
// Primitive truth tables
// ---------------------------------------------------------------------------

static const int8_t T_VALUES[3] = { -1, 0, 1 };

static const struct {
    gate_type_t type;
    int8_t expect[3];
} UNARY_TERNARY[] = {
    { G_STI, { +1,  0, -1 } },
    { G_PTI, { +1, +1, -1 } },
    { G_NTI, { +1, -1, -1 } },
};

static int8_t ref_min(int8_t a, int8_t b) { return a < b ? a : b; }
static int8_t ref_max(int8_t a, int8_t b) { return a > b ? a : b; }

// Evaluates a one-gate netlist, so the netlist path is tested as well.
static int8_t eval_single(logic_domain_t dom, gate_type_t type, int8_t a, int8_t b) {
    netlist_t nl;
    netlist_init(&nl, gate_type_name(type), dom);
    net_t ia = netlist_add_input(&nl);
    net_t ib = netlist_add_input(&nl);
    net_t o = (gate_arity(type) == 1) ? netlist_gate1(&nl, type, ia) : netlist_gate2(&nl, type, ia, ib);
    netlist_add_output(&nl, o);
    int8_t in[2] = { a, b };
    int8_t *vals = malloc((size_t)nl.gate_count);
    netlist_eval(&nl, in, vals);
    int8_t r = vals[o];
    free(vals);
    netlist_free(&nl);
    return r;
}

static void verify_primitives(ctx_t *ctx) {
    char name[64];
    for (size_t g = 0; g < sizeof(UNARY_TERNARY) / sizeof(UNARY_TERNARY[0]); g++) {
        gate_type_t t = UNARY_TERNARY[g].type;
        bool ok = true;
        for (int i = 0; i < 3; i++) {
            int8_t x = T_VALUES[i];
            ok &= gate_eval1(t, x) == UNARY_TERNARY[g].expect[i];
            ok &= eval_single(DOMAIN_TERNARY, t, x, 0) == UNARY_TERNARY[g].expect[i];
        }
        snprintf(name, sizeof(name), "primitive %s truth table", gate_type_name(t));
        report(ctx, ok, name, 3, "");
    }

    gate_type_t binops[2] = { G_MIN, G_MAX };
    for (int g = 0; g < 2; g++) {
        bool ok = true;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int8_t a = T_VALUES[i], b = T_VALUES[j];
                int8_t expect = (binops[g] == G_MIN) ? ref_min(a, b) : ref_max(a, b);
                int32_t isa = (binops[g] == G_MIN) ? trit_and(a, b) : trit_or(a, b);
                ok &= gate_eval2(binops[g], a, b) == expect;
                ok &= eval_single(DOMAIN_TERNARY, binops[g], a, b) == expect;
                ok &= isa == expect;  // must agree with the ISA's TAND/TOR
            }
        }
        snprintf(name, sizeof(name), "primitive %s truth table (= %s)",
                 gate_type_name(binops[g]), binops[g] == G_MIN ? "TAND" : "TOR");
        report(ctx, ok, name, 9, "");
    }

    {
        bool ok = true;
        for (int8_t x = 0; x <= 1; x++) {
            ok &= gate_eval1(G_INV, x) == !x;
            ok &= eval_single(DOMAIN_BINARY, G_INV, x, 0) == !x;
        }
        report(ctx, ok, "primitive INV truth table", 2, "");
    }
    gate_type_t bin2[2] = { G_NAND, G_NOR };
    for (int g = 0; g < 2; g++) {
        bool ok = true;
        for (int8_t a = 0; a <= 1; a++) {
            for (int8_t b = 0; b <= 1; b++) {
                int8_t expect = (bin2[g] == G_NAND) ? !(a && b) : !(a || b);
                ok &= gate_eval2(bin2[g], a, b) == expect;
                ok &= eval_single(DOMAIN_BINARY, bin2[g], a, b) == expect;
            }
        }
        snprintf(name, sizeof(name), "primitive %s truth table", gate_type_name(bin2[g]));
        report(ctx, ok, name, 4, "");
    }

    // Tie cells.
    {
        bool ok = true;
        netlist_t nl;
        netlist_init(&nl, "ties", DOMAIN_TERNARY);
        net_t n = netlist_const(&nl, -1), z = netlist_const(&nl, 0), p = netlist_const(&nl, 1);
        ok &= netlist_const(&nl, 0) == z;  // shared, not duplicated
        int8_t vals[3];
        netlist_eval(&nl, NULL, vals);
        ok &= vals[n] == -1 && vals[z] == 0 && vals[p] == 1;
        netlist_free(&nl);

        netlist_init(&nl, "ties", DOMAIN_BINARY);
        net_t lo = netlist_const(&nl, 0), hi = netlist_const(&nl, 1);
        netlist_eval(&nl, NULL, vals);
        ok &= vals[lo] == 0 && vals[hi] == 1;
        netlist_free(&nl);
        report(ctx, ok, "tie cells TIE- TIE0 TIE+ TIE_LO TIE_HI", 5, "");
    }
}

// ---------------------------------------------------------------------------
// Adders
//
// All adder netlists share one port layout: inputs a[0..n-1], b[0..n-1] and
// (optionally) cin; outputs s[0..n-1] and the carry. The check is
//     A + B + cin == S + radix^n * carry
// ---------------------------------------------------------------------------

typedef struct {
    const netlist_t *nl;
    int n;
    int radix;
    bool has_cin;
    wide_t radix_pow_n;
    int8_t *nets;
    int8_t *in;
    int8_t *outs;
} adder_check_t;

static void adder_check_init(adder_check_t *c, const netlist_t *nl, int n, int radix, bool has_cin) {
    c->nl = nl;
    c->n = n;
    c->radix = radix;
    c->has_cin = has_cin;
    c->radix_pow_n = w_from_int(1);
    for (int i = 0; i < n; i++) c->radix_pow_n = w_mul_small(c->radix_pow_n, radix);
    c->nets = malloc((size_t)nl->gate_count);
    c->in = calloc((size_t)nl->input_count, 1);
    c->outs = malloc((size_t)nl->output_count);
}

static void adder_check_free(adder_check_t *c) {
    free(c->nets);
    free(c->in);
    free(c->outs);
}

// Evaluates the current c->in vector and checks the arithmetic identity.
static bool adder_check_vector(adder_check_t *c) {
    netlist_eval(c->nl, c->in, c->nets);
    for (int i = 0; i < c->nl->output_count; i++) c->outs[i] = c->nets[c->nl->outputs[i]];

    int n = c->n;
    wide_t lhs = w_add(w_value(c->in, n, c->radix), w_value(c->in + n, n, c->radix));
    if (c->has_cin) lhs = w_add(lhs, w_from_int(c->in[2 * n]));

    int8_t carry = c->outs[n];
    wide_t rhs = w_value(c->outs, n, c->radix);
    if (carry != 0) rhs = w_add(rhs, w_mul_small(c->radix_pow_n, carry));

    return w_eq(lhs, rhs);
}

static char digit_char(int8_t d) {
    return d < 0 ? '-' : (d == 0 ? '0' : '+');
}

// Operands most significant digit first, e.g. "first failure: a=+0- b=--+ cin=0".
static void format_vector(const adder_check_t *c, char *buf, size_t size) {
    char a[128], b[128];
    int n = c->n;
    for (int i = 0; i < n; i++) {
        a[i] = digit_char(c->in[n - 1 - i]);
        b[i] = digit_char(c->in[2 * n - 1 - i]);
    }
    a[n] = b[n] = '\0';
    if (c->radix == 2) {
        for (int i = 0; i < n; i++) {
            a[i] = (a[i] == '+') ? '1' : '0';
            b[i] = (b[i] == '+') ? '1' : '0';
        }
    }
    if (c->has_cin) snprintf(buf, size, "first failure: a=%s b=%s cin=%d", a, b, c->in[2 * n]);
    else snprintf(buf, size, "first failure: a=%s b=%s", a, b);
}

static int8_t digit_from_index(int radix, int idx) {
    return (int8_t)(radix == 3 ? idx - 1 : idx);
}

static void verify_adder_exhaustive(ctx_t *ctx, const netlist_t *nl, int n, int radix, bool has_cin,
                                    const char *what) {
    adder_check_t c;
    adder_check_init(&c, nl, n, radix, has_cin);
    int k = nl->input_count;
    int *idx = calloc((size_t)k, sizeof(int));
    long cases = 0;
    bool ok = true;
    char detail[256] = "";

    for (;;) {
        for (int i = 0; i < k; i++) c.in[i] = digit_from_index(radix, idx[i]);
        cases++;
        if (!adder_check_vector(&c) && ok) {
            ok = false;
            format_vector(&c, detail, sizeof(detail));
        }
        int i = 0;
        while (i < k && ++idx[i] == radix) idx[i++] = 0;
        if (i == k) break;
    }

    report(ctx, ok, what, cases, detail);
    free(idx);
    adder_check_free(&c);
}

// Carry-chain stress patterns followed by uniformly random digit vectors.
static void verify_adder_random(ctx_t *ctx, const netlist_t *nl, int n, int radix, const char *what) {
    adder_check_t c;
    adder_check_init(&c, nl, n, radix, true);
    int8_t lo = (int8_t)(radix == 3 ? -1 : 0);
    int8_t hi = 1;
    long cases = 0;
    bool ok = true;
    char detail[1024] = "";

    // Edge cases: {a fill, b fill, b lsb, cin}. A "fill" sets every digit.
    const int8_t edge[][4] = {
        { hi, hi, hi, hi },   // maximum + maximum + carry-in
        { lo, lo, lo, lo },   // minimum + minimum
        { hi, 0,  hi, 0  },   // all-max + 1: carry runs through every stage
        { lo, 0,  lo, lo },   // all-min - 1 (ternary) / all-zero (binary)
        { hi, lo, lo, 0  },   // x + (-x) style cancellation
        { 0,  0,  0,  hi },   // carry-in only
    };
    for (size_t e = 0; e < sizeof(edge) / sizeof(edge[0]); e++) {
        for (int i = 0; i < n; i++) {
            c.in[i] = edge[e][0];
            c.in[n + i] = edge[e][1];
        }
        c.in[n] = edge[e][2];
        c.in[2 * n] = edge[e][3];
        cases++;
        if (!adder_check_vector(&c) && ok) {
            ok = false;
            format_vector(&c, detail, sizeof(detail));
        }
    }

    rng_state = GATE_VERIFY_SEED;
    for (int t = 0; t < GATE_VERIFY_RANDOM_CASES; t++) {
        for (int i = 0; i < nl->input_count; i++) {
            c.in[i] = digit_from_index(radix, (int)(rng_next() % (uint64_t)radix));
        }
        cases++;
        if (!adder_check_vector(&c) && ok) {
            ok = false;
            format_vector(&c, detail, sizeof(detail));
        }
    }

    report(ctx, ok, what, cases, detail);
    adder_check_free(&c);
}

static void verify_ternary(ctx_t *ctx) {
    char what[96];
    for (int v = 0; v < TADD_VERSION_COUNT; v++) {
        const char *vn = tadd_version_name((tadd_version_t)v);
        netlist_t nl;

        netlist_init(&nl, "tern half adder", DOMAIN_TERNARY);
        tern_build_half_adder(&nl, (tadd_version_t)v);
        snprintf(what, sizeof(what), "ternary half adder %s, exhaustive", vn);
        verify_adder_exhaustive(ctx, &nl, 1, 3, false, what);
        netlist_free(&nl);

        netlist_init(&nl, "tern full adder", DOMAIN_TERNARY);
        tern_build_full_adder(&nl, (tadd_version_t)v);
        snprintf(what, sizeof(what), "ternary full adder %s, exhaustive", vn);
        verify_adder_exhaustive(ctx, &nl, 1, 3, true, what);
        netlist_free(&nl);

        netlist_init(&nl, "tern ripple 4", DOMAIN_TERNARY);
        tern_build_ripple_adder(&nl, (tadd_version_t)v, 4);
        snprintf(what, sizeof(what), "ternary 4-trit ripple adder %s, exhaustive", vn);
        verify_adder_exhaustive(ctx, &nl, 4, 3, true, what);
        netlist_free(&nl);

        const int sizes[2] = { 27, 41 };
        for (int s = 0; s < 2; s++) {
            netlist_init(&nl, "tern ripple", DOMAIN_TERNARY);
            tern_build_ripple_adder(&nl, (tadd_version_t)v, sizes[s]);
            snprintf(what, sizeof(what), "ternary %d-trit ripple adder %s, random", sizes[s], vn);
            verify_adder_random(ctx, &nl, sizes[s], 3, what);
            netlist_free(&nl);
        }
    }
}

static void verify_binary(ctx_t *ctx) {
    char what[96];
    netlist_t nl;

    netlist_init(&nl, "bin full adder", DOMAIN_BINARY);
    bin_build_full_adder(&nl);
    verify_adder_exhaustive(ctx, &nl, 1, 2, true, "binary full adder (9 NAND), exhaustive");
    netlist_free(&nl);

    netlist_init(&nl, "bin ripple 4", DOMAIN_BINARY);
    bin_build_ripple_adder(&nl, 4);
    verify_adder_exhaustive(ctx, &nl, 4, 2, true, "binary 4-bit ripple adder, exhaustive");
    netlist_free(&nl);

    const int sizes[2] = { 43, 64 };
    for (int s = 0; s < 2; s++) {
        netlist_init(&nl, "bin ripple", DOMAIN_BINARY);
        bin_build_ripple_adder(&nl, sizes[s]);
        snprintf(what, sizeof(what), "binary %d-bit ripple adder, random", sizes[s]);
        verify_adder_random(ctx, &nl, sizes[s], 2, what);
        netlist_free(&nl);
    }
}

int gate_verify_all(FILE *out) {
    ctx_t ctx = { out, 0 };
    fprintf(out, "Verification (random seed 0x%08X, %d random vectors per wide adder)\n",
            GATE_VERIFY_SEED, GATE_VERIFY_RANDOM_CASES);
    verify_primitives(&ctx);
    verify_ternary(&ctx);
    verify_binary(&ctx);
    fprintf(out, "  => %s (%d failed)\n\n", ctx.failed ? "VERIFICATION FAILED" : "all checks passed", ctx.failed);
    return ctx.failed;
}
