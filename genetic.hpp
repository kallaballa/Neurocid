#ifndef CGENALG_H
#define CGENALG_H

#include "util.hpp"
#include "tank.hpp"
#include "population.hpp"

using namespace std;

namespace tankwar {
//-----------------------------------------------------------------------
//
//	the genetic algorithm class
//-----------------------------------------------------------------------
class GeneticAlgorithm {
private:
	//probability that a chromosones bits will mutate.
	//Try figures around 0.05 to 0.3 ish
	double mutationRate_;
	//probability of chromosones crossing over bits
	//0.7 is pretty good
	double crossoverRate_;
	double maxPerturbation_;
	size_t numElite_;
	size_t numCopiesElite_;

	//total fitness of population
	double totalFitness_;
	//best fitness this population
	double bestFitness_;
	//average fitness
	double averageFitness_;
	//worst
	double worstFitness_;
	//keeps track of the best genome
	//generation counter
	size_t generation_;

	int fittestGenome_;


	void mutate(Brain& brain);
	Tank& getChromoRoulette(Population& pop);
	std::pair<Tank, Tank> crossover(Tank &mum, Tank &dad);
	void copyNBest(size_t nBest, const size_t numCopies, Population& in, Population& out);
	void calculateBestWorstAvTot(Population& pop);
	void reset();
public:
	GeneticAlgorithm(double mutRate, double crossRate, double maxPertubation, size_t numElite, size_t numCopiesElite);

	//this runs the GA for one generation.
	void epoch(Population& old_pop, Population& new_pop);

	size_t generation() const {
		return generation_;
	}

	double averageFitness() const {
		return averageFitness_;
	}

	double bestFitness() const {
		return bestFitness_;
	}
};
}

#endif

