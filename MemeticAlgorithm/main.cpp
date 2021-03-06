#include <iostream>
#include <random>
#include <fstream>
#include <numeric>

#include "memetic.hpp"

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(-1, 1);

double ackley(double x[3]) {
	double a = -1.0, b = 10, c = 20;
	return b * exp(sqrt((x[0] * x[0] + x[1] * x[1]) / 2) * a) + exp(cos(x[0] * c) + cos(x[1] * c) / 2);
}

struct SampleGene {
	double x, y;
	static double mutationStep;

	SampleGene() : x(dist(mt)), y(dist(mt)) {}

	double evaluate() const {
		double p[] = { x, y, 0 };
		return ackley(p);
	}

	SampleGene crossover(const SampleGene& rhs) const {
		double x_op = (dist(mt) + 1.0) / 2.0;
		double y_op = (dist(mt) + 1.0) / 2.0;

		SampleGene ret;
		ret.x = x_op * x + (1 - x_op) * rhs.x;
		ret.y = y_op * y + (1 - y_op) * rhs.y;
		return ret;
	}

	SampleGene mutation() const {
		double x_op = dist(mt);
		double y_op = dist(mt);

		SampleGene ret;
		ret.x = std::max(std::min(x + x_op * mutationStep, 1.0), -1.0);
		ret.y = std::max(std::min(y + y_op * mutationStep, 1.0), -1.0);
		return ret;
	}

	double optimize() {
		double p[3] = { x, y, 0 };
		GradientDescent<double, 3> solver(ackley, false);
		double finalCost = solver.optimize(p);
		x = p[0];
		y = p[1];
		return finalCost;
	}
};

double SampleGene::mutationStep = 0.2;

int main() {
	MemeticAlgorithm<SampleGene, double> ma;
	GA_Params<double> params;
	params.numNextGenes = 32;

	ma.initialize(32);

	std::ofstream ofs("result.txt");

	for (int g = 0; g < 100; g++) {
		auto aveFitness = ma.nextGeneration(params);
		aveFitness = ma.optimize();
		std::cout << g << "-th Generation, Average fitness: " << aveFitness << std::endl;

		double ave = 0.0, var = 0.0;
		for (const auto& gene : ma.getAllGenes()) {
			ofs << gene.x << " " << gene.y << " ";
			ave += (gene.x + gene.y) / 2;
			var += pow((gene.x + gene.y) / 2, 2);
		}
		ofs << std::endl;
		ave /= ma.getAllGenes().size();
		var = var / ma.getAllGenes().size() - ave * ave;

//		SampleGene::mutationStep = sqrt(std::max(var, 0.0)) * 10;
	}

	const auto& best = ma.getBestGene();
	std::cout << "Best: " << best.x << ", " << best.y << std::endl;
}
