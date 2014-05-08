#include "game.hpp"
#include "gamestate.hpp"
#include "renderer.hpp"
#include "physics.hpp"
#include "time_tracker.hpp"
#include "./lua.hpp"
#include <algorithm>
#include <chrono>
#include <thread>

namespace neurocid {

Game::Game(Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools) :
		scenario_(scenario),
		teams_(teams),
		newTeams_(teams.size()),
		pools_(pools) {
}

Game::~Game() {
}

void Game::prepare() {
	//shuffle positions
	for(Population& team : teams_) {
		random_shuffle(team.begin(), team.end());
		for(Ship& t : team) {
			t.resetScore();
			t.brain_->reset();
		}
	}
}

void Game::place() {
/*	scenario_->configureTeams(teams_);
	scenario_->configurePools(pools_); */
	lua::run_placer(scenario_->pl_.placer_, teams_, scenario_->pl_, 0);
}

void Game::fight(bool render) {
	//std::cerr << "####### game start #######" << std::endl;

	BattleField field(scenario_, teams_);
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();
	Renderer& renderer = *Renderer::getInstance();

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
			renderer.update(&field);
		}
#ifdef _NO_THREADS
	    	renderer.render();
#endif
	}
	});

	if(render)
		renderer.update(NULL);
}

void Game::score() {
	vector<size_t> ownedFacilites(2,0);

	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(Facility& f : team.facilities_) {
			++ownedFacilites[f.teamID_];
		}
	}

	if(ownedFacilites[0] != ownedFacilites[1]) {
		if(ownedFacilites[0] > ownedFacilites[1]) {
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
		newTeams_.push_back(pools_[i].epoch(teams_[i],scenario_->bfl_));
	}

	/*
	// copy some from winner to loser
	// FIXME: disable in play mode
	if(		   newTeams_[0].size() > pools_[0].layout_.numElite_
			&& newTeams_[0].size() > pools_[1].layout_.numElite_
			&& newTeams_[1].size() > pools_[0].layout_.numElite_
			&& newTeams_[1].size() > pools_[1].layout_.numElite_
	) {
		if(teams_[0].winner_) {
			for(size_t i = 0; i < pools_[0].layout_.numElite_; ++i) {
				if(newTeams_[0][i].layout_.isDummy_)
					continue;
				size_t j = newTeams_[1].size() - i - 1;
				newTeams_[1][j].brain_->destroy();
				newTeams_[1][j] = newTeams_[0][i].clone();
				newTeams_[1][j].teamID_ = 1;
			}
		} else if(teams_[1].winner_) {
			for(size_t i = 0; i < pools_[1].layout_.numElite_; ++i) {
				if(newTeams_[1][i].layout_.isDummy_)
					continue;
				size_t j = newTeams_[0].size() - i - 1;
				newTeams_[0][j].brain_->destroy();
				newTeams_[0][j] = newTeams_[1][i].clone();
				newTeams_[0][j].teamID_ = 0;
			}
		}
	}
	*/
}

void Game::cleanup() {
	for(Population& p : teams_) {
		for(Facility& f : p.facilities_) {
			f.teamID_ = p[0].teamID_;
			f.reset();
		}

		for(Ship& t : p) {
			t.resetGameState();
			t.brain_->destroy();
		}
	}

	for(Population& p : newTeams_) {
		for(Facility& f : p.facilities_) {
			f.teamID_ = p[0].teamID_;
			f.reset();
		}

		for(Ship& t : p) {
			t.resetGameState();
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

		tt.execute("game", "score", [&]() {
			score();
		});

		tt.execute("game", "mate", [&]() {
			mate();
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
		return teams_;
	}
	else {
		return newTeams_;
	}
}
} /* namespace neurocid */
