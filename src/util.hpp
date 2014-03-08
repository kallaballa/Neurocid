/*
 * util.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

namespace tankwar {

Population makePopulation(size_t teamID, size_t size, BrainLayout layout) {
	Population p;
	for(size_t i = 0; i < size; i++) {
		Tank t(teamID, layout, {0, 0}, 0);
		t.brain_.randomize();
		p.push_back(t);
	}
	return p;
}

vector<Population> makeTeams(size_t numTeams, size_t teamSize, BrainLayout l) {
	vector<Population> teams(numTeams);
	size_t teamID = 0;
	std::generate(teams.begin(), teams.end(), [&]() { return makePopulation(teamID++, teamSize, l); });
	return teams;
}

vector<GeneticPool> makePools(size_t numTeams, GeneticParams gp) {
	vector<GeneticPool> pools(numTeams);
	std::generate(pools.begin(), pools.end(), [&]() { return GeneticPool(gp); });
	return pools;
}

double fRand(double fMin, double fMax);
int iRand(int x,int y);

template<class T>
void clamp(T &arg, T min, T max)
{
  if (arg < min) {
    arg = min;
  }

  if (arg > max) {
    arg = max;
  }
}
} /* namespace tankwar */

#endif /* UTIL_HPP_ */
