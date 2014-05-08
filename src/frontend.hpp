#ifndef FRONTEND_HPP_
#define FRONTEND_HPP_

#include "2d.hpp"
#include "population.hpp"
#include "scenario.hpp"

struct SDL_Surface;

namespace neurocid {

class BattleFieldLayout;
class Gui;

void init(Coord width, Coord height, size_t frameRate);
SDL_Surface* init_canvas(BattleFieldLayout& bfl);
void init_gui(Gui* gui);
void quit();

void render();
bool is_running();
void play_game(size_t gameIter, Scenario* scenario,
		vector<Population>& teams, vector<GeneticPool>& pools,
		const string& videoFile);
}

#endif /* FRONTEND_HPP_ */
