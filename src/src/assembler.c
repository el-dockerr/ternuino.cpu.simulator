#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void assembler_init(assembler_t *asm_state) {
    asm_state->label_count = 0;
    asm_state->data_size = 0;
    asm_state->in_data_section = false;
    memset(asm_state->labels, 0, sizeof(asm_state->labels));
    memset(asm_state->data_image, 0, sizeof(asm_state->data_image));
}

static void trim_string(char *str) {
    // Remove leading whitespace
    char *start = str;
    while (isspace(*start)) start++;
    
    // Remove trailing whitespace
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    
    // Move trimmed string to beginning and null-terminate
    int len = end - start + 1;
    memmove(str, start, len);
    str[len] = '\0';
}

static void remove_comments(char *line) {
    char *comment = strchr(line, '#');
    if (comment) *comment = '\0';
    
    comment = strchr(line, ';');
    if (comment) *comment = '\0';
}

static bool add_label(assembler_t *asm_state, const char *name, int32_t address, bool is_data) {
    if (asm_state->label_count >= MAX_LABELS) {
        printf("Error: Too many labels (max %d)\n", MAX_LABELS);
        return false;
    }
    
    // Check for duplicate labels
    for (int i = 0; i < asm_state->label_count; i++) {
        if (strcmp(asm_state->labels[i].name, name) == 0 && 
            asm_state->labels[i].is_data_label == is_data) {
            printf("Error: Duplicate label '%s'\n", name);
            return false;
        }
    }
    
    snprintf(asm_state->labels[asm_state->label_count].name, MAX_LABEL_LENGTH, "%s", name);
    asm_state->labels[asm_state->label_count].address = address;
    asm_state->labels[asm_state->label_count].is_data_label = is_data;
    asm_state->label_count++;
    
    return true;
}

opcode_t string_to_opcode(const char *str) {
    if (strcmp(str, "NOP") == 0) return OP_NOP;
    if (strcmp(str, "MOV") == 0) return OP_MOV;
    if (strcmp(str, "ADD") == 0) return OP_ADD;
    if (strcmp(str, "SUB") == 0) return OP_SUB;
    if (strcmp(str, "MUL") == 0) return OP_MUL;
    if (strcmp(str, "DIV") == 0) return OP_DIV;
    if (strcmp(str, "JMP") == 0) return OP_JMP;
    if (strcmp(str, "HLT") == 0) return OP_HLT;
    if (strcmp(str, "LEA") == 0) return OP_LEA;
    if (strcmp(str, "LD") == 0) return OP_LD;
    if (strcmp(str, "ST") == 0) return OP_ST;
    if (strcmp(str, "TAND") == 0) return OP_TAND;
    if (strcmp(str, "TOR") == 0) return OP_TOR;
    if (strcmp(str, "TNOT") == 0) return OP_TNOT;
    if (strcmp(str, "NEG") == 0) return OP_NEG;
    if (strcmp(str, "TSIGN") == 0) return OP_TSIGN;
    if (strcmp(str, "TABS") == 0) return OP_TABS;
    if (strcmp(str, "TSHL3") == 0) return OP_TSHL3;
    if (strcmp(str, "TSHR3") == 0) return OP_TSHR3;
    if (strcmp(str, "TCMPR") == 0) return OP_TCMPR;
    if (strcmp(str, "TJZ") == 0) return OP_TJZ;
    if (strcmp(str, "TJN") == 0) return OP_TJN;
    if (strcmp(str, "TJP") == 0) return OP_TJP;
    return OP_NOP;  // Default for unknown opcodes
}

ternuino_register_t string_to_register(const char *str) {
    if (strcmp(str, "A") == 0) return REG_A;
    if (strcmp(str, "B") == 0) return REG_B;
    if (strcmp(str, "C") == 0) return REG_C;
    return REG_A;  // Default
}

bool is_valid_register(const char *str) {
    return (strcmp(str, "A") == 0 || strcmp(str, "B") == 0 || strcmp(str, "C") == 0);
}

bool is_indirect_operand(const char *str, char *reg_name) {
    int len = strlen(str);
    if (len >= 3 && str[0] == '[' && str[len-1] == ']') {
        strncpy(reg_name, str + 1, len - 2);
        reg_name[len - 2] = '\0';
        return is_valid_register(reg_name);
    }
    return false;
}

bool parse_operand(const char *token, operand_t *operand) {
    char reg_name[4];
    
    // Check for indirect addressing [A], [B], [C]
    if (is_indirect_operand(token, reg_name)) {
        operand->mode = ADDR_INDIRECT;
        operand->value.reg = string_to_register(reg_name);
        return true;
    }
    
    // Check for register
    if (is_valid_register(token)) {
        operand->mode = ADDR_REGISTER;
        operand->value.reg = string_to_register(token);
        return true;
    }
    
    // Check for immediate integer
    char *endptr;
    long val = strtol(token, &endptr, 10);
    if (*endptr == '\0') {
        operand->mode = ADDR_IMMEDIATE;
        operand->value.immediate = (int32_t)val;
        return true;
    }
    
    // Otherwise, it's a label (will be resolved later)
    operand->mode = ADDR_DIRECT;
    operand->value.address = -1;  // Mark as unresolved
    return true;
}

static bool parse_data_directive(assembler_t *asm_state, char *line, const char *directive) {
    if (strncasecmp(line, directive, strlen(directive)) == 0) {
        char *args = line + strlen(directive);
        trim_string(args);
        
        if (strcmp(directive, ".word") == 0) {
            if (strlen(args) == 0) {
                printf("Error: .word expects a value\n");
                return false;
            }
            
            char *endptr;
            long val = strtol(args, &endptr, 10);
            if (*endptr != '\0') {
                printf("Error: .word expects an integer\n");
                return false;
            }
            
            if (asm_state->data_size >= MAX_DATA_MEMORY_SIZE) {
                printf("Error: Data memory overflow\n");
                return false;
            }
            
            asm_state->data_image[asm_state->data_size++] = (int32_t)val;
            return true;
        }
        
        if (strcmp(directive, ".zero") == 0) {
            if (strlen(args) == 0) {
                printf("Error: .zero expects a count\n");
                return false;
            }
            
            char *endptr;
            long count = strtol(args, &endptr, 10);
            if (*endptr != '\0' || count < 0) {
                printf("Error: .zero expects a non-negative integer\n");
                return false;
            }
            
            if (asm_state->data_size + count > MAX_DATA_MEMORY_SIZE) {
                printf("Error: Data memory overflow\n");
                return false;
            }
            
            for (int i = 0; i < count; i++) {
                asm_state->data_image[asm_state->data_size++] = 0;
            }
            return true;
        }
    }
    
    return false;
}

bool parse_instruction_line(assembler_t *asm_state, const char *line, 
                           instruction_t *instr, bool *has_instruction) {
    char working_line[MAX_LINE_LENGTH];
    strncpy(working_line, line, MAX_LINE_LENGTH - 1);
    working_line[MAX_LINE_LENGTH - 1] = '\0';
    
    *has_instruction = false;
    
    // Remove comments and trim
    remove_comments(working_line);
    trim_string(working_line);
    
    if (strlen(working_line) == 0) {
        return true;  // Empty line is OK
    }
    
    // Skip lines that start with # (full line comments)
    if (working_line[0] == '#') {
        return true;
    }
    
    // Check for section directives
    if (strcasecmp(working_line, ".data") == 0) {
        asm_state->in_data_section = true;
        return true;
    }
    
    if (strcasecmp(working_line, ".text") == 0) {
        asm_state->in_data_section = false;
        return true;
    }
    
    // Handle data directives
    if (asm_state->in_data_section) {
        return parse_data_directive(asm_state, working_line, ".word") ||
               parse_data_directive(asm_state, working_line, ".zero");
    }
    
    // Parse instruction
    char tokens[MAX_TOKENS_PER_LINE][MAX_TOKEN_LENGTH];
    int token_count = 0;
    
    // Tokenize
    char *token = strtok(working_line, " ,\t");
    while (token && token_count < MAX_TOKENS_PER_LINE) {
        strncpy(tokens[token_count], token, MAX_TOKEN_LENGTH - 1);
        tokens[token_count][MAX_TOKEN_LENGTH - 1] = '\0';
        
        // Convert to uppercase for opcodes
        if (token_count == 0) {
            for (int i = 0; tokens[0][i]; i++) {
                tokens[0][i] = toupper(tokens[0][i]);
            }
        }
        
        token_count++;
        token = strtok(NULL, " ,\t");
    }
    
    if (token_count == 0) {
        return true;  // Empty line
    }
    
    // Parse opcode
    opcode_t opcode = string_to_opcode(tokens[0]);
    if (opcode == OP_NOP && strcmp(tokens[0], "NOP") != 0) {
        printf("Error: Unknown instruction '%s'\n", tokens[0]);
        return false;
    }
    
    instr->opcode = opcode;
    instr->has_operand1 = false;
    instr->has_operand2 = false;
    
    // Parse operands based on instruction type
    switch (opcode) {
        case OP_NOP:
        case OP_HLT:
            // No operands
            break;
            
        case OP_TNOT:
        case OP_NEG:
        case OP_TSIGN:
        case OP_TABS:
        case OP_TSHL3:
        case OP_TSHR3:
        case OP_JMP:
            // One operand
            if (token_count < 2) {
                printf("Error: '%s' expects 1 argument\n", tokens[0]);
                return false;
            }
            parse_operand(tokens[1], &instr->operand1);
            instr->has_operand1 = true;
            break;
            
        case OP_MOV:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_TAND:
        case OP_TOR:
        case OP_TJZ:
        case OP_TJN:
        case OP_TJP:
        case OP_TCMPR:
        case OP_LD:
        case OP_ST:
        case OP_LEA:
            // Two operands
            if (token_count < 3) {
                printf("Error: '%s' expects 2 arguments\n", tokens[0]);
                return false;
            }
            parse_operand(tokens[1], &instr->operand1);
            parse_operand(tokens[2], &instr->operand2);
            instr->has_operand1 = true;
            instr->has_operand2 = true;
            break;
    }
    
    *has_instruction = true;
    return true;
}

bool resolve_labels(assembler_t *asm_state, instruction_t *program, int32_t program_size) {
    (void)asm_state; // Suppress unused parameter warning
    
    for (int i = 0; i < program_size; i++) {
        instruction_t *instr = &program[i];
        
        // Resolve operand1 if it's a direct address with unresolved label
        if (instr->has_operand1 && instr->operand1.mode == ADDR_DIRECT && 
            instr->operand1.value.address == -1) {
            // This indicates an unresolved label
            // For now, we'll just set it to 0 as a placeholder
            // In a full implementation, you'd store the label name and resolve it here
            instr->operand1.value.address = 0;
        }
        
        // Resolve operand2 if it's a direct address with unresolved label
        if (instr->has_operand2 && instr->operand2.mode == ADDR_DIRECT && 
            instr->operand2.value.address == -1) {
            instr->operand2.value.address = 0;
        }
    }
    
    return true;
}

bool assembler_parse_file(assembler_t *asm_state, const char *filename, 
                         instruction_t *program, int32_t *program_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        return false;
    }
    
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    int address = 0;
    int data_address = 0;
    
    // First pass: collect labels and build program
    while (fgets(line, sizeof(line), file) && address < MAX_MEMORY_SIZE) {
        line_num++;
        
        // Make a copy for processing
        char working_line[MAX_LINE_LENGTH];
        strncpy(working_line, line, MAX_LINE_LENGTH - 1);
        working_line[MAX_LINE_LENGTH - 1] = '\0';
        
        // Remove comments first
        remove_comments(working_line);
        trim_string(working_line);
        
        // Skip empty lines
        if (strlen(working_line) == 0) {
            continue;
        }
        
        // Handle labels
        char *colon = strchr(working_line, ':');
        if (colon) {
            *colon = '\0';
            char label_name[MAX_LABEL_LENGTH];
            strncpy(label_name, working_line, MAX_LABEL_LENGTH - 1);
            label_name[MAX_LABEL_LENGTH - 1] = '\0';
            trim_string(label_name);
            
            if (strlen(label_name) > 0) {
                if (!add_label(asm_state, label_name, 
                              asm_state->in_data_section ? data_address : address, 
                              asm_state->in_data_section)) {
                    fclose(file);
                    return false;
                }
            }
            
            // Continue with the rest of the line after the colon
            strcpy(working_line, colon + 1);
            trim_string(working_line);
        }
        
        // Skip if nothing left after label processing
        if (strlen(working_line) == 0) {
            continue;
        }
        
        instruction_t instr;
        bool has_instruction;
        
        if (!parse_instruction_line(asm_state, working_line, &instr, &has_instruction)) {
            printf("Error on line %d\n", line_num);
            fclose(file);
            return false;
        }
        
        if (has_instruction && !asm_state->in_data_section) {
            program[address] = instr;
            address++;
        } else if (asm_state->in_data_section) {
            data_address = asm_state->data_size;
        }
    }
    
    fclose(file);
    
    *program_size = address;
    
    // Second pass: resolve labels
    return resolve_labels(asm_state, program, *program_size);
}
