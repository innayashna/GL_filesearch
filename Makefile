# Compiler
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -g

# Source files
SRC := main.c fileutils.c

# Object files
OBJ := $(SRC:.c=.o)

# Executable
TARGET := find

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Rules for individual object files
main.o: main.c fileutils.h
	$(CC) $(CFLAGS) -c main.c -o main.o

fileutils.o: fileutils.c fileutils.h
	$(CC) $(CFLAGS) -c fileutils.c -o fileutils.o

# Clean intermediate and executable files
clean:
	rm -f $(OBJ) $(TARGET)