#include <iostream>

#include "genetic.hpp"

struct Gene {
	double evaluate() const {
		return 0;
	};

	Gene crossover(const Gene& rhs) const {
	};

	Gene mutation() const {
	};
};

int main() {
	GeneticAlgorithm<Gene, double> ga;
	GA_Params<double> params;

	ga.initialize(10);
	ga.nextGeneration(params);
}
