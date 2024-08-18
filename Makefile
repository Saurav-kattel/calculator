# Define the compiler
CC = gcc

# Define the directories
SRC_DIR = .
LIB_DIR = ./lib
BUILD_DIR = ./build

# Define the sources
SOURCES = $(SRC_DIR)/main.c $(LIB_DIR)/lib.c

# Define the object files
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

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
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)  # Create the directory if it doesn't exist
	$(CC) $(CFLAGS) -c -o $@ $<

# Install target
install: $(TARGET)
	sudo install -m 755 $(TARGET) /usr/local/bin/

# Clean up
clean:
	rm -rf $(TARGET) $(BUILD_DIR)

# Uninstall target
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Phony targets
.PHONY: all clean install uninstall
