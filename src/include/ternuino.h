#ifndef TERNUINO_H
#define TERNUINO_H

#include <stdint.h>
#include <stdbool.h>

// Ternary values: -1, 0, 1
typedef int8_t trit_t;

// Maximum memory and data memory sizes
#define MAX_MEMORY_SIZE 27
#define MAX_DATA_MEMORY_SIZE 27

// Instruction types
typedef enum {
    OP_NOP,
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_JMP,
    OP_HLT,
    OP_LEA,
    OP_LD,
    OP_ST,
    OP_TAND,
    OP_TOR,
    OP_TNOT,
    OP_NEG,
    OP_TSIGN,
    OP_TABS,
    OP_TSHL3,
    OP_TSHR3,
    OP_TCMPR,
    OP_TJZ,
    OP_TJN,
    OP_TJP
} opcode_t;

// Addressing modes
typedef enum {
    ADDR_IMMEDIATE,
    ADDR_REGISTER,
    ADDR_DIRECT,
    ADDR_INDIRECT
} addr_mode_t;

// Register identifiers
typedef enum {
    REG_A = 0,
    REG_B = 1,
    REG_C = 2
} ternuino_register_t;

// Operand structure
typedef struct {
    addr_mode_t mode;
    union {
        int32_t immediate;
        ternuino_register_t reg;
        int32_t address;
    } value;
} operand_t;

// Instruction structure
typedef struct {
    opcode_t opcode;
    operand_t operand1;
    operand_t operand2;
    bool has_operand1;
    bool has_operand2;
} instruction_t;

// CPU state structure
typedef struct {
    int32_t registers[3];  // A, B, C
    int32_t pc;            // Program counter
    bool running;          // CPU running state
    instruction_t memory[MAX_MEMORY_SIZE];  // Instruction memory
    int32_t data_mem[MAX_DATA_MEMORY_SIZE]; // Data memory
    int32_t dmem_size;     // Actual data memory size
    bool memory_valid[MAX_MEMORY_SIZE];     // Track which memory slots have valid instructions
} ternuino_t;

// Core CPU functions
void ternuino_init(ternuino_t *cpu, int32_t dmem_size);
void ternuino_reset(ternuino_t *cpu);
void ternuino_load_program(ternuino_t *cpu, instruction_t *program, int32_t program_size, 
                          int32_t *data, int32_t data_size);
void ternuino_step(ternuino_t *cpu);
void ternuino_run(ternuino_t *cpu);

// Helper functions
const char* opcode_to_string(opcode_t opcode);
const char* register_to_string(ternuino_register_t reg);
void print_instruction(const instruction_t *instr);
void print_cpu_state(const ternuino_t *cpu);

#endif // TERNUINO_H
