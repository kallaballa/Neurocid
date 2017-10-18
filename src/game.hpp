#ifndef GAME_HPP_
#define GAME_HPP_

#include "options.hpp"
#include "placer.hpp"
#include "genetic.hpp"
#include "physics.hpp"
#include "scenario.hpp"
#include "battlefield.hpp"
#include <ctime>
#include <vector>
#include <stddef.h>

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
  BattleField* field_;
  size_t steps_;

	Game(Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools);
	~Game();
	void start();
	vector<Population> finish();
	bool step(bool render = true);
	void init();
	vector<Population> result();
	vector<Population> play(bool render);
};

} /* namespace neurocid */


#endif /* GAME_HPP_ */
