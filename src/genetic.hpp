#ifndef CGENALG_H
#define CGENALG_H

#include "util.hpp"
#include "tank.hpp"
#include "population.hpp"

using namespace std;

namespace tankwar {

struct GeneticParams {
	double mutationRate;
	double crossoverRate;
	size_t crossoverIterations;
	double maxPertubation;
	size_t numElite_;
	size_t numEliteCopies_;
};


//-----------------------------------------------------------------------
//
//	the genetic algorithm class
//-----------------------------------------------------------------------
class GeneticPool {
private:
	GeneticParams params_;
	bool initialized_ = false;

	void mutate(Brain& brain);
	Tank& pickSpecimen(Population& pop);
	std::pair<Tank, Tank> crossover(Tank &mum, Tank &dad, size_t iterations);
	void copyNBest(size_t n, const size_t numCopies, Population& in, Population& out);
	void calculateStatistics(Population& pop);
public:
	GeneticPool(GeneticParams params);
	GeneticPool();

	//this runs the GA for one generation.
	virtual Population epoch(Population& old_pop);
};
}

#endif

