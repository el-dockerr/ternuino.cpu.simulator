#ifndef TRITARITH_H
#define TRITARITH_H

#include <stdint.h>

// Ternary arithmetic operations
int32_t tsign(int32_t v);
int32_t tabs(int32_t v);
int32_t tshl3(int32_t v);
int32_t tshr3(int32_t v);
int32_t tcmpr(int32_t a, int32_t b);

#endif // TRITARITH_H
