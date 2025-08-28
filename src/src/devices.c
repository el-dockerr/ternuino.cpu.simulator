#include "devices.h"
#include "ternuino.h"
#include "ternio.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// General device initialization
void device_init(device_t *dev, device_type_t type, uint8_t device_id, uint8_t irq_vector) {
    dev->type = type;
    dev->device_id = device_id;
    dev->status = DEVICE_READY;
    dev->irq_vector = irq_vector;
    dev->irq_enabled = false;
    dev->device_data = NULL;
    
    // Initialize function pointers to NULL
    dev->read = NULL;
    dev->write = NULL;
    dev->open = NULL;
    dev->close = NULL;
    dev->tick = NULL;
}

void device_cleanup(device_t *dev) {
    if (dev->close) {
        dev->close(dev);
    }
    if (dev->device_data) {
        free(dev->device_data);
        dev->device_data = NULL;
    }
}

// Terminal device implementation
device_t* terminal_device_create(uint8_t device_id, uint8_t irq_vector) {
    device_t *dev = malloc(sizeof(device_t));
    if (!dev) return NULL;
    
    device_init(dev, DEVICE_TERMINAL, device_id, irq_vector);
    
    terminal_data_t *tdata = malloc(sizeof(terminal_data_t));
    if (!tdata) {
        free(dev);
        return NULL;
    }
    
    // Initialize terminal data
    memset(tdata->input_buffer, 0, sizeof(tdata->input_buffer));
    tdata->input_pos = 0;
    tdata->input_len = 0;
    tdata->input_ready = false;
    tdata->echo_enabled = true;
    
    dev->device_data = tdata;
    dev->read = terminal_read;
    dev->write = terminal_write;
    dev->open = terminal_open;
    dev->close = terminal_close;
    dev->tick = terminal_tick;
    
    return dev;
}

int32_t terminal_read(device_t *dev, int32_t *value) {
    if (!dev || !dev->device_data || !value) return -1;
    
    terminal_data_t *tdata = (terminal_data_t*)dev->device_data;
    
    if (!tdata->input_ready || tdata->input_pos >= tdata->input_len) {
        return -1; // No data available
    }
    
    *value = (int32_t)tdata->input_buffer[tdata->input_pos++];
    
    // If we've read all available data, reset
    if (tdata->input_pos >= tdata->input_len) {
        tdata->input_ready = false;
        tdata->input_pos = 0;
        tdata->input_len = 0;
        dev->status &= ~DEVICE_IRQ_PENDING;
    }
    
    return 0; // Success
}

int32_t terminal_write(device_t *dev, int32_t value) {
    if (!dev) return -1;
    
    // Convert value to character and print
    if (value >= 0 && value <= 255) {
        char c = (char)value;
        putchar(c);
        fflush(stdout);
        return 0;
    }
    
    // For values outside ASCII range, just output them as numbers
    printf("[%d]", value);
    fflush(stdout);
    return 0;
}

int32_t terminal_open(device_t *dev, int32_t mode) {
    if (!dev || !dev->device_data) return -1;
    
    terminal_data_t *tdata = (terminal_data_t*)dev->device_data;
    
    // Mode 0: Read mode (enable input)
    // Mode 1: Write mode (enable output)
    // Mode 2: Read/Write mode
    
    if (mode == 0 || mode == 2) {
        // Enable input - set up for non-blocking input
        tdata->input_ready = false;
        tdata->input_pos = 0;
        tdata->input_len = 0;
    }
    
    dev->irq_enabled = true;
    dev->status = DEVICE_READY;
    
    return 0; // Success
}

int32_t terminal_close(device_t *dev) {
    if (!dev) return -1;
    
    dev->irq_enabled = false;
    dev->status = DEVICE_READY;
    
    return 0;
}

void terminal_tick(device_t *dev, struct ternuino_s *cpu) {
    if (!dev || !dev->device_data || !dev->irq_enabled) return;
    
    terminal_data_t *tdata = (terminal_data_t*)dev->device_data;
    
    // Check for input availability
    if (!tdata->input_ready) {
#ifdef _WIN32
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r') c = '\n'; // Convert CR to LF
            
            tdata->input_buffer[0] = c;
            tdata->input_len = 1;
            tdata->input_pos = 0;
            tdata->input_ready = true;
            
            if (tdata->echo_enabled) {
                putchar(c);
                fflush(stdout);
            }
            
            // Signal interrupt
            dev->status |= DEVICE_IRQ_PENDING;
        }
#else
        // Non-blocking input for Unix-like systems
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            tdata->input_buffer[0] = c;
            tdata->input_len = 1;
            tdata->input_pos = 0;
            tdata->input_ready = true;
            
            if (tdata->echo_enabled) {
                putchar(c);
                fflush(stdout);
            }
            
            // Signal interrupt
            dev->status |= DEVICE_IRQ_PENDING;
        }
        
        fcntl(STDIN_FILENO, F_SETFL, flags);
#endif
    }
}

// File device implementation
device_t* file_device_create(uint8_t device_id, uint8_t irq_vector) {
    device_t *dev = malloc(sizeof(device_t));
    if (!dev) return NULL;
    
    device_init(dev, DEVICE_FILE, device_id, irq_vector);
    
    file_data_t *fdata = malloc(sizeof(file_data_t));
    if (!fdata) {
        free(dev);
        return NULL;
    }
    
    // Initialize file data
    fdata->file = NULL;
    memset(fdata->filename, 0, sizeof(fdata->filename));
    fdata->is_open = false;
    fdata->is_write_mode = false;
    
    dev->device_data = fdata;
    dev->read = file_read;
    dev->write = file_write;
    dev->open = file_open;
    dev->close = file_close;
    dev->tick = file_tick;
    
    return dev;
}

int32_t file_read(device_t *dev, int32_t *value) {
    if (!dev || !dev->device_data || !value) return -1;
    
    file_data_t *fdata = (file_data_t*)dev->device_data;
    
    if (!fdata->is_open || fdata->is_write_mode || !fdata->file) {
        return -1;
    }
    
    return t3_read_value(fdata->file, value) ? 0 : -1;
}

int32_t file_write(device_t *dev, int32_t value) {
    if (!dev || !dev->device_data) return -1;
    
    file_data_t *fdata = (file_data_t*)dev->device_data;
    
    if (!fdata->is_open || !fdata->is_write_mode || !fdata->file) {
        return -1;
    }
    
    return t3_write_value(fdata->file, value) ? 0 : -1;
}

int32_t file_open(device_t *dev, int32_t mode) {
    if (!dev || !dev->device_data) return -1;
    
    file_data_t *fdata = (file_data_t*)dev->device_data;
    
    // Close existing file if open
    if (fdata->is_open && fdata->file) {
        fclose(fdata->file);
        fdata->file = NULL;
        fdata->is_open = false;
    }
    
    // Generate filename based on device ID
    snprintf(fdata->filename, sizeof(fdata->filename), "ternary_%d.t3", dev->device_id);
    
    const char* file_mode = (mode == 0) ? "rb" : "wb";
    fdata->file = fopen(fdata->filename, file_mode);
    fdata->is_open = (fdata->file != NULL);
    fdata->is_write_mode = (mode != 0);
    
    // If writing, create header
    if (fdata->is_open && fdata->is_write_mode) {
        t3_write_header(fdata->file, 0);
    }
    
    return fdata->is_open ? 0 : -1;
}

int32_t file_close(device_t *dev) {
    if (!dev || !dev->device_data) return -1;
    
    file_data_t *fdata = (file_data_t*)dev->device_data;
    
    if (fdata->is_open && fdata->file) {
        fclose(fdata->file);
        fdata->file = NULL;
        fdata->is_open = false;
        fdata->filename[0] = '\0';
        return 0;
    }
    
    return -1;
}

void file_tick(device_t *dev, struct ternuino_s *cpu) {
    // File devices don't need regular ticking
    (void)dev;
    (void)cpu;
}
