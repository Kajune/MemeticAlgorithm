#include <iostream>
#include <random>
#include <fstream>

#include "genetic.hpp"

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(-1, 1);

struct Gene {
	Gene() {
	}

	double evaluate() const {
		return 0;
	}

	Gene crossover(const Gene& rhs) const {
		return *this;
	}

	Gene mutation() const {
		return *this;
	}
};

struct SampleGene {
	double x, y;

	SampleGene() : x(dist(mt)), y(dist(mt)) {}

	double evaluate(double a=-1.0, double b=10, double c=20) const {
		return b * exp(sqrt((x * x + y * y) / 2) * a) + exp(cos(x * c) + cos(y * c) / 2);
	}

	SampleGene crossover(const SampleGene& rhs) const {
		double x_op = dist(mt);
		double y_op = dist(mt);

		SampleGene ret;
		ret.x = x_op >= 0 ? x : rhs.x;
		ret.y = y_op >= 0 ? y : rhs.y;
		return ret;
	}

	SampleGene mutation() const {
		double x_op = dist(mt);
		double y_op = dist(mt);

		SampleGene ret;
		ret.x = std::max(std::min(x + x_op * 0.2, 1.0), -1.0);
		ret.y = std::max(std::min(y + y_op * 0.2, 1.0), -1.0);
		return ret;
	}
};

int main() {
	GeneticAlgorithm<SampleGene, double> ga;
	GA_Params<double> params;
	params.numNextGenes = 128;

	ga.initialize(128);

	std::ofstream ofs("result.txt");

	for (int g = 0; g < 1000; g++) {
		auto aveFitness = ga.nextGeneration(params);
		std::cout << g << "-th Generation, Average fitness: " << aveFitness << std::endl;

		for (const auto& gene : ga.getAllGenes()) {
			ofs << gene.x << " " << gene.y << " ";
		}
		ofs << std::endl;
	}

	const auto& best = ga.getBestGene();
	std::cout << "Best: " << best.x << ", " << best.y << std::endl;
}
