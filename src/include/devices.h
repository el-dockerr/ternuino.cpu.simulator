#ifndef DEVICES_H
#define DEVICES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Device types
typedef enum {
    DEVICE_NONE = 0,
    DEVICE_TERMINAL = 1,
    DEVICE_FILE = 2
} device_type_t;

// Device status flags
typedef enum {
    DEVICE_READY = 0x01,
    DEVICE_BUSY = 0x02,
    DEVICE_ERROR = 0x04,
    DEVICE_IRQ_PENDING = 0x08
} device_status_t;

// Forward declaration
struct ternuino_s;

// Device interface structure
typedef struct device_s {
    device_type_t type;
    uint8_t device_id;
    uint8_t status;
    uint8_t irq_vector;
    bool irq_enabled;
    
    // Device-specific operations
    int32_t (*read)(struct device_s *dev, int32_t *value);
    int32_t (*write)(struct device_s *dev, int32_t value);
    int32_t (*open)(struct device_s *dev, int32_t mode);
    int32_t (*close)(struct device_s *dev);
    void (*tick)(struct device_s *dev, struct ternuino_s *cpu);
    
    // Device-specific data
    void *device_data;
} device_t;

// Terminal device data
typedef struct {
    char input_buffer[256];
    int input_pos;
    int input_len;
    bool input_ready;
    bool echo_enabled;
} terminal_data_t;

// File device data
typedef struct {
    FILE *file;
    char filename[256];
    bool is_open;
    bool is_write_mode;
} file_data_t;

// Device management functions
void device_init(device_t *dev, device_type_t type, uint8_t device_id, uint8_t irq_vector);
void device_cleanup(device_t *dev);

// Terminal device functions
device_t* terminal_device_create(uint8_t device_id, uint8_t irq_vector);
int32_t terminal_read(device_t *dev, int32_t *value);
int32_t terminal_write(device_t *dev, int32_t value);
int32_t terminal_open(device_t *dev, int32_t mode);
int32_t terminal_close(device_t *dev);
void terminal_tick(device_t *dev, struct ternuino_s *cpu);

// File device functions
device_t* file_device_create(uint8_t device_id, uint8_t irq_vector);
int32_t file_read(device_t *dev, int32_t *value);
int32_t file_write(device_t *dev, int32_t value);
int32_t file_open(device_t *dev, int32_t mode);
int32_t file_close(device_t *dev);
void file_tick(device_t *dev, struct ternuino_s *cpu);

#endif // DEVICES_H
