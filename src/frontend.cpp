#include "options.hpp"
#include "gamestate.hpp"
#include "canvas.hpp"
#include "time_tracker.hpp"
#include "renderer.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "scenario.hpp"
#include "gui.hpp"
#include "video_encoder.hpp"
#include "battlefieldlayout.hpp"

namespace neurocid {

void init_core(Coord width, Coord height, size_t frameRate) {
	Options& opt = *Options::getInstance();
	opt.WINDOW_WIDTH = width;
	opt.WINDOW_HEIGHT = height;
	opt.FRAMERATE = frameRate;
	GameState& gs = *GameState::getInstance();
	TimeTracker& timeTracker = *TimeTracker::getInstance();
	timeTracker.setEnabled(true);
	gs.setSlow(false);
	gs.setSlower(false);
}

//uses opts and initializes the canvas
void init_canvas(Canvas* canvas) {
	Canvas::init(canvas);
}

//initialize gui overlay
void init_gui(Gui* gui) {
	Gui::init(gui);
}

//initialize video capturing of the game
void init_video_capture(const string& captureFile) {
#ifndef _NO_VIDEOENC
  VideoEncoder::init(captureFile);
#endif
}

void destroy() {
  GameState::getInstance()->stop();
#ifndef _NO_VIDEOENC
  VideoEncoder::getInstance()->close();
#endif
  VideoEncoder::destroy();
  Options::destroy();
  GameState::destroy();
  TimeTracker::destroy();
  Canvas::destroy();
  Gui::destroy();
  Renderer::destroy();
}

void quit() {
  destroy();
  exit(0);
}

void play_game(size_t gameIter, Scenario* scenario,
		vector<Population>& teams, vector<GeneticPool>& pools,
		const string& videoFile, long autosaveInterval) {
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();
	scenario->configureTeams(teams);
	scenario->configurePools(pools);

	if(autosaveInterval > 0) {
	  gs.enableAutosave(autosaveInterval);
	}

	while (gs.isRunning() && --gameIter > 0) {
    tt.execute("game", [&]() {
      Game game(scenario, teams, pools);
      gs.setCurrentGame(&game);
      teams = game.play(true);
      gs.setCurrentGame(NULL);
    });
  }
	gs.stop();
}

bool is_running() {
	return GameState::getInstance()->isRunning();
}

void render() {
	Renderer::getInstance()->render();
}

}
