#include "battlefield.hpp"
#include "population.hpp"
#include "gamestate.hpp"
#include "time_tracker.hpp"

#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>

namespace neurocid {
using std::vector;
using std::list;
using std::cerr;
using std::endl;

BattleField::BattleField(Scenario* scenario, vector<Population>& teams) :
	layout_(scenario->bfl_),
	physics_(scenario->phl_),
	scanner_(scenario->scl_),
	teams_(teams){
	physics_.create(*this);
}

void BattleField::scan() {
	scanner_.scan(*this);
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		size_t s = team.size();
		for(size_t j = 0; j < s; ++j) {
				team[j].log();
		}
	}
}

void BattleField::think() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		size_t s = team.size();
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
			Ship& t = team[j];
			if(t.dead_)
				continue;

			t.move(layout_);
			if(t.loc_.x_ < 0 || t.loc_.y_ < 0 || t.loc_.x_ > layout_.width_ || t.loc_.y_ > layout_.height_) {
			  t.dead_ = true;
			  continue;
			}
			if(t.willShoot()) {
				spawned_.push_back(t.shoot());
			}
		}

		for(Facility& f : team.facilities_) {
			f.move(layout_);
		}
	}

	physics_.update(spawned_);
	physics_.step();
}

void BattleField::cleanup() {
}

bool BattleField::step() {
	if(teams_[0].isDead() || teams_[1].isDead())
		return false;

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

	return true;
}

} /* namespace neurocid */
