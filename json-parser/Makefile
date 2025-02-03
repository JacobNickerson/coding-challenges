# Project name
PROJECT_NAME = json-parser

# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include  

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Output binary
TARGET = $(BIN_DIR)/$(PROJECT_NAME)

# Default target
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)  # Create bin directory if it doesn't exist
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)  # Create build directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clang-Tidy: Run clang-tidy for static analysis
clang-tidy: $(SRCS)
	@for file in $(SRCS); do \
		echo "Running clang-tidy on $$file..."; \
		clang-tidy $$file -- -I$(INCLUDE_DIR); \
	done

# Clang-Format: Automatically format the code
clang-format:
	@echo "Running clang-format on source files..."
	@clang-format -i $(SRCS)

# Clean up generated files
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean distclean clang-tidy clang-format cppcheck

