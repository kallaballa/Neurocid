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
#include <algorithm>
#include <chrono>
#include <thread>


namespace tankwar {

Game::Game(vector<Population>& teams,
		vector<GeneticPool>& pools, Placer& placer, BattleFieldLayout& bfl, PhysicsLayout& phl) :
		placer_(placer),
		bfl_(bfl),
		phl_(phl),
		teams_(teams),
		newTeams_(teams.size()),
		pools_(pools) {
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
	placer_.place(teams_);
}

void Game::fight() {
	//std::cerr << "####### game start #######" << std::endl;

	BattleField field(bfl_, phl_, teams_);
	GameState& gs = *GameState::getInstance();
	for(size_t i = 0; (i < bfl_.iterations_) && gs.isRunning(); ++i) {
		field.step();
		while(gs.tryPause()) {};

		if(gs.isSlow()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(4));
		}
		else if(gs.isSlower()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

		Renderer::getInstance()->update(&field);
	}
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
			assert(!t.brain_->isDestroyed());
			t.brain_->destroy();
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

vector<Population> Game::play() {
	std::cerr << 1000.0 / measure<>::execution( [&]() {
		if(GameState::getInstance()->isRunning()) prepare();
		if(GameState::getInstance()->isRunning()) place();
		if(GameState::getInstance()->isRunning()) fight();
		if(GameState::getInstance()->isRunning()) mate();
		if(GameState::getInstance()->isRunning()) score();
		cleanup();

		if(GameState::getInstance()->isRunning()) print();
	}) << std::endl;

	return newTeams_;
}
} /* namespace tankwar */
