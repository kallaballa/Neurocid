/*
 * lua.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: elchaschab
 */

#ifndef LUA_HPP_
#define LUA_HPP_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "ship.hpp"

namespace neurocid {

using std::string;
struct PlacerLayout;
struct BattleFieldLayout;

namespace lua {
double run_fitness_function(const string& script, Ship& ship, const BattleFieldLayout& bfl);
void run_placer(const string& name, vector<Population>& teams, const PlacerLayout& sl, const size_t& tick);
}
} /* namespace neurocid */

#endif /* LUA_HPP_ */
