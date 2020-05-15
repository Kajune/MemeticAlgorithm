#pragma once

#include "genetic.hpp"
#include "optimization.hpp"

template <class T>
concept MemeConcept = requires (T& x) {
	requires GeneConcept<T>;
	x.optimize();
};

template <MemeConcept Meme, typename FitnessType>
class MemeticAlgorithm : public GeneticAlgorithm<Meme, FitnessType> {
public:
	FitnessType optimize() {
		for (auto& gene : this->m_genes) {
			gene.optimize();
		}

		this->evaluateAllGenes();
		return this->getAverageFitness();
	}
};
