#include "tritarith.h"
#include <stdlib.h>

int32_t tsign(int32_t v) {
    // Return -1 if v < 0, 0 if v == 0, +1 if v > 0
    if (v < 0) return -1;
    if (v > 0) return 1;
    return 0;
}

int32_t tabs(int32_t v) {
    // Absolute value
    return abs(v);
}

int32_t tshl3(int32_t v) {
    // Scale by 3 (base-3 left shift)
    return v * 3;
}

int32_t tshr3(int32_t v) {
    // Divide by 3, truncating toward zero (base-3 right shift)
    return v / 3;
}

int32_t tcmpr(int32_t a, int32_t b) {
    // Three-way compare: sign(a - b) in {-1, 0, +1}
    int32_t diff = a - b;
    return tsign(diff);
}
