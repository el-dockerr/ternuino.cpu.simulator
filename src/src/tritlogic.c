#include "tritlogic.h"

int32_t trit_and(int32_t a, int32_t b) {
    // Ternary AND: min(a, b)
    return (a < b) ? a : b;
}

int32_t trit_or(int32_t a, int32_t b) {
    // Ternary OR: max(a, b)
    return (a > b) ? a : b;
}

int32_t trit_not(int32_t a) {
    // Ternary NOT: -a
    return -a;
}
