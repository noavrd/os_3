CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: server

server: server.cpp deque_graph.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

clean:
	rm -f server
