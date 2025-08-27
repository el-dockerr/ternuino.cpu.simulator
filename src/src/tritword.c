#include "tritword.h"
#include <string.h>
#include <stdio.h>

void tritword_init(tritword_t *tw, int8_t *trits, int32_t size) {
    if (size > TRITWORD_SIZE) size = TRITWORD_SIZE;
    tw->size = size;
    
    if (trits) {
        memcpy(tw->trits, trits, size);
    } else {
        memset(tw->trits, 0, size);
    }
    
    // Fill remaining with zeros if size < TRITWORD_SIZE
    if (size < TRITWORD_SIZE) {
        memset(tw->trits + size, 0, TRITWORD_SIZE - size);
    }
}

void tritword_init_empty(tritword_t *tw, int32_t size) {
    tritword_init(tw, NULL, size);
}

void tritword_from_int(tritword_t *tw, int32_t value, int32_t size) {
    if (size > TRITWORD_SIZE) size = TRITWORD_SIZE;
    tw->size = size;
    
    for (int i = size - 1; i >= 0; i--) {
        int32_t rem = ((value + 1) % 3) - 1;  // Results in -1, 0, or 1
        tw->trits[i] = (int8_t)rem;
        value = (value - rem) / 3;
    }
    
    // Fill remaining with zeros if size < TRITWORD_SIZE
    if (size < TRITWORD_SIZE) {
        memset(tw->trits + size, 0, TRITWORD_SIZE - size);
    }
}

int32_t tritword_to_int(const tritword_t *tw) {
    int32_t result = 0;
    int32_t power = 1;
    
    for (int i = tw->size - 1; i >= 0; i--) {
        result += tw->trits[i] * power;
        power *= 3;
    }
    
    return result;
}

void tritword_to_string(const tritword_t *tw, char *buffer, int32_t buffer_size) {
    if (buffer_size < tw->size + 1) {
        buffer[0] = '\0';
        return;
    }
    
    for (int i = 0; i < tw->size; i++) {
        switch (tw->trits[i]) {
            case -1: buffer[i] = '-'; break;
            case 0:  buffer[i] = '0'; break;
            case 1:  buffer[i] = '+'; break;
            default: buffer[i] = '?'; break;
        }
    }
    buffer[tw->size] = '\0';
}
