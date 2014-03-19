/*
 * util.cpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#include "util.hpp"
#include "tank.hpp"
#include "population.hpp"
#include "genetic.hpp"
#include <cstdlib>

namespace tankwar {

Population makePopulation(size_t teamID, size_t size, PopulationLayout& pl) {
	Population p;
	p.layout_ = pl;
	for(size_t i = 0; i < size; i++) {
		Tank t(teamID, pl.tl_);
		Brain* b = new Brain(pl.bl_);
		b->randomize();
		t.setBrain(b);
		p.push_back(t);
	}
	return p;
}

vector<Population> makeTeams(size_t numTeams, size_t teamSize, PopulationLayout& pl) {
	vector<Population> teams(numTeams);
	size_t teamID = 0;
	std::generate(teams.begin(), teams.end(), [&]() { return makePopulation(teamID++, teamSize, pl); });
	return teams;
}

vector<GeneticPool> makePools(size_t numTeams, GeneticParams& gp) {
	vector<GeneticPool> pools(numTeams);
	std::generate(pools.begin(), pools.end(), [&]() { return GeneticPool(gp); });
	return pools;
}

int iRand(int x,int y) {
	return rand()%(y-x+1)+x;
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

} /* namespace tankwar */
