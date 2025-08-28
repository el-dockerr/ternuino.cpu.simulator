#ifndef TERNIO_H
#define TERNIO_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Ternary I/O format constants
#define TERNARY_FILE_EXTENSION ".t3"
#define TERNARY_FILE_HEADER "T3FMT"
#define TERNARY_FORMAT_VERSION 1

// Ternary file format header
typedef struct {
    char header[6];     // "T3FMT\0"
    uint8_t version;    // Format version
    uint32_t data_size; // Number of ternary values in file
} t3_header_t;

// Ternary I/O functions
bool t3_create_file(const char *filename);
bool t3_write_value(FILE *file, int32_t value);
bool t3_read_value(FILE *file, int32_t *value);
bool t3_write_header(FILE *file, uint32_t data_size);
bool t3_read_header(FILE *file, t3_header_t *header);

// Balanced ternary string conversion
void int_to_balanced_ternary(int32_t value, char *buffer, int buffer_size);
int32_t balanced_ternary_to_int(const char *bt_string);
bool is_valid_balanced_ternary(const char *bt_string);

// Utility functions
char trit_to_char(int8_t trit);
int8_t char_to_trit(char c);

#endif // TERNIO_H
