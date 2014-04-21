#include "options.hpp"
#include "gamestate.hpp"
#include "canvas.hpp"
#include "time_tracker.hpp"
#include "renderer.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "scenario.hpp"
#include "gui/gui.hpp"
#include "video_encoder.hpp"
#include "gui/osd.hpp"
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
void init_canvas(BattleFieldLayout& bfl) {
	Canvas::init(bfl);
	Canvas& canvas = *Canvas::getInstance();
	canvas.enableDrawGrid(false);
	canvas.enableDrawEngines(false);
}

void init_gui() {
	Canvas& canvas = *Canvas::getInstance();
	// initialize osd overlay
	Gui::init(canvas.getSurface());
	OsdScreenWidget::init(canvas.width(), canvas.height());
	Gui& gui = *Gui::getInstance();
	OsdScreenWidget& osd = *OsdScreenWidget::getInstance();
	gui.setScreen(&osd);
}

void init_full(Coord width, Coord height, size_t frameRate, BattleFieldLayout& bfl) {
	init(width, height, frameRate);
	init_canvas(bfl);
	init_gui();
}

void quit() {
	SDL_Quit();
	exit(0);
}

void play_game(size_t gameIter, Scenario* scenario,
		vector<Population>& teams, vector<GeneticPool>& pools,
		const string& videoFile) {
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();
	Options& opt = *Options::getInstance();
	VideoEncoder& ve = *VideoEncoder::getInstance();
	if (!videoFile.empty())
		ve.init(opt.WINDOW_WIDTH, opt.WINDOW_HEIGHT, opt.FRAMERATE,
				videoFile.c_str(), AV_CODEC_ID_H264);

	scenario->configureTeams(teams);
	scenario->configurePools(pools);
    while(gs.isRunning() && --gameIter > 0) {
            tt.execute("game", [&](){
                    Game game(scenario, teams, pools);
                    gs.setCurrentGame(&game);
                    teams = game.play(true);
                    gs.setCurrentGame(NULL);
            });
    }
	if (!videoFile.empty())
		ve.close();

	gs.stop();
}

bool is_running() {
	return GameState::getInstance()->isRunning();
}

void render() {
	Renderer::getInstance()->render();
}

}

