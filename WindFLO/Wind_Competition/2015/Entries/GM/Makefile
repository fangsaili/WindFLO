CC=g++
CFLAGS =-Wall -std=c++0x -g
OPTFLAGS=-O2
INCLUDES=-I$(PWD) -lcurl

SOURCES=tinyxml2.cpp WindScenario.cpp WindFarmLayoutEvaluator.cpp KusiakLayoutEvaluator.cpp
OBJECTS=$(SOURCES:.cpp=.o)

main:$(OBJECTS)
	$(CC) $(CFLAGS) -o competition.o $(OBJECTS) GA.cpp main.cpp $(INCLUDES) $(OPTFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@ -w $(INCLUDES) $(OPTFLAGS)

clean:
	rm $(OBJECTS) $(COMP_OBJECTS) competition.o evaluate.o 2> /dev/null
