#ifndef GENERATIVE_SCENARIOS_HPP_
#define GENERATIVE_SCENARIOS_HPP_

#include "scenario.hpp"
#include "population.hpp"
#include <vector>
#include <string>

namespace neurocid {

using std::vector;
using std::string;

void registerScenario(const string& name, Scenario* s);
void load_procedural_scenarios();
Scenario* get_procedural_scenario(const string& name);
void multiplyTeams(vector<Population>& teams, size_t n);
} /* namespace neurocid */

#endif /* GENERATIVE_SCENARIOS_HPP_ */
