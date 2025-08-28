#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "ternuino.h"
#include "assembler.h"
#include "tritword.h"

#ifdef _WIN32
#include <windows.h>
#define PATH_SEPARATOR '\\'
#else
#include <unistd.h>
#define PATH_SEPARATOR '/'
#endif

void print_program(instruction_t *program, int32_t program_size) {
    printf("\nParsed program:\n");
    for (int i = 0; i < program_size; i++) {
        printf("  %2d: ", i);
        print_instruction(&program[i]);
        printf("\n");
    }
    printf("\n");
}

void print_data_memory(ternuino_t *cpu, int32_t max_elements) {
    printf("Data memory[0:%d]: ", max_elements - 1);
    for (int i = 0; i < max_elements && i < cpu->dmem_size; i++) {
        printf("%d ", cpu->data_mem[i]);
    }
    printf("\n");
}

bool run_program_file(const char *filename) {
    printf("=== Running program: %s ===\n", filename);
    
    // Check if file exists
    FILE *test_file = fopen(filename, "r");
    if (!test_file) {
        printf("Error: File '%s' not found.\n", filename);
        return false;
    }
    fclose(test_file);
    
    // Initialize assembler
    assembler_t assembler;
    assembler_init(&assembler);
    
    // Parse the assembly file
    instruction_t program[MAX_MEMORY_SIZE];
    int32_t program_size;
    
    if (!assembler_parse_file(&assembler, filename, program, &program_size)) {
        printf("Error: Failed to parse assembly file.\n");
        return false;
    }
    
    printf("Loaded %d instructions, data cells: %d\n", program_size, assembler.data_size);
    
    // Display the parsed program
    print_program(program, program_size);
    
    // Create and run the CPU
    ternuino_t cpu;
    ternuino_init(&cpu, MAX_DATA_MEMORY_SIZE);
    ternuino_load_program(&cpu, program, program_size, assembler.data_image, assembler.data_size);
    
    printf("Initial registers: ");
    print_cpu_state(&cpu);
    if (assembler.data_size > 0) {
        print_data_memory(&cpu, 9);
    }
    
    // Run the program
    ternuino_run(&cpu);
    
    printf("Final registers:   ");
    print_cpu_state(&cpu);
    if (assembler.data_size > 0) {
        print_data_memory(&cpu, 9);
    }
    printf("\n");
    
    return true;
}

int list_available_programs(char programs[][256], int max_programs) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    
    dir = opendir("programs");
    if (dir == NULL) {
        printf("No programs directory found.\n");
        return 0;
    }
    
    while ((entry = readdir(dir)) != NULL && count < max_programs) {
        int len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".asm") == 0) {
            strncpy(programs[count], entry->d_name, 255);
            programs[count][255] = '\0';
            count++;
        }
    }
    
    closedir(dir);
    return count;
}

void interactive_mode(void) {
    printf("=== Ternuino CPU Interpreter ===\n");
    printf("Ternary Computer Simulator with Assembly Language Support\n\n");
    
    char programs[100][256];
    int program_count;
    char input[256];
    int choice;
    char program_path[512];
    
    while (1) {
        printf("Available programs:\n");
        program_count = list_available_programs(programs, 100);
        
        if (program_count == 0) {
            printf("  No assembly programs found in 'programs/' directory\n");
            break;
        }
        
        for (int i = 0; i < program_count; i++) {
            printf("  %d. %s\n", i + 1, programs[i]);
        }
        
        printf("  0. Exit\n");
        printf("\nSelect a program to run (number): ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        choice = atoi(input);
        
        if (choice == 0) {
            printf("Goodbye!\n");
            break;
        }
        
        if (choice >= 1 && choice <= program_count) {
            snprintf(program_path, sizeof(program_path), "programs%c%s", PATH_SEPARATOR, programs[choice - 1]);
            run_program_file(program_path);
        } else {
            printf("Invalid selection. Please try again.\n\n");
        }
    }
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc > 1) {
        // Run specific program file
        run_program_file(argv[1]);
    } else {
        // Interactive mode
        interactive_mode();
    }
    
    return 0;
}
