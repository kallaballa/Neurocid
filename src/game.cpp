/*
 * game.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#include "game.hpp"
#include "gamestate.hpp"
#include "renderer.hpp"
#include "physics.hpp"
#include "time_tracker.hpp"
#include <algorithm>
#include <chrono>
#include <thread>

namespace tankwar {

Game::Game(Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools) :
		scenario_(scenario),
		placer_(scenario->createPlacer()),
		teams_(teams),
		newTeams_(teams.size()),
		pools_(pools) {
	scenario->configureTeams(teams);
	scenario->configurePools(pools);
}

Game::~Game() {
	delete placer_;
}

void Game::prepare() {
	//shuffle positions
	for(Population& team : teams_) {
		random_shuffle(team.begin(), team.end());
		for(Tank& t : team) {
			t.resetScore();
			t.brain_->reset();
		}
	}
}

void Game::place() {
	placer_->place(teams_);
}

void Game::fight(bool render) {
	//std::cerr << "####### game start #######" << std::endl;

	BattleField field(scenario_, teams_);
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();

	tt.execute("battlefield", [&](){
	for(size_t i = 0; (i < scenario_->bfl_.iterations_) && gs.isRunning(); ++i) {
		auto dur = tt.measure([&]() {
			field.step();
		});

		if(render) {
			while(gs.tryPause()) {};

			if(gs.isSlow() && dur < 1600) {
				std::this_thread::sleep_for(std::chrono::microseconds(1600 - dur));
			}
			else if(gs.isSlower() && dur < 16000) {
				std::this_thread::sleep_for(std::chrono::microseconds(6400 - dur));
			}
			Renderer::getInstance()->update(&field);
		}
	}
	});

	if(render)
		Renderer::getInstance()->update(NULL);
}

void Game::score() {
	assert(teams_.size() == 2);
/*	size_t collisions = (teams_[0].stats_.totalHits_

			+ teams_[0].stats_.totalFriendlyFire_
			+ teams_[1].stats_.totalHits_
			+ teams_[1].stats_.totalFriendlyFire_
	);

	size_t damage = (teams_[0].stats_.totalDamage_+ teams_[1].stats_.totalDamage_);

	assert(collisions == damage);*/
	vector<size_t> alive(2,0);

	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(Tank& t : team) {
			if(!t.dead_)
				++alive[i];
		}
	}

	if(alive[0] != alive[1]) {
		if(alive[0] > alive[1]) {
			teams_[0].score_++;
			teams_[0].winner_=true;
		} else {
			teams_[1].score_++;
			teams_[1].winner_=true;
		}
	}

	newTeams_[0].score_ = teams_[0].score_;
	newTeams_[1].score_ = teams_[1].score_;
}

void Game::mate() {
	newTeams_.clear();
	for(size_t i = 0; i < teams_.size(); ++i) {
		newTeams_.push_back(pools_[i].epoch(teams_[i]));
	}
}


void Game::cleanup() {
	for(Population& p : teams_) {
		for(Tank& t : p) {
			t.resetGameState();
			t.brain_->destroy();
		}
	}
}

void Game::print() {
	for(Population& team : teams_) {
		team.stats_.print(std::cout);
		std::cout << ":";
	}
	std::cout << std::endl;
}

vector<Population> Game::play(bool render) {
	TimeTracker& tt = *TimeTracker::getInstance();

	size_t dur = tt.measure([&]() {
		tt.execute("game", "prepare", [&]() {
					prepare();
				});

		tt.execute("game", "place", [&]() {
					place();
				});

		tt.execute("game", "fight", [&]() {
					fight(render);
				});

		tt.execute("game", "mate", [&]() {
					mate();
				});

		tt.execute("game", "score", [&]() {
					score();
				});

		tt.execute("game", "print", [&]() {
					print();
				});

		tt.execute("game", "cleanup", [&]() {
			cleanup();
		});
	});

	std::cerr << "game/s: " << 1000000.0f/dur << std::endl;

	if(!GameState::getInstance()->isRunning()) {
		scenario_->restoreTeams(teams_);
		return teams_;
	}
	else {
		scenario_->restoreTeams(newTeams_);
		return newTeams_;
	}
}
} /* namespace tankwar */
