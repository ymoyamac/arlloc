# Compiler
CC = g++

# Compiler options
CFLAGS = -Wall -Wextra -std=c++23 -I src/include
OFLAGS = -o

# Directories
SRCDIR = src
SUBDIRS = \
    backend

# Executable
TARGET = main

DEPS = \
    $(SRCDIR)/backend/allocator/region.cpp \
    $(SRCDIR)/backend/arlloc.cpp \
	$(SRCDIR)/main.cpp

# Main rule
all: $(TARGET)

# Compilation
$(TARGET): $(DEPS)
	$(CC) $(CFLAGS) $^ $(OFLAGS) $@

# Clean
clean:
	rm -f $(TARGET)

run:
	./$(TARGET)