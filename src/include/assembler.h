#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include <stdbool.h>
#include "ternuino.h"

#define MAX_LABELS 128
#define MAX_LABEL_LENGTH 64
#define MAX_LINE_LENGTH 256
#define MAX_TOKEN_LENGTH 64
#define MAX_TOKENS_PER_LINE 8

// Label structure
typedef struct {
    char name[MAX_LABEL_LENGTH];
    int32_t address;
    bool is_data_label;
} label_t;

// Assembler state
typedef struct {
    label_t labels[MAX_LABELS];
    int32_t label_count;
    int32_t data_image[MAX_DATA_MEMORY_SIZE];
    int32_t data_size;
    bool in_data_section;
} assembler_t;

// Assembler functions
void assembler_init(assembler_t *asm_state);
bool assembler_parse_file(assembler_t *asm_state, const char *filename, 
                         instruction_t *program, int32_t *program_size);
bool parse_instruction_line(assembler_t *asm_state, const char *line, 
                           instruction_t *instr, bool *has_instruction);
bool resolve_labels(assembler_t *asm_state, instruction_t *program, int32_t program_size);

// Helper functions
bool parse_operand(const char *token, operand_t *operand);
opcode_t string_to_opcode(const char *str);
ternuino_register_t string_to_register(const char *str);
bool is_valid_register(const char *str);
bool is_indirect_operand(const char *str, char *reg_name);

#endif // ASSEMBLER_H
