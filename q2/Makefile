# Makefile for Q2

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: deque_graph list_graph

deque_graph: main.cpp deque_graph.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -DUSE_DEQUE -o deque_graph main.cpp

list_graph: main.cpp list_graph.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -DUSE_LIST -o list_graph main.cpp

clean:
	rm -f deque_graph list_graph
