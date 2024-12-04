CXXFLAGS = -std=c++17 -Wall -Wextra -g

all: server

server: reactor.o server.o deque_graph.o
		g++ $(CXXFLAGS) -o server reactor.o server.o deque_graph.o

reactor.o: reactor.cpp reactor.hpp
		g++ $(CXXFLAGS) -c reactor.cpp

server.o: server.cpp reactor.hpp
		g++ $(CXXFLAGS) -c server.cpp

deque_graph.o: deque_graph.cpp graph.hpp
		g++ $(CXXFLAGS) -c deque_graph.cpp

clean:
		rm -f *.o server
