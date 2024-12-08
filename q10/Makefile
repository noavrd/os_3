CXXFLAGS = -std=c++17 -Wall -Wextra -g

all: server

server: server.o reactor.o deque_graph.o
	g++ $(CXXFLAGS) -o server server.o reactor.o deque_graph.o -lpthread

server.o: server.cpp reactor.hpp deque_graph.cpp graph.hpp
	g++ $(CXXFLAGS) -c server.cpp

reactor.o: reactor.cpp reactor.hpp
	g++ $(CXXFLAGS) -c reactor.cpp

deque_graph.o: deque_graph.cpp graph.hpp
	g++ $(CXXFLAGS) -c deque_graph.cpp

clean:
	rm -f *.o server
