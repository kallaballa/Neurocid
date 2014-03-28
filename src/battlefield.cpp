/*
 * Battlefield.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */


#include "battlefield.hpp"
#include "population.hpp"
#include "gamestate.hpp"
#include "time_tracker.hpp"

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
	#pragma omp parallel num_threads(2)
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		size_t s = team.size();
		#pragma omp for
		for(size_t j = 0; j < s; ++j) {
			if(!team[j].dead_)
				team[j].think(layout_);
		}
	}
}

void BattleField::move() {
	//mpve tanks and update the simulation with spawned projectiles
	spawned_.clear();
	for(size_t i = 0; i < teams_.size(); ++i)  {
		Population& team  = teams_[i];
		for(size_t j = 0; j < team.size(); ++j)  {
			Tank& t = team[j];
			if(t.dead_)
				continue;

			t.move(layout_);
			if(t.willShoot()) {
				spawned_.push_back(t.shoot());
			}
		}
	}

	physics_.update(spawned_);
	physics_.step();
}

void BattleField::cleanup() {
}

void BattleField::step() {
	TimeTracker& tt = *TimeTracker::getInstance();
	tt.execute("battlefield", "scan", [&](){
		scan();
	});
	tt.execute("battlefield", "think", [&](){
		think();
	});
	tt.execute("battlefield", "move", [&](){
		move();
	});
	tt.execute("battlefield", "cleanup", [&](){
		cleanup();
	});
}

} /* namespace tankwar */
