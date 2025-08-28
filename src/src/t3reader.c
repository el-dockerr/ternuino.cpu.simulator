#include <stdio.h>
#include "ternio.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.t3>\n", argv[0]);
        return 1;
    }
    
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    t3_header_t header;
    if (!t3_read_header(file, &header)) {
        printf("Error: Invalid ternary file format\n");
        fclose(file);
        return 1;
    }
    
    printf("Ternary File: %s\n", argv[1]);
    printf("Format: %s v%d\n", header.header, header.version);
    printf("Data size: %u values\n", header.data_size);
    printf("\nContents:\n");
    
    int32_t value;
    int count = 0;
    while (t3_read_value(file, &value)) {
        char bt_string[64];
        int_to_balanced_ternary(value, bt_string, sizeof(bt_string));
        printf("  %d: %d (balanced ternary: %s)\n", count++, value, bt_string);
    }
    
    if (count == 0) {
        printf("  (no values found)\n");
    }
    
    fclose(file);
    return 0;
}
