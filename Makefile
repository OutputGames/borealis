# Define compilers
CC = gcc
CXX = g++

# Define compiler flags
CFLAGS = -Wall  -std=c11 -Iext/glad/include -Iext/glfw/include/  # C specific flags
CXXFLAGS = -Wall -std=c++17  -Iext/glad/include -Iext/glfw/include/ # C++ specific flags
LDFLAGS = -Lext/glfw/lib-mingw-w64/ -lglfw3 -lopengl32 -lgdi32 # Linker flags, e.g., -lm for math library

# Define source directories and build directory
SRC_DIRS = src include # Add all your source subdirectories here
BUILD_DIR = build

# Find all C and C++ source files in subdirectories
C_SRCS = $(shell find $(SRC_DIRS) -name "*.c")
CPP_SRCS = $(shell find $(SRC_DIRS) -name "*.cpp")

# Generate object file paths in the build directory, maintaining directory structure
C_OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRCS))
CPP_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS))
OBJS = $(C_OBJS) $(CPP_OBJS)

# Define the executable name
TARGET = myapp.exe

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

# Rule for compiling C files
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling C++ files
$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)