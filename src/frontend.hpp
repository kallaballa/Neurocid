#ifndef FRONTEND_HPP_
#define FRONTEND_HPP_

#include "2d.hpp"
#include "population.hpp"
#include "scenario.hpp"

namespace neurocid {

class BattleFieldLayout;

void init_full(Coord width, Coord height, size_t frameRate, BattleFieldLayout& layout);
void init(Coord width, Coord height, size_t frameRate);
void init_canvas(BattleFieldLayout& bfl);
void init_gui();
void quit();

void render();
bool is_running();
void play_game(size_t gameIter, Scenario* scenario,
		vector<Population>& teams, vector<GeneticPool>& pools,
		const string& videoFile);
}

#endif /* FRONTEND_HPP_ */
