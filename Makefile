CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

TARGET = puzzle
OBJS = main.o PuzzleState.o Heuristic.o Solver.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp PuzzleState.h Heuristic.h Solver.h
	$(CXX) $(CXXFLAGS) -c main.cpp

PuzzleState.o: PuzzleState.cpp PuzzleState.h
	$(CXX) $(CXXFLAGS) -c PuzzleState.cpp

Heuristic.o: Heuristic.cpp Heuristic.h PuzzleState.h
	$(CXX) $(CXXFLAGS) -c Heuristic.cpp

Solver.o: Solver.cpp Solver.h PuzzleState.h
	$(CXX) $(CXXFLAGS) -c Solver.cpp

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: all clean
