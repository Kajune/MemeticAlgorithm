#pragma once
#include <concepts>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>
#include <random>
#include <numeric>
#include <cassert>

namespace impl {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0, 1.0);
}

template <class T>
concept GeneConcept = requires (const T& x, const T& y) {
	{ x.evaluate() < y.evaluate() }->std::convertible_to<bool>;
	{ x.evaluate() > y.evaluate() }->std::convertible_to<bool>;
	{ x.evaluate() <= y.evaluate() }->std::convertible_to<bool>;
	{ x.evaluate() >= y.evaluate() }->std::convertible_to<bool>;
	{ x.evaluate() == y.evaluate() }->std::convertible_to<bool>;
	{ x.evaluate() != y.evaluate() }->std::convertible_to<bool>;

	{ x.crossover(y) }->std::convertible_to<T>;

	{ x.mutation() }->std::convertible_to<T>;
};

template <typename T>
struct RouletteSelection {
	size_t operator()(const std::vector<T>& fitness) {
		auto sum = std::accumulate(fitness.begin(), fitness.end(), T(0));
		std::uniform_real_distribution<T> dist(T(0), sum);
		auto op = dist(impl::mt);
		T currentSum(0);
		for (size_t i = 0; i < fitness.size(); i++) {
			assert(fitness.at(i) >= 0);
			currentSum += fitness.at(i);
			if (currentSum >= op) {
				return i;
			}
		}
		return fitness.size() - 1;
	}
};

template <typename T>
using SelectionAlgorithm = std::function<size_t(const std::vector<T>&)>;

template <typename FitnessType>
struct GA_Params {
	size_t numNextGenes;
	double crossoverRate = 0.8;
	double mutationRate = 0.02;
	SelectionAlgorithm<FitnessType> sa = RouletteSelection<FitnessType>();
};

template <GeneConcept Gene, typename FitnessType>
class GeneticAlgorithm {
public:
	void initialize(size_t numGenes) {
		for (size_t i = 0; i < numGenes; i++) {
			m_genes.emplace_back(Gene());
		}
		evaluateAllGenes();
	}

	FitnessType nextGeneration(const GA_Params<FitnessType>& params) {
		assert(params.crossoverRate + params.mutationRate <= 1);
		std::vector<Gene> next_genes;
	
		#pragma omp parallel for
		for (int i = 0; i < params.numNextGenes; i++) {
			double op = impl::dist(impl::mt);
			if (op <= params.crossoverRate) {
				const auto& first = m_genes.at(params.sa(m_fitness));
				const auto& second = m_genes.at(params.sa(m_fitness));
				next_genes.emplace_back(first.crossover(second));
			} else if (op <= params.crossoverRate + params.mutationRate) {
				next_genes.emplace_back(m_genes.at(params.sa(m_fitness)).mutation());
			} else {
				next_genes.emplace_back(m_genes.at(params.sa(m_fitness)));
			}
		}

		m_genes = next_genes;
		evaluateAllGenes();
		return getAverageFitness();
	}

	const Gene& getBestGene() const {
		if (m_fitness.size() != m_genes.size()) {
			evaluateAllGenes();
		}

		auto it = std::max_element(m_fitness.begin(), m_fitness.end());
		size_t index = std::distance(m_fitness.begin(), it);
		return m_genes.at(index);
	}

	const std::vector<Gene>& getAllGenes() const {
		return m_genes;
	}

	FitnessType getAverageFitness() const {
		if (m_fitness.size() != m_genes.size()) {
			evaluateAllGenes();
		}

		return std::accumulate(m_fitness.begin(), m_fitness.end(), FitnessType(0)) / m_fitness.size();
	}

private:
	std::vector<Gene> m_genes;
	mutable std::vector<FitnessType> m_fitness;

	void evaluateAllGenes() const {
		m_fitness.clear();
		for (const auto& gene : m_genes) {
			m_fitness.emplace_back(gene.evaluate());
		}
	}
};
