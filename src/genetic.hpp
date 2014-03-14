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

struct Statistics {
	Statistics() {
		reset();
	}

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
	double totalFriendlyFire_;
	double totalHits_;
	double totalDamage_;
	double totalAmmonition_;
	double averageFriendlyFire_;
	double averageHits_;
	double averageDamage_;
	double averageAmmonition_;

	size_t generationCnt_;
	double score_;

	size_t fittestGenome_ ;

	void reset() {
		totalFitness_ = 0;
		bestFitness_ = 0;
		worstFitness_ = std::numeric_limits<double>().max();
		averageFitness_ = 0;
		averageFriendlyFire_ = 0;
		averageDamage_ = 0;
		averageAmmonition_ = 0;
		totalFitness_ = 0;
		totalFriendlyFire_ = 0;
		totalHits_ = 0;
		totalDamage_ = 0;
		totalAmmonition_ = 0;
		score_ = 0;
	}

	void print(std::ostream& os) {
		os << generationCnt_ << ":"
		<< bestFitness_ << ":"
		<< averageFitness_ << ":"
		<< averageHits_ << ":"
		<< averageFriendlyFire_ / 3 << ":"
		<< averageHits_ << ":"
		<< averageDamage_ << ":"
		<< (averageAmmonition_ / 3) << ":"
		<< (score_);
	}
};

//-----------------------------------------------------------------------
//
//	the genetic algorithm class
//-----------------------------------------------------------------------
class GeneticPool {
private:
	GeneticParams params_;
	Statistics stats_;
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

	Statistics& statistics() {
		return stats_;
	}
};
}

#endif

