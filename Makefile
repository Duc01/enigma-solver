# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = enigma

# Phony targets to protect against files named 'all' or 'clean'
.PHONY: all clean

# Default target
all: $(TARGET)

# Rule to compile the executable
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

# Rule to clean up build files
clean:
	rm -f $(TARGET)
