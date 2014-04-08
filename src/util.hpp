#ifndef UTIL_HPP_
#define UTIL_HPP_

#include "2d.hpp"

#include <vector>
#include <stddef.h>
#include <chrono>
#include <string>

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

inline std::string get_env( const std::string & var ) {
     const char * val = ::getenv( var.c_str() );
     if ( val == 0 ) {
         return "";
     }
     else {
         return val;
     }
}

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
