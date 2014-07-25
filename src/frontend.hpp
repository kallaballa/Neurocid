#ifndef FRONTEND_HPP_
#define FRONTEND_HPP_

#include "2d.hpp"
#include "population.hpp"
#include "scenario.hpp"

struct SDL_Surface;

namespace neurocid {

class BattleFieldLayout;
class Gui;
class Canvas;

void init_core(Coord width, Coord height, size_t frameRate);
void init_canvas(Canvas* canvas);
void init_gui(Gui* gui);
void init_video_capture(const string& captureFile);
void quit();

void render();
bool is_running();
void play_game(size_t gameIter, Scenario* scenario,
		vector<Population>& teams, vector<GeneticPool>& pools,
		const string& videoFile, long autosaveInterval);
}

#endif /* FRONTEND_HPP_ */
