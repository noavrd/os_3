CXXFLAGS = -std=c++17 -Wall -Wextra -g

all: server

server: server.o deque_graph.o
	g++ $(CXXFLAGS) -o server server.o deque_graph.o -lpthread

server.o: server.cpp graph.hpp
	g++ $(CXXFLAGS) -c server.cpp

deque_graph.o: deque_graph.cpp graph.hpp
	g++ $(CXXFLAGS) -c deque_graph.cpp

clean:
	rm -f *.o server
