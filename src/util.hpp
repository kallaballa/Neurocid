#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <vector>
#include <stddef.h>
#include <chrono>
#include "2d.hpp"

namespace neurocid {

using std::vector;
class Population;
class GeneticPool;
struct GeneticParams;
struct BrainLayout;
struct ShipLayout;
struct PopulationLayout;

Population makePopulation(size_t teamID, size_t size, PopulationLayout& pl);
vector<Population> makeTeams(size_t numTeams, size_t teamSize, PopulationLayout& pl);
vector<GeneticPool> makePools(size_t numTeams, GeneticParams& gp);

Coord fRand(Coord fMin, Coord fMax);
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
} /* namespace neurocid */

#endif /* UTIL_HPP_ */
