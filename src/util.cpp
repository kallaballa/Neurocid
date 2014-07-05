/*
 * util.cpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#include "util.hpp"
#include "ship.hpp"
#include "population.hpp"
#include "genetic.hpp"
#include "brain_swarm.hpp"
#include <cstdlib>


namespace neurocid {

typedef BrainSwarm Brain;

Population make_population(size_t teamID, PopulationLayout& pl) {
	Population p;
	p.layout_ = pl;
	for(size_t i = 0; i < pl.size_; i++) {
		Ship t(teamID, pl.sl_);
		Brain* b = new Brain();
		b->initialize(pl.bl_);
		b->randomize();
		t.setBrain(b);
		p.push_back(t);
	}
	return p;
}

vector<Population> make_teams(size_t numTeams, size_t teamSize, PopulationLayout& pl) {
	vector<Population> teams(numTeams);
	size_t teamID = 0;
	std::generate(teams.begin(), teams.end(), [&]() { return make_population(teamID++, pl); });
	return teams;
}

vector<GeneticPool> make_pools(size_t numTeams, GeneticLayout& gp) {
	vector<GeneticPool> pools(numTeams);
	std::generate(pools.begin(), pools.end(), [&]() { return GeneticPool(gp); });
	return pools;
}

int iRand(int x,int y) {
	return rand()%(y-x+1)+x;
}

Coord fRand(Coord fMin, Coord fMax)
{
	Coord f = (Coord)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

} /* namespace neurocid */
