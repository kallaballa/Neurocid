#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <vector>
#include <stddef.h>
#include <chrono>

namespace tankwar {

using std::vector;
class Population;
class GeneticPool;
class GeneticParams;
class BrainLayout;
class TankLayout;
class PopulationLayout;

Population makePopulation(size_t teamID, size_t size, PopulationLayout& pl);
vector<Population> makeTeams(size_t numTeams, size_t teamSize, PopulationLayout& pl);
vector<GeneticPool> makePools(size_t numTeams, GeneticParams& gp);

double fRand(double fMin, double fMax);
int iRand(int x,int y);

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
    template<typename F>
    static typename TimeT::rep execution(F const &func)
    {
        auto start = std::chrono::system_clock::now();
        func();
        auto duration = std::chrono::duration_cast< TimeT>(
            std::chrono::system_clock::now() - start);
        return duration.count();
    }
};

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
