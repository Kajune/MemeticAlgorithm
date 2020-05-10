#pragma once
#include <concepts>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>
#include <random>
#include <numeric>

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
using SelectionAlgorithm = std::function<size_t(const std::vector<T>&)>;

template <typename FitnessType>
struct GA_Params {
	size_t numNextGenes;
	double crossoverRate;
	double mutationRate;
	SelectionAlgorithm<FitnessType> sa;
};

template <GeneConcept Gene, typename FitnessType>
class GeneticAlgorithm {
public:
	void initialize(size_t numGenes) {
		for (size_t i = 0; i < numGenes; i++) {
			m_genes.emplace_back(std::make_shared<Gene>());
		}
		evaluateAllGenes();
	}

	FitnessType nextGeneration(const GA_Params<FitnessType>& params) {
		std::vector<std::shared_ptr<Gene>> next_genes;
	
		#pragma omp parallel for
		for (int i = 0; i < params.numNextGenes; i++) {

		}

		m_genes = next_genes;
		evaluateAllGenes();
		return getAverageFitness();
	}

	std::shared_ptr<Gene> getBestGene() const {
		if (m_fitness.size() != m_genes.size()) {
			evaluateAllGenes();
		}

		auto it = std::max_element(m_fitness.begin(), m_fitness.end());
		size_t index = std::distance(m_fitness.begin(), it);
		return m_genes.at(index);
	}

	FitnessType getAverageFitness() const {
		return std::accumulate(m_fitness.begin(), m_fitness.end(), FitnessType(0)) / m_fitness.size();
	}

private:
	std::vector<std::shared_ptr<Gene>> m_genes;
	std::vector<FitnessType> m_fitness;

	void evaluateAllGenes() {
		m_fitness.clear();
		for (const auto& gene : m_genes) {
			m_fitness.emplace_back(gene->evaluate());
		}
	}
};
