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

	static PopulationLayout make_default_population_layout() {
		return {
			20, // size_
			//Ship Layout
			{
			    //Projectile Layout
				{
					1,    // max_speed
					10000, // max_travel
					5     // range
				},
				false, // isDummy
				true,// canShoot
				true,// canRotate
				true,// canMove
				false,// disableProjectileFitness

				50.0,// range_
				1.0, // max_speed_
				1.0, // max_rotation
				10000, // max_fuel
				1, // fuel_rate
				5, // max_cooldown
				5, // max_ammo_
				6, // max_damage_

				1, // crashes_per_damage_
				4  // num_perf_desc_
			},
			//BrainLayout
			{
			    84, // inputs
				5,  // outputs
				8,  // layers
				11  // neurons per hidden layer
			}
		};
	}

	static GeneticLayout make_default_genetic_layout() {
		return {
				0.1, // mutationRate
				0.7, // crossoverRate
				1,   // crossoverIterations
				0.3, // maxPertubation
				4,   // numElite
				1,   // numEliteCopies
				false// usePerfDesc_
		};
	}
}

#endif /* NEUROCID_HPP_ */
