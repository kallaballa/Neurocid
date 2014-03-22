/*
 * Battlefield.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */


#include "battlefield.hpp"
#include "population.hpp"
#include "gamestate.hpp"
#include "bsp.hpp"

#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>

namespace tankwar {
using std::vector;
using std::list;
using std::cerr;
using std::endl;

BattleField::BattleField(BattleFieldLayout& bfl, PhysicsLayout& pl, vector<Population>& teams) :
		layout_(bfl),
		teams_(teams),
		physics_(pl),
		scanner_() {
	physics_.create(*this);
}

void BattleField::scan() {
	scanner_.scan(*this);
}

void BattleField::think() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		size_t s = team.size();
		#pragma omp parallel for num_threads(4)
		for(size_t j = 0; j < s; ++j) {
			if(!team[j].dead_)
				team[j].think(layout_);
		}
	}
}

void BattleField::move() {
	//mpve tanks and update the simulation with spawned projectiles
	vector<Projectile*> spawned;

	for(size_t i = 0; i < teams_.size(); ++i)  {
		Population& team  = teams_[i];
		for(size_t j = 0; j < team.size(); ++j)  {
			Tank& t = team[j];
			if(t.dead_)
				continue;

			t.move(layout_);
			if(t.willShoot()) {
				Projectile* p = t.shoot();
				spawned.push_back(p);
			}
		}
	}

	physics_.update(spawned);
	physics_.step();
    spawned.clear();
}

void BattleField::cleanup() {
}

void BattleField::step() {
	if(GameState::getInstance()->isRunning()) scan();
	if(GameState::getInstance()->isRunning()) think();
	if(GameState::getInstance()->isRunning()) move();
	cleanup();
}

} /* namespace tankwar */
