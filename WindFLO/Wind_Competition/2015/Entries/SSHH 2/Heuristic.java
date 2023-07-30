

class Heuristic {
	int no_added_move_acceptances, no_added_next_heuristics, no_added_parameters, maximum_size_array;
	int[] sequenceBasedAcceptanceStrategySelection;
	int[] parameterSelection;
	int[] nextHeuristicSelection;
	Heuristic(int number_of_llhs) {
		no_added_move_acceptances = 2;
		no_added_parameters = 3;
		no_added_next_heuristics = number_of_llhs;
		maximum_size_array = 100000;
		sequenceBasedAcceptanceStrategySelection = new int[maximum_size_array];
		parameterSelection = new int[maximum_size_array];
		nextHeuristicSelection = new int[maximum_size_array];
		for (int i=0; i<2; ++i)
			sequenceBasedAcceptanceStrategySelection[i] = i;
		for (int i=0; i<3; ++i)
			parameterSelection[i] = i;
		for (int i=0; i<number_of_llhs; ++i)
			nextHeuristicSelection[i] = i;
	}
}