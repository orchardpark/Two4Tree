CXX = g++
CXXFLAGS = -Wall -g

binaries=main Two4Tree

all: clean $(binaries)

.PHONY : clean
clean :
	rm -f $(binaries) *.o

main: main.o Two4Tree.o
	$(CXX) -o main main.o Two4Tree.o

Two4Tree.o: Two4Tree.cpp
	$(CXX) $(CXXFLAGS) -c Two4Tree.cpp

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp
