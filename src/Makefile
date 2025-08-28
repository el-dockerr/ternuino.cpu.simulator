# Ternuino CPU Simulator - C Version
# Makefile for building the project

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude
LDFLAGS = 

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj

# Source files (excluding utilities)
MAIN_SOURCES = $(SRCDIR)/main.c $(SRCDIR)/ternuino.c $(SRCDIR)/assembler.c $(SRCDIR)/tritlogic.c $(SRCDIR)/tritarith.c $(SRCDIR)/tritword.c $(SRCDIR)/ternio.c $(SRCDIR)/devices.c
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Utility sources
UTIL_SOURCES = $(SRCDIR)/t3reader.c $(SRCDIR)/ternio.c
T3READER_OBJECTS = $(OBJDIR)/t3reader.o $(OBJDIR)/ternio.o

# Target executables
TARGET = $(BUILDDIR)/ternuino
T3READER = $(BUILDDIR)/t3reader

# Default target
all: $(TARGET) $(T3READER)

# Create build directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Build the main executable
$(TARGET): $(MAIN_OBJECTS) | $(OBJDIR)
	@mkdir -p $(BUILDDIR)
	$(CC) $(MAIN_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built $(TARGET)"

# Build the T3 reader utility
$(T3READER): $(T3READER_OBJECTS) | $(OBJDIR)
	@mkdir -p $(BUILDDIR)
	$(CC) $(T3READER_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built $(T3READER)"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILDDIR)
	@echo "Cleaned build directory"

# Install (copy to system PATH - optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/ternuino
	@echo "Installed to /usr/local/bin/ternuino"

# Run the program in interactive mode
run: $(TARGET)
	cd .. && ./c-version/$(TARGET)

# Run tests (if test programs exist)
test: $(TARGET)
	@echo "Running test programs..."
	@for prog in ../programs/*.asm; do \
		if [ -f "$$prog" ]; then \
			echo "Testing $$prog"; \
			cd .. && ./c-version/$(TARGET) "$$prog"; \
		fi \
	done

# Show help
help:
	@echo "Ternuino CPU Simulator - C Version"
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove build files"
	@echo "  run     - Run the program in interactive mode"
	@echo "  test    - Run all test programs"
	@echo "  t3reader- Build T3 file reader utility"
	@echo "  install - Install to system PATH"
	@echo "  help    - Show this help message"

# Build only the T3 reader utility
t3reader: $(T3READER)

.PHONY: all clean install run test help t3reader

# Dependencies (header files)
$(OBJDIR)/main.o: $(INCDIR)/ternuino.h $(INCDIR)/assembler.h $(INCDIR)/tritword.h $(INCDIR)/devices.h
$(OBJDIR)/ternuino.o: $(INCDIR)/ternuino.h $(INCDIR)/tritlogic.h $(INCDIR)/tritarith.h $(INCDIR)/ternio.h $(INCDIR)/devices.h
$(OBJDIR)/assembler.o: $(INCDIR)/assembler.h $(INCDIR)/ternuino.h
$(OBJDIR)/tritlogic.o: $(INCDIR)/tritlogic.h
$(OBJDIR)/tritarith.o: $(INCDIR)/tritarith.h
$(OBJDIR)/tritword.o: $(INCDIR)/tritword.h
$(OBJDIR)/ternio.o: $(INCDIR)/ternio.h
$(OBJDIR)/devices.o: $(INCDIR)/devices.h $(INCDIR)/ternuino.h $(INCDIR)/ternio.h
$(OBJDIR)/t3reader.o: $(INCDIR)/ternio.h
