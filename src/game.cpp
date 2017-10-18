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
		pools_(pools),
		field_(NULL),
		steps_(0) {
}

Game::~Game() {
}

void Game::prepare() {
	//shuffle positions
	for(Population& team : teams_) {
		random_shuffle(team.begin(), team.end());
		for(Ship& t : team) {
			t.resetScore();
      t.resetGameState();
			t.brain_->reset();
		}

    for(Facility& f : team.facilities_) {
      f.resetGameState();
      f.resetScore();
    }
	}
}

void Game::place() {
/*	scenario_->configureTeams(teams_);
	scenario_->configurePools(pools_); */
	lua::run_placer(scenario_->pl_.placer_, teams_, scenario_->pl_, 0);
}

void Game::score() {
	vector<size_t> ownedFacilites(2,0);

	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(Facility& f : team.facilities_) {
		  if(!f.dead_)
		    ++ownedFacilites[f.teamID_];
		}
	}

	if(ownedFacilites[0] != ownedFacilites[1]) {
		if(ownedFacilites[0] > ownedFacilites[1]) {
			teams_[0].score_++;
			teams_[0].winner_=true;
      teams_[1].winner_=false;
		} else {
			teams_[1].score_++;
			teams_[1].winner_=true;
      teams_[0].winner_=false;
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

	// copy some from winner to loser
	// FIXME: disable in play mode
	if(newTeams_[0].size() > pools_[0].layout_.numElite_
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
}

void Game::cleanup() {
	for(Population& p : teams_) {
		for(Facility& f : p.facilities_) {
			f.resetGameState();
		}

		for(Ship& t : p) {
			t.resetGameState();
			t.brain_->destroy();
		}
	}

	for(Population& p : newTeams_) {
		for(Facility& f : p.facilities_) {
			f.resetGameState();
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

void Game::start(){
  TimeTracker& tt = *TimeTracker::getInstance();

  tt.execute("game", "prepare", [&]() {
    prepare();
  });

  tt.execute("game", "place", [&]() {
    place();
  });

  field_ =  new BattleField(scenario_, teams_);
}

vector<Population> Game::finish(){
  TimeTracker& tt = *TimeTracker::getInstance();
#ifndef _NO_GUI
  Renderer::getInstance()->update(NULL);
#endif
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

  delete field_;

  if(!GameState::getInstance()->isRunning()) {
    return teams_;
  }
  else {
    return newTeams_;
  }
}

bool Game::step(bool render) {
  GameState& gs = *GameState::getInstance();
  TimeTracker& tt = *TimeTracker::getInstance();
  #ifndef _NO_GUI
  Renderer& renderer = *Renderer::getInstance();
#endif
  gs.pauseBarrier(100);
  bool cont = false;
  auto dur = tt.measure([&]() {
    cont = field_->step();
  });

  if(!cont)
    return false;
#ifndef _NO_THREADS
  if (render) {
    if (gs.isSlow() && dur < 1600) {
      std::this_thread::sleep_for(std::chrono::microseconds(1600 - dur));
    } else if (gs.isSlower() && dur < 16000) {
      std::this_thread::sleep_for(std::chrono::microseconds(64000 - dur));
    }
#ifndef _NO_GUI
    renderer.update(field_);
#endif
}
#else
    renderer.update(field_);
    if(steps_ % 10 == 0)
      renderer.render();
#endif


  ++steps_;
  if (steps_ >= scenario_->bfl_.iterations_ || !gs.isRunning() || (teams_[0].isDead() || teams_[1].isDead()))
    return false;
  else
    return true;
}

void Game::init() {
  TimeTracker::newGame();
  start();
}

vector<Population> Game::result() {
  finish();
    if(!GameState::getInstance()->isRunning()) {
	    return teams_;
    }
    else {
	    return newTeams_;
    }
}

vector<Population> Game::play(bool render) {
  TimeTracker::newGame();
	TimeTracker& tt = *TimeTracker::getInstance();

	size_t dur = tt.measure([&]() {
	  start();

		tt.execute("game", "fight", [&]() {
			bool cont;
		  do {
		    tt.execute("game", "step", [&]() {
		      cont = step(render);
		    });
		  }
		  while(cont);
		});

		finish();
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
