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
struct GeneticLayout;
struct BrainLayout;
struct ShipLayout;
struct PopulationLayout;

Population make_population(size_t teamID, PopulationLayout& pl);
vector<Population> make_teams(size_t numTeams, size_t teamSize, PopulationLayout& pl);
vector<GeneticPool> make_pools(size_t numTeams, GeneticLayout& gp);

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
