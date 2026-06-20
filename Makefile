# Variables
CC = clang
CFLAGS = -Wall -Wextra -g -O0
TARGET = enigma

# Phony targets to protect against files named 'all' or 'clean'
.PHONY: all clean

# Default target
all: $(TARGET)

# Rule to compile the executable
$(TARGET): main.c plugboard.c rotors.c encode.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c plugboard.c rotors.c encode.c

# Rule to clean up build files
clean:
	rm -f $(TARGET)
