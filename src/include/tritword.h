#ifndef TRITWORD_H
#define TRITWORD_H

#include <stdint.h>
#include <stdbool.h>

#define TRITWORD_SIZE 6

typedef struct {
    int8_t trits[TRITWORD_SIZE];
    int32_t size;
} tritword_t;

// TritWord functions
void tritword_init(tritword_t *tw, int8_t *trits, int32_t size);
void tritword_init_empty(tritword_t *tw, int32_t size);
void tritword_from_int(tritword_t *tw, int32_t value, int32_t size);
int32_t tritword_to_int(const tritword_t *tw);
void tritword_to_string(const tritword_t *tw, char *buffer, int32_t buffer_size);

#endif // TRITWORD_H
