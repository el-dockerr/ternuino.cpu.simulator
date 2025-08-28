#include "ternio.h"
#include <string.h>
#include <stdlib.h>

// Convert trit (-1, 0, 1) to character ('T', '0', '1')
char trit_to_char(int8_t trit) {
    switch (trit) {
        case -1: return 'T';  // T for negative (ternary minus)
        case 0:  return '0';  // 0 for neutral
        case 1:  return '1';  // 1 for positive
        default: return '?';  // Invalid trit
    }
}

// Convert character ('T', '0', '1') to trit (-1, 0, 1)
int8_t char_to_trit(char c) {
    switch (c) {
        case 'T': case 't': case '-': return -1;
        case '0': return 0;
        case '1': case '+': return 1;
        default: return 0;  // Default to neutral for invalid chars
    }
}

// Convert integer to balanced ternary string representation
void int_to_balanced_ternary(int32_t value, char *buffer, int buffer_size) {
    if (buffer_size < 2) {
        if (buffer_size > 0) buffer[0] = '\0';
        return;
    }
    
    if (value == 0) {
        strcpy(buffer, "0");
        return;
    }
    
    char temp[64];
    int pos = 0;
    int32_t n = value;
    
    while (n != 0 && pos < 63) {
        int32_t remainder = ((n % 3) + 3) % 3;
        if (remainder == 2) {
            remainder = -1;
            n = (n + 1) / 3;
        } else {
            n = n / 3;
        }
        temp[pos++] = trit_to_char(remainder);
    }
    
    // Reverse the string
    int len = (pos < buffer_size - 1) ? pos : buffer_size - 1;
    for (int i = 0; i < len; i++) {
        buffer[i] = temp[pos - 1 - i];
    }
    buffer[len] = '\0';
}

// Convert balanced ternary string to integer
int32_t balanced_ternary_to_int(const char *bt_string) {
    if (!bt_string || !*bt_string) return 0;
    
    int32_t result = 0;
    int32_t power = 1;
    int len = strlen(bt_string);
    
    for (int i = len - 1; i >= 0; i--) {
        int8_t trit = char_to_trit(bt_string[i]);
        result += trit * power;
        power *= 3;
    }
    
    return result;
}

// Check if string is valid balanced ternary
bool is_valid_balanced_ternary(const char *bt_string) {
    if (!bt_string) return false;
    
    for (int i = 0; bt_string[i] != '\0'; i++) {
        char c = bt_string[i];
        if (c != 'T' && c != 't' && c != '-' && c != '0' && c != '1' && c != '+') {
            return false;
        }
    }
    return true;
}

// Write ternary file header
bool t3_write_header(FILE *file, uint32_t data_size) {
    if (!file) return false;
    
    t3_header_t header;
    strcpy(header.header, TERNARY_FILE_HEADER);
    header.version = TERNARY_FORMAT_VERSION;
    header.data_size = data_size;
    
    return fwrite(&header, sizeof(t3_header_t), 1, file) == 1;
}

// Read ternary file header
bool t3_read_header(FILE *file, t3_header_t *header) {
    if (!file || !header) return false;
    
    if (fread(header, sizeof(t3_header_t), 1, file) != 1) {
        return false;
    }
    
    // Verify header
    if (strcmp(header->header, TERNARY_FILE_HEADER) != 0) {
        return false;
    }
    
    if (header->version != TERNARY_FORMAT_VERSION) {
        return false;
    }
    
    return true;
}

// Write a ternary value to file in balanced ternary format
bool t3_write_value(FILE *file, int32_t value) {
    if (!file) return false;
    
    char bt_string[64];
    int_to_balanced_ternary(value, bt_string, sizeof(bt_string));
    
    // Write length followed by the balanced ternary string
    uint8_t len = strlen(bt_string);
    if (fwrite(&len, sizeof(uint8_t), 1, file) != 1) {
        return false;
    }
    
    return fwrite(bt_string, sizeof(char), len, file) == len;
}

// Read a ternary value from file
bool t3_read_value(FILE *file, int32_t *value) {
    if (!file || !value) return false;
    
    uint8_t len;
    if (fread(&len, sizeof(uint8_t), 1, file) != 1) {
        return false;
    }
    
    if (len == 0 || len > 63) {
        return false;
    }
    
    char bt_string[64];
    if (fread(bt_string, sizeof(char), len, file) != len) {
        return false;
    }
    
    bt_string[len] = '\0';
    *value = balanced_ternary_to_int(bt_string);
    
    return true;
}

// Create a new ternary file
bool t3_create_file(const char *filename) {
    if (!filename) return false;
    
    FILE *file = fopen(filename, "wb");
    if (!file) return false;
    
    // Write header with zero data size (will be updated later)
    bool success = t3_write_header(file, 0);
    fclose(file);
    
    return success;
}
