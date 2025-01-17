# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Werror -std=c++11

# Target
TARGET := sighting_search

# Source file
SRCS := sighting_search.cc

# Default rule
all: $(TARGET)

# Compile sighting_search
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule
clean:
	rm -f $(TARGET)

.PHONY: all clean
