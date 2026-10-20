CXX = g++
CXXFLAGS = -std=c++20

.PHONY: all test clean

all: simple

simple: compiler.cpp
	$(CXX) $(CXXFLAGS) -o simple compiler.cpp

test:
	./test.sh

clean:
	rm -f simple generated.asm
