#ifndef TERNUINO_H
#define TERNUINO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Forward declarations
struct device_s;

// Ternary values: -1, 0, 1
typedef int8_t trit_t;

#define MAX_MEMORY_SIZE 27
#define MAX_DATA_MEMORY_SIZE 27
#define MAX_OPEN_FILES 8
#define MAX_DEVICES 8
#define MAX_IRQ_VECTORS 8

// File handle structure for I/O operations
typedef struct {
    FILE *file;
    char filename[256];
    bool is_open;
    bool is_write_mode;
} tfile_t;

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
    OP_TJP,
    OP_TOPEN,  // Open device for I/O
    OP_TREAD,  // Read from device
    OP_TWRITE, // Write to device
    OP_TCLOSE, // Close device
    OP_IRQ,    // Software interrupt
    OP_IRET,   // Return from interrupt
    OP_EI,     // Enable interrupts
    OP_DI      // Disable interrupts
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

// Interrupt vector table entry
typedef struct {
    int32_t handler_address;
    bool enabled;
} irq_entry_t;

// CPU state structure
typedef struct ternuino_s {
    int32_t registers[3];  // A, B, C
    int32_t pc;            // Program counter
    int32_t sp;            // Stack pointer (for interrupt handling)
    bool running;          // CPU running state
    bool interrupts_enabled; // Global interrupt enable flag
    bool in_interrupt;     // Currently handling interrupt
    instruction_t memory[MAX_MEMORY_SIZE];  // Instruction memory
    int32_t data_mem[MAX_DATA_MEMORY_SIZE]; // Data memory
    int32_t dmem_size;     // Actual data memory size
    bool memory_valid[MAX_MEMORY_SIZE];     // Track which memory slots have valid instructions
    tfile_t files[MAX_OPEN_FILES];          // File handles for I/O operations (legacy)
    
    // Interrupt and device management
    irq_entry_t irq_table[MAX_IRQ_VECTORS]; // Interrupt vector table
    struct device_s *devices[MAX_DEVICES];  // Device array
    int32_t device_count;                   // Number of registered devices
    int32_t pending_irq;                    // Pending interrupt vector (-1 if none)
    int32_t saved_pc;                       // Saved PC for interrupt return
} ternuino_t;

// Core CPU functions
void ternuino_init(ternuino_t *cpu, int32_t dmem_size);
void ternuino_reset(ternuino_t *cpu);
void ternuino_load_program(ternuino_t *cpu, instruction_t *program, int32_t program_size, 
                          int32_t *data, int32_t data_size);
void ternuino_step(ternuino_t *cpu);
void ternuino_run(ternuino_t *cpu);

// Interrupt handling functions
void ternuino_set_irq_handler(ternuino_t *cpu, int32_t vector, int32_t handler_address);
void ternuino_enable_irq(ternuino_t *cpu, int32_t vector);
void ternuino_disable_irq(ternuino_t *cpu, int32_t vector);
void ternuino_trigger_irq(ternuino_t *cpu, int32_t vector);
void ternuino_check_interrupts(ternuino_t *cpu);

// Device management functions
int32_t ternuino_register_device(ternuino_t *cpu, struct device_s *device);
void ternuino_unregister_device(ternuino_t *cpu, int32_t device_id);
struct device_s* ternuino_get_device(ternuino_t *cpu, int32_t device_id);
void ternuino_tick_devices(ternuino_t *cpu);

// Helper functions
const char* opcode_to_string(opcode_t opcode);
const char* register_to_string(ternuino_register_t reg);
void print_instruction(const instruction_t *instr);
void print_cpu_state(const ternuino_t *cpu);

#endif // TERNUINO_H
