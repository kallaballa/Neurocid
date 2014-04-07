/*
 * CustomScenarios.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: elchaschab
 */

#ifndef CUSTOMSCENARIOS_HPP_
#define CUSTOMSCENARIOS_HPP_

#include "scenario.hpp"
#include "population.hpp"
#include <vector>
#include <string>

namespace neurocid {

using std::vector;
using std::string;

void registerScenario(const string& name, Scenario* s);
void loadScenarios();
Scenario* getScenario(const string& name);
void multiplyTeams(vector<Population>& teams, size_t n);
} /* namespace neurocid */

#endif /* CUSTOMSCENARIOS_HPP_ */
