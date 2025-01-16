# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Werror -std=c++11

# Targets
TARGET := sighting_search
DATASET := create_dataset

# Source files
SRCS := sighting_search.cc
DATASET_SRC := create_dataset.cc

# Default rule
all: $(TARGET) $(DATASET)

# Compile sighting_search
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile create_dataset
$(DATASET): $(DATASET_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Lint rule
lint:
	clang-tidy $(SRCS) $(DATASET_SRC) -- -std=c++11

# Clean rule
clean:
	rm -f $(TARGET) $(DATASET) sightings_*.dat signatures_*.dat

.PHONY: all clean lint
