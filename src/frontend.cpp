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

void init(Coord width, Coord height, size_t frameRate) {
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

void init_gui(Gui* gui) {
	// initialize osd overlay
	Gui::init(gui);
}

void quit() {
	exit(0);
}

void play_game(size_t gameIter, Scenario* scenario,
		vector<Population>& teams, vector<GeneticPool>& pools,
		const string& videoFile, long autosaveInterval) {
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();
	Options& opt = *Options::getInstance();
#ifndef _NO_VIDEOENC
	VideoEncoder& ve = *VideoEncoder::getInstance();
	if (!videoFile.empty())
		ve.init(opt.WINDOW_WIDTH, opt.WINDOW_HEIGHT, opt.FRAMERATE,
				videoFile.c_str(), AV_CODEC_ID_H264);
#endif
	scenario->configureTeams(teams);
	scenario->configurePools(pools);
	if(autosaveInterval > 0) {
	  gs.enableAutosave(autosaveInterval);
	}
    while(gs.isRunning() && --gameIter > 0) {
            tt.execute("game", [&](){
                    Game game(scenario, teams, pools);
                    gs.setCurrentGame(&game);
                    teams = game.play(true);
                    gs.setCurrentGame(NULL);
            });
    }
#ifndef _NO_VIDEOENC
    if (!videoFile.empty())
		ve.close();
#endif
	gs.stop();
}

bool is_running() {
	return GameState::getInstance()->isRunning();
}

void render() {
	Renderer::getInstance()->render();
}

}

