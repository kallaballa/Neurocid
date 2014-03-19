/*
 * game.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef GAME_HPP_
#define GAME_HPP_

#include "options.hpp"
#include "placer.hpp"
#include "genetic.hpp"
#include "physics.hpp"
#include <ctime>
#include <vector>
#include <stddef.h>
#include <cassert>

namespace tankwar {

using std::vector;

class Game {
private:
	void place();
	void fight();
	void score();
	void mate();
	void cleanup();
	void print();
	void prepare();
	Placer& placer_;
	BattleFieldLayout bfl_;
	PhysicsLayout phl_;
public:
	vector<Population>& teams_;
	vector<Population> newTeams_;
	vector<GeneticPool>& pools_;

	Game(vector<Population>& teams, vector<GeneticPool>& pools, Placer& placer, BattleFieldLayout& bl, PhysicsLayout& phl);
	vector<Population> play();
};

} /* namespace tankwar */

#endif /* GAME_HPP_ */
