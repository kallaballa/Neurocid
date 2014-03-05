#include "genetic.hpp"
#include "fann.h"
#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <assert.h>

namespace tankwar {
//-----------------------------------constructor-------------------------
//
//	sets up the population with random floats
//
//-----------------------------------------------------------------------
GeneticAlgorithm::GeneticAlgorithm(double mutRate, double crossRate, double maxPerturbation, size_t numElite, size_t numCopiesElite) :
		mutationRate_(mutRate),
		crossoverRate_(crossRate),
		maxPerturbation_(maxPerturbation),
		numElite_(numElite),
		numCopiesElite_(numCopiesElite),
		totalFitness_(0),
		bestFitness_(0),
		averageFitness_(0),
		worstFitness_(99999999),
		generation_(0),
		fittestGenome_(0)
{
}

//---------------------------------Mutate--------------------------------
//
//	mutates a chromosome by perturbing its weights by an amount not 
//	greater than CParams::dMaxPerturbation
//-----------------------------------------------------------------------
void GeneticAlgorithm::mutate(Brain& brain) {
	//traverse the chromosome and mutate each weight dependent
	//on the mutation rate
	fann_type* weights = brain.weights();
	for (size_t i = 0; i < brain.size(); ++i) {
		//do we perturb this weight?
		if (fRand(0, 1) < mutationRate_) {
			//add or subtract a small value to the weight
			weights[i] += ((fRand(0, 1) - fRand(0, 1)) * maxPerturbation_);
		}
	}
}

//----------------------------------GetChromoRoulette()------------------
//
//	returns a chromo based on roulette wheel sampling
//
//-----------------------------------------------------------------------
Tank& GeneticAlgorithm::getChromoRoulette(Population& pop) {
	//generate a random number between 0 & total fitness count
	double slice = (double) (fRand(0, 1) * totalFitness_);

	//this will be set to the chosen chromosome

	//go through the chromosones adding up the fitness so far
	double fitnessSoFar = 0;

	for (size_t i = 0; i < pop.size(); ++i) {
		fitnessSoFar += pop[i].fitness_;

		//if the fitness so far > random number return the chromo at 
		//this point
		if (fitnessSoFar >= slice) {
			return pop[i];
		}
	}

	assert(false);
	//surpress warning
	return pop[0];
}

//-------------------------------------Crossover()-----------------------
//	
//  given parents and storage for the offspring this method performs
//	crossover according to the GAs crossover rate
//-----------------------------------------------------------------------
std::pair<Tank, Tank> GeneticAlgorithm::crossover(Tank &mum,
		Tank &dad) {
	Tank baby1 = mum.makeChild();
	Tank baby2 = mum.makeChild();

	fann_type* wMum = mum.brain_.weights();
	fann_type* wDad = dad.brain_.weights();
	fann_type* wBaby1 = baby1.brain_.weights();
	fann_type* wBaby2 = baby2.brain_.weights();

	//just return parents as offspring dependent on the rate
	//or if parents are the same
	if ((fRand(0,1) > crossoverRate_) || (mum == dad)) {
		return {mum.clone(), dad.clone()};
	}
	/*
	size_t it = Params::CROSSOVER_ITERATIONS;
	size_t last_cp = 0;
	size_t cp;
	bool cross = false;

	for(size_t i = 0; i < it && cp < (mum.brain_.size() - 1); ++i) {
		//determine a crossover point
		cp = iRand(last_cp, mum.brain_.size() - 1);

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

	for (size_t i = last_cp; i < mum.brain_.size(); ++i) {
		if(cross) {
			wBaby1[i] = wMum[i];
			wBaby2[i] = wDad[i];
		} else {
			wBaby1[i] = wDad[i];
			wBaby2[i] = wMum[i];
		}
	}
	*/

	//determine a crossover point
	size_t cp = iRand(0, mum.brain_.size() - 1);
	size_t i = 0;
	//create the offspring
	for (; i < cp; ++i) {
		wBaby1[i] = wMum[i];
		wBaby2[i] = wDad[i];
	}

	for (i = cp; i < mum.brain_.size(); ++i) {
		wBaby1[i] = wDad[i];
		wBaby2[i] = wMum[i];
	}

	return {baby1, baby2};
}

//-----------------------------------Epoch()-----------------------------
//
//	takes a population of chromosones and runs the algorithm through one
//	 cycle.
//	Returns a new population of chromosones.
//
//-----------------------------------------------------------------------
void GeneticAlgorithm::epoch(Population& old_pop, Population& new_pop) {
	//reset the appropriate variables
	reset();

	for(Tank& t : old_pop) {
		t.calculateFitness();
	}

	//sort the population (for scaling and elitism)
	sort(old_pop.begin(), old_pop.end());

	//calculate best, worst, average and total fitness
	calculateBestWorstAvTot(old_pop);

	//Now to add a little elitism we shall add in some copies of the
	//fittest genomes. Make sure we add an EVEN number or the roulette
	//wheel sampling will crash
	if (!(numCopiesElite_ * (numElite_ % 2))) {
		copyNBest(numElite_, numCopiesElite_, old_pop, new_pop);
	}

	//now we enter the GA loop
	//repeat until a new population is generated
	while (new_pop.size() < old_pop.size()) {
		//grab two chromosones

		Tank& mum = getChromoRoulette(old_pop);
		Tank& dad = getChromoRoulette(old_pop);

		//create some offspring via crossover
		std::pair<Tank, Tank> babies = crossover(mum, dad);

		//now we mutate
		mutate(babies.first.brain_);
		mutate(babies.second.brain_);

		//now copy into vecNewPop population
		new_pop.push_back(babies.first);
		new_pop.push_back(babies.second);
	}
	this->generation_++;
}

//-------------------------GrabNBest----------------------------------
//
//	This works like an advanced form of elitism by inserting NumCopies
//  copies of the NBest most fittest genomes into a population vector
//--------------------------------------------------------------------
void GeneticAlgorithm::copyNBest(size_t nBest, const size_t numCopies,
		Population& in, Population& out) {
	//add the required amount of copies of the n most fittest to the supplied population
	while (nBest--) {
		for (size_t i = 0; i < numCopies; ++i) {
			Tank &t = in[(in.size() - 1) - nBest];
			out.push_back(t.clone());
			assert(t.brain_ == (*(out.end() - 1)).brain_);
		}
	}
}

//-----------------------CalculateBestWorstAvTot-----------------------	
//
//	calculates the fittest and weakest genome and the average/total 
//	fitness scores
//---------------------------------------------------------------------
void GeneticAlgorithm::calculateBestWorstAvTot(Population& pop) {
	totalFitness_ = 0;

	size_t size = pop.size();
	double highestSoFar = 0;
	double lowestSoFar = 9999999;

	for (size_t i = 0; i < size; ++i) {
		//update fittest if necessary
		if (pop[i].fitness_ > highestSoFar) {
			highestSoFar = pop[i].fitness_;
			fittestGenome_ = i;
			bestFitness_ = highestSoFar;
		}

		//update worst if necessary
		if (pop[i].fitness_ < lowestSoFar) {
			lowestSoFar = pop[i].fitness_;
			worstFitness_ = lowestSoFar;
		}

		totalFitness_ += pop[i].fitness_;
	}

	averageFitness_ = totalFitness_ / size;
}

//-------------------------Reset()------------------------------
//
//	resets all the relevant variables ready for a new generation
//--------------------------------------------------------------
void GeneticAlgorithm::reset() {
	totalFitness_ = 0;
	bestFitness_ = 0;
	worstFitness_ = 9999999;
	averageFitness_ = 0;
}
}
