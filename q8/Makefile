CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

all: reactor_test

reactor_test: reactor.cpp reactor_test.cpp
	$(CXX) $(CXXFLAGS) reactor.cpp reactor_test.cpp -o reactor_test

clean:
	rm -f reactor_test
