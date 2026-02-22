# Compiler
CC = gcc

# Compiler options
CFLAGS = -Wall -Wextra -std=c17 -I src/include
OFLAGS = -o

# Directories
SRCDIR = src
SUBDIRS = \
    backend

# Executable
TARGET = main

DEPS = \
    $(SRCDIR)/backend/dll.c \
	$(SRCDIR)/main.c

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