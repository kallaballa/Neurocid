#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <vector>
#include <stddef.h>

namespace tankwar {

using std::vector;
class Population;
class GeneticPool;
class GeneticParams;
class BrainLayout;

Population makePopulation(size_t teamID, size_t size, BrainLayout layout);
vector<Population> makeTeams(size_t numTeams, size_t teamSize, BrainLayout l);
vector<GeneticPool> makePools(size_t numTeams, GeneticParams gp);

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
