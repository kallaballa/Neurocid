#ifndef GAME_HPP_
#define GAME_HPP_

#include "options.hpp"
#include "placer.hpp"
#include "genetic.hpp"
#include "physics.hpp"
#include "scenario.hpp"
#include <ctime>
#include <vector>
#include <stddef.h>
#include <cassert>

namespace neurocid {

using std::vector;

class Game {
private:
	void place();
	void fight(bool render);
	void score();
	void mate();
	void cleanup();
	void print();
	void prepare();
	Scenario* scenario_;
public:
	vector<Population>& teams_;
	vector<Population> newTeams_;
	vector<GeneticPool>& pools_;

	Game(Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools);
	~Game();
	vector<Population> play(bool render);
};

} /* namespace neurocid */


#endif /* GAME_HPP_ */
