#ifndef CGENALG_H
#define CGENALG_H

#include "util.hpp"
#include "ship.hpp"
#include "bsp.hpp"
#include "population.hpp"

using namespace std;

namespace neurocid {

struct GeneticLayout {
	double mutationRate;
	double crossoverRate;
	size_t crossoverIterations;
	double maxPertubation;
	size_t numElite_;
	size_t numEliteCopies_;
	bool usePerfDesc_;
};

inline GeneticLayout make_default_genetic_layout() {
	return {
			0.1, // mutationRate
			0.7, // crossoverRate
			1,   // crossoverIterations
			0.3, // maxPertubation
			4,   // numElite
			1,   // numEliteCopies
			false// usePerfDesc_
	};
}

//-----------------------------------------------------------------------
//
//	the genetic algorithm class
//-----------------------------------------------------------------------
class GeneticPool {
private:
	bool initialized_ = false;
	void mutate(Brain& brain);
	Ship& pickSpecimen(Population& pop);
	std::pair<Ship, Ship> crossover(Ship &mum, Ship &dad, size_t iterations);
	void copyNBest(size_t n, const size_t numCopies, Population& in, Population& out);
	void calculateStatistics(Population& pop);
public:
	GeneticLayout params_;
	GeneticPool(GeneticLayout params);
	GeneticPool();

	//this runs the GA for one generation.
	virtual Population epoch(Population& old_pop);
};
}

#endif

