#include "battlefield.hpp"
#include "population.hpp"
#include "gamestate.hpp"
#include "time_tracker.hpp"
#include "sound_encoder.hpp"
#include "renderer.hpp"
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
#ifndef _NO_GUI
  // lock the renderer while we move.
  Renderer::getInstance()->lock();
#endif
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
#ifndef _NO_GUI
  Renderer::getInstance()->unlock();
#endif
}

void BattleField::cleanup() {
}

void write_brain_dump(const string& name, Population& pop, int channels, float* buffer = NULL) {
  if(!SoundEncoder::getInstance()->has(name))
    return;

  bool destroy = false;
  if (buffer == NULL) {
    buffer = new float[channels];
    destroy = true;
  }

  CHECK(channels <= pop.layout_.bl_.numOutputs);

  for (size_t i = 0; i < channels; ++i) {
    for (Ship& s : pop) {
      buffer[i] += s.brain_->outputs_[i];
    }
    buffer[i] /= pop.size();
  }

  SoundEncoder::getInstance()->encode(name, buffer, channels);
  if (destroy)
    delete buffer;
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
	int channels = teams_[0].layout_.bl_.numOutputs;
	write_brain_dump("teamA", teams_[0], channels);
  write_brain_dump("teamB", teams_[1], channels);

	++(teams_[0].stats_.battleFieldIterations_);
  ++(teams_[1].stats_.battleFieldIterations_);

	return true;
}

} /* namespace neurocid */
