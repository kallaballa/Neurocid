/*
 * game.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#include "game.hpp"
#include "gamestate.hpp"
#include "renderer.hpp"
#include <algorithm>
#include <chrono>
#include <thread>

namespace tankwar {

Game::Game(size_t battleIterations, vector<Population>& teams, vector<GeneticPool>& pools, Placer& placer) :
	battleIterations_(battleIterations),
	teams_(teams),
	newTeams_(teams.size()),
	pools_(pools),
	placer_(placer){
}

void Game::prepare() {
	//shuffle positions
	for(Population& team : teams_) {
		random_shuffle(team.begin(), team.end());
		for(Tank& t : team) {
			t.resetScore();
		}
	}
}

void Game::place() {
	Options& opts = *Options::getInstance();
	placer_.place(teams_, Vector2D(opts.WINDOW_WIDTH / 2, opts.WINDOW_HEIGHT / 2), std::min(opts.WINDOW_WIDTH, opts.WINDOW_HEIGHT) - 100, 20);
}

void Game::fight() {
	BattleField field(teams_);

	for(size_t i = 0; (i < battleIterations_) && GameState::getInstance()->isRunning(); ++i) {
		field.step();
		if(GameState::getInstance()->isSlow())
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		Renderer::getInstance()->update(&field);
	}
	Renderer::getInstance()->update(NULL);
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
			assert(!t.brain_.isDestroyed());
			t.brain_.destroy();
		}
	}
}

void Game::print() {
	for(GeneticPool& pool : pools_) {
		pool.statistics().print(std::cout);
		std::cout << ":";
	}
	std::cout << std::endl;
}


vector<Population> Game::play() {
	if(GameState::getInstance()->isRunning()) prepare();
	if(GameState::getInstance()->isRunning()) place();
	if(GameState::getInstance()->isRunning()) fight();
	if(GameState::getInstance()->isRunning()) mate();
	if(GameState::getInstance()->isRunning()) print();
	cleanup();
	return newTeams_;
}

} /* namespace tankwar */
