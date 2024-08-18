# Define the compiler
CC = gcc

# Define the directories
SRC_DIR = .
LIB_DIR = ./lib

# Define the sources
SOURCES = $(SRC_DIR)/main.c $(LIB_DIR)/lib.c

# Define the object files
OBJECTS = $(SOURCES:.c=.o)

# Define the executable name
TARGET = calc

# Define the SDL2 flags
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --libs)

# Define the SDL2_ttf flags
SDL2_TTF_CFLAGS = -I/usr/include/SDL2
SDL2_TTF_LIBS = -lSDL2_ttf

# Define the flags
CFLAGS = -Wall -Wextra -I$(LIB_DIR) $(SDL2_CFLAGS) $(SDL2_TTF_CFLAGS)
LDFLAGS = $(SDL2_LIBS) $(SDL2_TTF_LIBS)

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Build the object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
