CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wnonportable-include-path

TARGET = main

SRCS = main.cpp Graph.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
