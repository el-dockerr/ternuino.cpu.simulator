#include "ternuino.h"
#include "tritlogic.h"
#include "tritarith.h"
#include <stdio.h>
#include <string.h>

void ternuino_init(ternuino_t *cpu, int32_t dmem_size) {
    // Initialize registers
    cpu->registers[REG_A] = 0;
    cpu->registers[REG_B] = 0;
    cpu->registers[REG_C] = 0;
    
    // Initialize state
    cpu->pc = 0;
    cpu->running = true;
    cpu->dmem_size = (dmem_size > MAX_DATA_MEMORY_SIZE) ? MAX_DATA_MEMORY_SIZE : dmem_size;
    
    // Clear memory
    memset(cpu->memory, 0, sizeof(cpu->memory));
    memset(cpu->data_mem, 0, sizeof(cpu->data_mem));
    memset(cpu->memory_valid, false, sizeof(cpu->memory_valid));
}

void ternuino_reset(ternuino_t *cpu) {
    cpu->registers[REG_A] = 0;
    cpu->registers[REG_B] = 0;
    cpu->registers[REG_C] = 0;
    cpu->pc = 0;
    cpu->running = true;
}

void ternuino_load_program(ternuino_t *cpu, instruction_t *program, int32_t program_size, 
                          int32_t *data, int32_t data_size) {
    // Load program
    for (int i = 0; i < program_size && i < MAX_MEMORY_SIZE; i++) {
        cpu->memory[i] = program[i];
        cpu->memory_valid[i] = true;
    }
    
    // Load data memory
    if (data && data_size > 0) {
        int32_t copy_size = (data_size < cpu->dmem_size) ? data_size : cpu->dmem_size;
        memcpy(cpu->data_mem, data, copy_size * sizeof(int32_t));
        
        // Fill remaining with zeros if needed
        if (copy_size < cpu->dmem_size) {
            memset(cpu->data_mem + copy_size, 0, (cpu->dmem_size - copy_size) * sizeof(int32_t));
        }
    }
}

static int32_t resolve_operand_value(ternuino_t *cpu, const operand_t *operand) {
    switch (operand->mode) {
        case ADDR_IMMEDIATE:
            return operand->value.immediate;
        case ADDR_REGISTER:
            return cpu->registers[operand->value.reg];
        case ADDR_DIRECT:
            return operand->value.address % cpu->dmem_size;
        case ADDR_INDIRECT: {
            int32_t addr = cpu->registers[operand->value.reg] % cpu->dmem_size;
            return addr;
        }
        default:
            return 0;
    }
}

void ternuino_step(ternuino_t *cpu) {
    // Halt if PC out of memory bounds
    if (cpu->pc < 0 || cpu->pc >= MAX_MEMORY_SIZE) {
        cpu->running = false;
        return;
    }
    
    // Check if instruction is valid
    if (!cpu->memory_valid[cpu->pc]) {
        // If we encounter empty memory, stop to avoid running off the end
        if (cpu->pc >= MAX_MEMORY_SIZE - 1) {
            cpu->running = false;
        }
        cpu->pc++;
        return;
    }
    
    instruction_t *instr = &cpu->memory[cpu->pc];
    cpu->pc++;
    
    switch (instr->opcode) {
        case OP_NOP:
            // No operation
            break;
            
        case OP_MOV: {
            ternuino_register_t reg = instr->operand1.value.reg;
            int32_t val;
            if (instr->operand2.mode == ADDR_REGISTER) {
                val = cpu->registers[instr->operand2.value.reg];
            } else {
                val = resolve_operand_value(cpu, &instr->operand2);
            }
            cpu->registers[reg] = val;
            break;
        }
        
        case OP_ADD: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            cpu->registers[reg1] += cpu->registers[reg2];
            break;
        }
        
        case OP_SUB: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            cpu->registers[reg1] -= cpu->registers[reg2];
            break;
        }
        
        case OP_MUL: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            cpu->registers[reg1] *= cpu->registers[reg2];
            break;
        }
        
        case OP_DIV: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            if (cpu->registers[reg2] != 0) {
                cpu->registers[reg1] = cpu->registers[reg1] / cpu->registers[reg2];
            } else {
                cpu->registers[reg1] = 0;  // Division by zero: set result to 0
            }
            break;
        }
        
        case OP_JMP: {
            int32_t addr = resolve_operand_value(cpu, &instr->operand1);
            cpu->pc = addr;
            break;
        }
        
        case OP_HLT:
            cpu->running = false;
            break;
            
        case OP_LEA: {
            ternuino_register_t reg = instr->operand1.value.reg;
            if (instr->operand2.mode == ADDR_INDIRECT) {
                // LEA does not accept indirect; treat as error (silently ignored)
                break;
            }
            int32_t addr = resolve_operand_value(cpu, &instr->operand2);
            cpu->registers[reg] = addr % cpu->dmem_size;
            break;
        }
        
        case OP_LD: {
            ternuino_register_t reg = instr->operand1.value.reg;
            int32_t addr;
            if (instr->operand2.mode == ADDR_INDIRECT) {
                addr = cpu->registers[instr->operand2.value.reg] % cpu->dmem_size;
            } else {
                addr = resolve_operand_value(cpu, &instr->operand2) % cpu->dmem_size;
            }
            cpu->registers[reg] = cpu->data_mem[addr];
            break;
        }
        
        case OP_ST: {
            ternuino_register_t reg = instr->operand1.value.reg;
            int32_t addr;
            if (instr->operand2.mode == ADDR_INDIRECT) {
                addr = cpu->registers[instr->operand2.value.reg] % cpu->dmem_size;
            } else {
                addr = resolve_operand_value(cpu, &instr->operand2) % cpu->dmem_size;
            }
            cpu->data_mem[addr] = cpu->registers[reg];
            break;
        }
        
        case OP_TAND: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            cpu->registers[reg1] = trit_and(cpu->registers[reg1], cpu->registers[reg2]);
            break;
        }
        
        case OP_TOR: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            cpu->registers[reg1] = trit_or(cpu->registers[reg1], cpu->registers[reg2]);
            break;
        }
        
        case OP_TNOT: {
            ternuino_register_t reg = instr->operand1.value.reg;
            cpu->registers[reg] = trit_not(cpu->registers[reg]);
            break;
        }
        
        case OP_NEG: {
            ternuino_register_t reg = instr->operand1.value.reg;
            cpu->registers[reg] = -cpu->registers[reg];
            break;
        }
        
        case OP_TSIGN: {
            ternuino_register_t reg = instr->operand1.value.reg;
            cpu->registers[reg] = tsign(cpu->registers[reg]);
            break;
        }
        
        case OP_TABS: {
            ternuino_register_t reg = instr->operand1.value.reg;
            cpu->registers[reg] = tabs(cpu->registers[reg]);
            break;
        }
        
        case OP_TSHL3: {
            ternuino_register_t reg = instr->operand1.value.reg;
            cpu->registers[reg] = tshl3(cpu->registers[reg]);
            break;
        }
        
        case OP_TSHR3: {
            ternuino_register_t reg = instr->operand1.value.reg;
            cpu->registers[reg] = tshr3(cpu->registers[reg]);
            break;
        }
        
        case OP_TCMPR: {
            ternuino_register_t reg1 = instr->operand1.value.reg;
            ternuino_register_t reg2 = instr->operand2.value.reg;
            cpu->registers[reg1] = tcmpr(cpu->registers[reg1], cpu->registers[reg2]);
            break;
        }
        
        case OP_TJZ: {
            ternuino_register_t reg = instr->operand1.value.reg;
            int32_t addr = resolve_operand_value(cpu, &instr->operand2);
            if (cpu->registers[reg] == 0) {
                cpu->pc = addr;
            }
            break;
        }
        
        case OP_TJN: {
            ternuino_register_t reg = instr->operand1.value.reg;
            int32_t addr = resolve_operand_value(cpu, &instr->operand2);
            if (cpu->registers[reg] < 0) {
                cpu->pc = addr;
            }
            break;
        }
        
        case OP_TJP: {
            ternuino_register_t reg = instr->operand1.value.reg;
            int32_t addr = resolve_operand_value(cpu, &instr->operand2);
            if (cpu->registers[reg] > 0) {
                cpu->pc = addr;
            }
            break;
        }
    }
}

void ternuino_run(ternuino_t *cpu) {
    while (cpu->running) {
        ternuino_step(cpu);
    }
}

const char* opcode_to_string(opcode_t opcode) {
    switch (opcode) {
        case OP_NOP:   return "NOP";
        case OP_MOV:   return "MOV";
        case OP_ADD:   return "ADD";
        case OP_SUB:   return "SUB";
        case OP_MUL:   return "MUL";
        case OP_DIV:   return "DIV";
        case OP_JMP:   return "JMP";
        case OP_HLT:   return "HLT";
        case OP_LEA:   return "LEA";
        case OP_LD:    return "LD";
        case OP_ST:    return "ST";
        case OP_TAND:  return "TAND";
        case OP_TOR:   return "TOR";
        case OP_TNOT:  return "TNOT";
        case OP_NEG:   return "NEG";
        case OP_TSIGN: return "TSIGN";
        case OP_TABS:  return "TABS";
        case OP_TSHL3: return "TSHL3";
        case OP_TSHR3: return "TSHR3";
        case OP_TCMPR: return "TCMPR";
        case OP_TJZ:   return "TJZ";
        case OP_TJN:   return "TJN";
        case OP_TJP:   return "TJP";
        default:       return "UNKNOWN";
    }
}

const char* register_to_string(ternuino_register_t reg) {
    switch (reg) {
        case REG_A: return "A";
        case REG_B: return "B";
        case REG_C: return "C";
        default:    return "?";
    }
}

void print_instruction(const instruction_t *instr) {
    printf("%s", opcode_to_string(instr->opcode));
    
    if (instr->has_operand1) {
        printf(" ");
        switch (instr->operand1.mode) {
            case ADDR_IMMEDIATE:
                printf("%d", instr->operand1.value.immediate);
                break;
            case ADDR_REGISTER:
                printf("%s", register_to_string(instr->operand1.value.reg));
                break;
            case ADDR_DIRECT:
                printf("%d", instr->operand1.value.address);
                break;
            case ADDR_INDIRECT:
                printf("[%s]", register_to_string(instr->operand1.value.reg));
                break;
        }
        
        if (instr->has_operand2) {
            printf(", ");
            switch (instr->operand2.mode) {
                case ADDR_IMMEDIATE:
                    printf("%d", instr->operand2.value.immediate);
                    break;
                case ADDR_REGISTER:
                    printf("%s", register_to_string(instr->operand2.value.reg));
                    break;
                case ADDR_DIRECT:
                    printf("%d", instr->operand2.value.address);
                    break;
                case ADDR_INDIRECT:
                    printf("[%s]", register_to_string(instr->operand2.value.reg));
                    break;
            }
        }
    }
}

void print_cpu_state(const ternuino_t *cpu) {
    printf("Registers: A=%d, B=%d, C=%d\n", 
           cpu->registers[REG_A], cpu->registers[REG_B], cpu->registers[REG_C]);
    printf("PC: %d, Running: %s\n", cpu->pc, cpu->running ? "true" : "false");
}
