/*
 * neurocid.hpp
 *
 *  Created on: Apr 13, 2014
 *      Author: elchaschab
 */

#ifndef NEUROCID_HPP_
#define NEUROCID_HPP_

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

namespace neurocid {
	static void init(Coord width, Coord height, size_t frameRate) {
		Options& opt = *Options::getInstance();
		opt.WINDOW_WIDTH = width;
		opt.WINDOW_HEIGHT = height;
		opt.FRAMERATE = frameRate;

		GameState& gs = *GameState::getInstance();
		TimeTracker& timeTracker = *TimeTracker::getInstance();

		timeTracker.setEnabled(true);

		gs.setSlow(false);
		gs.setSlower(false);

		Canvas& canvas = *Canvas::getInstance();
		canvas.enableDrawGrid(false);
		canvas.enableDrawEngines(false);
	}

	//uses opts and initializes the canvas
	static void init_canvas() {
		Canvas& canvas = *Canvas::getInstance();
		canvas.enableDrawGrid(false);
		canvas.enableDrawEngines(false);
	}

	static void init_gui() {
		Canvas& canvas = *Canvas::getInstance();
		// initialize osd overlay
		Gui::init(canvas.getSurface());
		OsdScreenWidget::init(canvas.width(),canvas.height());
		Gui& gui = *Gui::getInstance();
		OsdScreenWidget& osd = *OsdScreenWidget::getInstance();
		gui.setScreen(&osd);
	}

	static void render() {
		Renderer::getInstance()->render();
	}

	static bool is_running() {
		return GameState::getInstance()->isRunning();
	}

	static void play_game(size_t gameIter, Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools, const string& videoFile) {
		GameState& gs = *GameState::getInstance();
		TimeTracker& tt = *TimeTracker::getInstance();

	#ifndef _NO_VIDEOENC
		VideoEncoder& ve = *VideoEncoder::getInstance();
		if(!videoFile.empty())
			ve.init(videoFile.c_str(), AV_CODEC_ID_H264);
	#endif

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
	#ifndef _NO_VIDEOENC
		if(!videoFile.empty())
			ve.close();
	#endif
		gs.stop();
	}
}

#endif /* NEUROCID_HPP_ */
