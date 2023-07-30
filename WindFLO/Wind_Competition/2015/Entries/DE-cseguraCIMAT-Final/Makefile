CC=g++ 
CFLAGS =-Wall -std=c++0x -g 
OPTFLAGS=-O2
INCLUDES=-I$(PWD)

SOURCES=tinyxml2.cpp WindScenario.cpp WindFarmLayoutEvaluator.cpp KusiakLayoutEvaluator.cpp SolutionSimpleModel.cpp TurbineEstimator.cpp SolutionInitializer.cpp FitnessCalculation.cpp LocalSearch.cpp utils.cpp jsoncpp.cpp
OBJECTS=$(SOURCES:.cpp=.o)

DE:$(OBJECTS) DE.cpp 
	$(CC) $(CFLAGS) -o DE $(OBJECTS) DE.cpp $(INCLUDES) $(OPTFLAGS) -lcurl

localSearchWithEstimator:$(OBJECTS) localSearchWithEstimator.cpp 
	$(CC) $(CFLAGS) -o localSearchWithEstimator $(OBJECTS) localSearchWithEstimator.cpp $(INCLUDES) $(OPTFLAGS)

testSolution:$(OBJECTS) testSolution.cpp 
	$(CC) $(CFLAGS) -o testSolution $(OBJECTS) testSolution.cpp $(INCLUDES) $(OPTFLAGS)

localSearch:$(OBJECTS) localSearch.cpp
	$(CC) $(CFLAGS) -o localSearch $(OBJECTS) localSearch.cpp $(INCLUDES) $(OPTFLAGS)

wellDistributed:$(OBJECTS) localSearch.cpp
	$(CC) $(CFLAGS) -o wellDistributed $(OBJECTS) wellDistributed.cpp $(INCLUDES) $(OPTFLAGS)

solver:$(OBJECTS) solver.cpp
	$(CC) $(CFLAGS) -o solver $(OBJECTS) solver.cpp $(INCLUDES) $(OPTFLAGS)

test:$(OBJECTS) test.cpp
	$(CC) $(CFLAGS) -o test $(OBJECTS) test.cpp $(INCLUDES) $(OPTFLAGS)

testAll:$(OBJECTS) testAll.cpp
	$(CC) $(CFLAGS) -o testAll $(OBJECTS) testAll.cpp $(INCLUDES) $(OPTFLAGS)

main:$(OBJECTS)
	$(CC) $(CFLAGS) -o eval.o $(OBJECTS) main.cpp $(INCLUDES) $(OPTFLAGS)

evaluate:$(OBJECTS)
	$(CC) $(CFLAGS) -o evaluate.o $(OBJECTS) evaluate.cpp $(INCLUDES) $(OPTFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@ -w $(INCLUDES) $(OPTFLAGS)

clean:
	rm $(OBJECTS) eval.o DE 2> /dev/null
