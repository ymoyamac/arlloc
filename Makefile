# Compiler
CC = gcc

# Compiler options
CFLAGS = -Wall -Wextra -std=c++20 -I src/include
OFLAGS = -o

# Directories
SRCDIR = src
SUBDIRS = \
    backend

# Executable
TARGET = main.o

# Dependencies and objects
_DEPS = \
    arlloc.c

DEPS = \
    $(SRCDIR)/backend/dll.c \
    $(SRCDIR)/backend/arlloc.c \
	$(SRCDIR)/main.c

INCLUDE = \
	$(SRCDIR)/include

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