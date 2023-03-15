CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -Wno-sign-compare

SOURCES = main.cpp tree.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = tree_executable

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)