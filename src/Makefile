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

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Target executable
TARGET = $(BUILDDIR)/ternuino

# Default target
all: $(TARGET)

# Create build directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Build the main executable
$(TARGET): $(OBJECTS) | $(OBJDIR)
	@mkdir -p $(BUILDDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built $(TARGET)"

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
	@echo "  install - Install to system PATH"
	@echo "  help    - Show this help message"

.PHONY: all clean install run test help

# Dependencies (header files)
$(OBJDIR)/main.o: $(INCDIR)/ternuino.h $(INCDIR)/assembler.h $(INCDIR)/tritword.h
$(OBJDIR)/ternuino.o: $(INCDIR)/ternuino.h $(INCDIR)/tritlogic.h $(INCDIR)/tritarith.h $(INCDIR)/ternio.h
$(OBJDIR)/assembler.o: $(INCDIR)/assembler.h $(INCDIR)/ternuino.h
$(OBJDIR)/tritlogic.o: $(INCDIR)/tritlogic.h
$(OBJDIR)/tritarith.o: $(INCDIR)/tritarith.h
$(OBJDIR)/tritword.o: $(INCDIR)/tritword.h
$(OBJDIR)/ternio.o: $(INCDIR)/ternio.h
