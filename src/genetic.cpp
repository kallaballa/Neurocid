#include "genetic.hpp"
#include "fann.h"
#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <assert.h>

namespace neurocid {

GeneticPool::GeneticPool(GeneticLayout params) :
		params_(params) {
	initialized_ = true;
}

GeneticPool::GeneticPool() :
		params_() {
	initialized_ = false;
}

/*
 * mutates a chromosome by perturbing its weights by an amount not greater than params_.maxPerturbation_
 */
void GeneticPool::mutate(Brain& brain) {
	//traverse the chromosome and mutate each weight dependent
	//on the mutation rate
	fann_type* weights = brain.weights();
	for (size_t i = 0; i < brain.size(); ++i) {
		//do we perturb this weight?
		if (fRand(0, 1) < params_.mutationRate) {
			//add or subtract a small value to the weight
			weights[i] += ((fRand(0, 1) - fRand(0, 1)) * params_.maxPertubation);
		}
	}
}

/*
 * Returns a tank base on roulette wheel sampling
 */
Ship& GeneticPool::pickSpecimen(Population& pop) {
	//generate a random number between 0 & total fitness count
	double slice = (double) (fRand(0, 1) * pop.stats_.totalFitness_);

	//this will be set to the chosen tank go through the tanks adding up the fitness so far
	double fitnessSoFar = 0;

	for (size_t i = 0; i < pop.size(); ++i) {
		fitnessSoFar += pop[i].fitness_;

		//if the fitness so far > random number return the tank at this point
		if (fitnessSoFar >= slice) {
			return pop[i];
		}
	}

	assert(false);
	return pop[0]; 	//surpress warning
}

/*
 * With a chance defined by params_.crossoverRate_ perform a crossover of brain_.weights()
 */
std::pair<Ship, Ship> GeneticPool::crossover(Ship &mum, Ship &dad, size_t iterations) {
	Ship baby1 = mum.makeChild();
	Ship baby2 = mum.makeChild();

	fann_type* wMum = mum.brain_->weights();
	fann_type* wDad = dad.brain_->weights();
	fann_type* wBaby1 = baby1.brain_->weights();
	fann_type* wBaby2 = baby2.brain_->weights();

	//just return parents as offspring dependent on the rate or if parents are the same
	if ((fRand(0,1) > params_.crossoverRate) || (mum == dad)) {
		baby1.brain_->destroy();
		baby2.brain_->destroy();
		return {mum.clone(), dad.clone()};
	}

	size_t last_cp = 0;
	size_t cp = 0;
	bool cross = false;

	for(size_t i = 0; i < iterations && cp < (mum.brain_->size() - 1); ++i) {
		//determine a crossover point
		cp = iRand(last_cp, mum.brain_->size() - 1);

		//create the offspring
		for (size_t j = last_cp; j < cp; ++j) {
			if(cross) {
				wBaby1[j] = wMum[j];
				wBaby2[j] = wDad[j];
			} else {
				wBaby1[j] = wDad[j];
				wBaby2[j] = wMum[j];
			}
		}

		last_cp = cp;
		cross = !cross;
	}

	for (size_t i = last_cp; i < mum.brain_->size(); ++i) {
		if(cross) {
			wBaby1[i] = wMum[i];
			wBaby2[i] = wDad[i];
		} else {
			wBaby1[i] = wDad[i];
			wBaby2[i] = wMum[i];
		}
	}

	return {baby1, baby2};
}


/*
 * copy numCopies copies of the n best specimen into the out population
 */
void GeneticPool::copyNBest(size_t n, const size_t numCopies,
		Population& in, Population& out) {
	//add the required amount of copies of the n most fittest to the supplied population
	while (n--) {
		for (size_t i = 0; i < numCopies; ++i) {
			Ship& t = in[(in.size() - 1) - n];
			Ship clone = t.clone();
			clone.isElite = true;
			out.push_back(clone);
		}
	}
}

/*
 * generates the statistics for the given population
 */
void GeneticPool::calculateStatistics(Population& pop) {

	size_t size = pop.size();
	double highestSoFar = 0;
	double lowestSoFar = 9999999;

	pop.stats_.bestFitness_ = std::numeric_limits<double>().max();

	for (size_t i = 0; i < size; ++i) {
		pop.stats_.totalFriendlyFire_ += pop[i].friendlyFire_;
		pop.stats_.totalCrash_ += pop[i].crash_;
		pop.stats_.totalHits_  += pop[i].hits_;
		pop.stats_.totalDamage_  += pop[i].damage_;
		pop.stats_.totalAmmonition_ += pop[i].ammonition_;

		//update fittest if necessary
		if (highestSoFar == 0 || pop[i].fitness_ > highestSoFar) {
			highestSoFar = pop[i].fitness_;
			pop.stats_.fittestGenome_ = i;
			pop.stats_.bestFitness_ = highestSoFar;
		}

		//update worst if necessary
		if (pop[i].fitness_ < lowestSoFar) {
			lowestSoFar = pop[i].fitness_;
			pop.stats_.worstFitness_ = lowestSoFar;
		}

		pop.stats_.totalFitness_ += pop[i].fitness_;
	}

	assert(pop.stats_.bestFitness_ != std::numeric_limits<double>().max());

	pop.stats_.score_ = pop.stats_.totalHits_ - pop.stats_.totalDamage_;
	pop.stats_.averageFitness_ = pop.stats_.totalFitness_ / size;
	pop.stats_.averageFriendlyFire_ = pop.stats_.totalFriendlyFire_ / size;
	pop.stats_.averageCrash_ = pop.stats_.totalCrash_ / size;
	pop.stats_.averageHits_ = pop.stats_.totalHits_ / size;
	pop.stats_.averageDamage_ = pop.stats_.totalDamage_ / size;
	pop.stats_.averageAmmonition_ = pop.stats_.totalAmmonition_ / size;
}

/*
 * Use the genetic algorithm to construct a new population from the old
 */
Population GeneticPool::epoch(Population& old_pop) {
	if(!initialized_ || old_pop.size() == 1) {
		Population new_pop = old_pop;
		new_pop.clear();
		old_pop.stats_.reset();

		for(Ship& t : old_pop) {
			t.calculateFitness();
			t.isElite = false;
		}

		//sort the population (for scaling and elitism)
		sort(old_pop.begin(), old_pop.end());

		//calculate best, worst, average and total fitness
		calculateStatistics(old_pop);

		for(Ship& t : old_pop) {
			new_pop.push_back(t.makeChild());
		}
		new_pop.stats_ = old_pop.stats_;
		return new_pop;
	}

	//FIXME preallocate
	Population new_pop = old_pop;
	new_pop.clear();
	old_pop.stats_.reset();

	for(Ship& t : old_pop) {
		t.calculateFitness();
		t.isElite = false;
	}

	//sort the population (for scaling and elitism)
	sort(old_pop.begin(), old_pop.end());

	//calculate best, worst, average and total fitness
	calculateStatistics(old_pop);

	//Now to add a little elitism we shall add in some copies of the
	//fittest genomes. Make sure we add an EVEN number or the roulette
	//wheel sampling will crash
	if(params_.numElite_ <= old_pop.size()){
		if (!(params_.numEliteCopies_ * (params_.numElite_ % 2))) {
			copyNBest(params_.numElite_, params_.numEliteCopies_, old_pop, new_pop);
		}
	}

	assert(old_pop.layout_.tl_.num_perf_desc == 4);
	PerfDescBsp<4,Ship> pdb;
	if(params_.usePerfDesc_) {
		for(Ship& t : old_pop) {
			pdb.insert(&t);
		}
	}
	//now we enter the GA loop
	//repeat until a new population is generated
	while (new_pop.size() < old_pop.size()) {
		//grab two chromosones

		Ship& mum = pickSpecimen(old_pop);
		Ship* dad;
		if(params_.usePerfDesc_) {
			dad = &pdb.findClosestMate(mum);
		} else
			dad = &pickSpecimen(old_pop);

		//create some offspring via crossover
		std::pair<Ship, Ship> babies = crossover(mum, *dad, params_.crossoverIterations);

		//now we mutate
		mutate(*babies.first.brain_);
		mutate(*babies.second.brain_);

		//now copy into vecNewPop population
		new_pop.push_back(babies.first);
		new_pop.push_back(babies.second);
	}
	assert(new_pop.size() == old_pop.size());
	old_pop.stats_.generationCnt_++;
	new_pop.stats_ = old_pop.stats_;
	return new_pop;
}
}
