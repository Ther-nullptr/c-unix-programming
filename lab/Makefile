# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lreadline

# Target executable
TARGET = yaush

# Source files
SRCS = yaush.c handle.c internal_command.c utils.c

# Header files
HEADERS = handle.h internal_command.h utils.h yaush.h header.h marcos.h jobs.h

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default rule to build the target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
