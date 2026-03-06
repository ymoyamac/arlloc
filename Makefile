# Compiler
CC = g++

# Compiler options
CFLAGS = -Wall -Wextra -std=c++23 -I src/include
OFLAGS = -o

# Directories
SRCDIR = src

# Executable
TARGET = main

DEPS = \
	$(SRCDIR)/backend/allocator/block.cpp \
	$(SRCDIR)/backend/allocator/region.cpp \
	$(SRCDIR)/backend/arlloc.cpp \
	$(SRCDIR)/main.cpp

# Default: debug build
all: debug

# Debug build - enables Logger output
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Release build - disables Logger output
release: $(TARGET)

# Compilation
$(TARGET): $(DEPS)
	$(CC) $(CFLAGS) $^ $(OFLAGS) $@

# Clean
clean:
	rm -f $(TARGET)

run:
	./$(TARGET)