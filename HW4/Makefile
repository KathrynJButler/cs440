CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

all: clearScreen main.out test 

main.out: main.cpp classes.h
	$(CXX) $(CXXFLAGS) main.cpp -o main.out

test: main.out
	./main.out id1 id2

clean:
	rm -f main.out
	rm -f EmployeeIndex
	
clearScreen:
	clear
	clear

