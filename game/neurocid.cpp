#include "battlefield.hpp"
#include "population.hpp"
#include "ship.hpp"
#include "2d.hpp"
#include <iostream>
#include "brain.hpp"
#include "genetic.hpp"
#include "util.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "gamestate.hpp"
#include "time_tracker.hpp"
#include "scenario.hpp"
#include "canvas.hpp"
#include "gui/gui.hpp"
#include "gui/osd.hpp"

#include <ctime>
#include <thread>
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#include <X11/Xlib.h>
#include <limits>
#include <fstream>
#include <ctime>

#ifndef _NO_VIDEOENC
#include "video_encoder.hpp"
#endif

#ifndef _NO_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#endif

using namespace neurocid;
#ifndef _NO_PROGRAM_OPTIONS
namespace po = boost::program_options;
#endif
using std::cerr;
using std::endl;
using std::vector;
using std::ifstream;
using std::ofstream;

void dumpTeams() {
	GameState& gameState = *GameState::getInstance();
	gameState.pause();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	Game* game = gameState.getCurrentGame();
	if(game != NULL) {
		  time_t rawtime;
		  struct tm * timeinfo;
		  char buffer[80];

		  time (&rawtime);
		  timeinfo = localtime(&rawtime);

		  strftime(buffer,80,"dump%d-%m-%Y_%I:%M:%S.pop",timeinfo);
		  string filename(buffer);

		ofstream os(filename);
		write_teams(game->teams_, os);
	}
	gameState.resume();
}

void runEventHandler() {
	GameState& gameState = *GameState::getInstance();
	Canvas& canvas = *Canvas::getInstance();
	Renderer& renderer = *Renderer::getInstance();
	Gui& gui = *Gui::getInstance();
	OsdScreenWidget& osd = *OsdScreenWidget::getInstance();


	SDL_Event event;

	while (gameState.isRunning()) {
		renderer.render();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLKey::SDLK_ESCAPE) {
					std::cerr << "Quitting" << std::endl;
					gameState.stop();
				} else if(event.key.keysym.mod == KMOD_LALT || event.key.keysym.mod == KMOD_RALT) {
					if (event.key.keysym.sym == SDLKey::SDLK_SPACE) {
						if (renderer.isEnabled()) {
							if(gameState.isSlow()) {
								gameState.setSlower(true);
								renderer.setFramerate(25);
							} else if(gameState.isSlower()) {
								renderer.setEnabled(false);
								gameState.setSlower(false);
								renderer.setFramerate(12);
							} else {
								gameState.setSlow(true);
								renderer.setFramerate(25);
							}
						} else {
							renderer.setEnabled(true);
							renderer.setFramerate(12);
						}
					} else if (event.key.keysym.sym == SDLKey::SDLK_p) {
						if (gameState.isPaused())
							gameState.resume();
						else
							gameState.pause();
					} else if (event.key.keysym.sym == SDLKey::SDLK_t) {
						if (osd.isOsdTrackerVisible()) {
							osd.showOsdTracker(false);
						} else {
							osd.showOsdTracker(true);
						}
					} else if (event.key.keysym.sym == SDLKey::SDLK_s) {
						if (osd.isOsdStatisticsVisible()) {
							osd.showOsdStatistics(false);
						} else {
							osd.showOsdStatistics(true);
						}
					} else if (event.key.keysym.sym == SDLKey::SDLK_c) {
						if (canvas.isDrawCentersEnabled())
							canvas.enableDrawCenters(false);
						else
							canvas.enableDrawCenters(true);
					} else if (event.key.keysym.sym == SDLKey::SDLK_e) {
						if (canvas.isDrawEnginesEnabled())
							canvas.enableDrawEngines(false);
						else
							canvas.enableDrawEngines(true);
					} else if (event.key.keysym.sym == SDLKey::SDLK_g) {
						if (canvas.isDrawGridEnabled())
							canvas.enableDrawGrid(false);
						else
							canvas.enableDrawGrid(true);
					} else if (event.key.keysym.sym == SDLKey::SDLK_a) {
						if (canvas.isDrawProjectilesEnabled())
							canvas.enableDrawProjectiles(false);
						else
							canvas.enableDrawProjectiles(true);
					} else if (event.key.keysym.sym == SDLKey::SDLK_d) {
						dumpTeams();
					} else if (event.key.keysym.sym == SDLKey::SDLK_PLUS) {
						canvas.zoomIn();
					} else if (event.key.keysym.sym == SDLKey::SDLK_MINUS) {
						canvas.zoomOut();
					} else if (event.key.keysym.sym == SDLKey::SDLK_LEFT) {
						canvas.left();
					} else if (event.key.keysym.sym == SDLKey::SDLK_RIGHT) {
						canvas.right();
					} else if (event.key.keysym.sym == SDLKey::SDLK_UP) {
						canvas.up();
					} else if (event.key.keysym.sym == SDLKey::SDLK_DOWN) {
						canvas.down();
					}
				} else
					gui.pushEvent(event);
				break;

			case SDL_KEYUP:
				if (event.key.keysym.sym != SDLKey::SDLK_ESCAPE && event.key.keysym.mod != KMOD_LCTRL)
					gui.pushEvent(event);
				break;

			default:
				break;
			}
		}
	}
}

/* *************************
 * SYMMETRIC LINES SCENARIOS
 * *************************/

class SymmetricLines : public Scenario {
public:
	SymmetricLines() : Scenario() {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 1500;

		gl_.center_ = {150000,150000};
		gl_.distance_ = 10000;
		gl_.spacing_ = 400;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;

		scl_.disableClusterCenters = false;
		scl_.numClusters_ = 3;
		scl_.numFriends_ = 20;
		scl_.numEnemies_ = 20;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 20;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = false;
		defenderTL.max_ammo_ = 5;
		defenderTL.max_cooldown = 20;
		teams[1].update(defenderTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}
};

class SymmetricLinesNoMove : public SymmetricLines {
public:
	SymmetricLinesNoMove() : SymmetricLines() {
		gl_.distance_ = 2000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesAttackerMove : public SymmetricLines {
public:
	SymmetricLinesAttackerMove() : SymmetricLines() {
		gl_.distance_ = 10500;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = true;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);

	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesAttackerMoveFacingInward: public SymmetricLinesAttackerMove {
public:
	SymmetricLinesAttackerMoveFacingInward() : SymmetricLinesAttackerMove() {
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, OppositeFacer, Spacer>({}, {M_PI}, {Scenario::gl_});
	}
};

class SymmetricLinesFacingInward : public SymmetricLines {
public:
	SymmetricLinesFacingInward() : SymmetricLines() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		SymmetricLines::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canRotate_ = false;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<IterRot, OppositeFacer, Spacer>({0, M_PI}, {M_PI}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesFacingOutward : public SymmetricLinesFacingInward {
public:
	SymmetricLinesFacingOutward() : SymmetricLinesFacingInward() {
	}

	virtual Placer* createPlacer() {
		return new OppositePlacer<IterRot, OppositeFacer, Spacer>({0, M_PI}, {0}, {Scenario::gl_});
	}
};

class SymmetricLinesFacingOutwardFar : public SymmetricLinesFacingOutward {
public:
	SymmetricLinesFacingOutwardFar() : SymmetricLinesFacingOutward() {
		gl_.distance_ = 20000;
	}
};

class SymmetricLinesFacingInwardFar : public SymmetricLinesFacingInward {
public:
	SymmetricLinesFacingInwardFar() : SymmetricLinesFacingInward() {
		gl_.distance_ = 20000;
	}
};

class SymmetricLinesNoMoveShort : public SymmetricLines {
public:
	SymmetricLinesNoMoveShort() : SymmetricLines() {
		bfl_.iterations_ = 300;
		gl_.spacing_ = 400;
		gl_.distance_ = 2000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		attackerTL.max_cooldown = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesNoMoveTwoRows : public SymmetricLines {
public:
	SymmetricLinesNoMoveTwoRows() : SymmetricLines() {
	}

	virtual Placer* createPlacer() {
		return new OppositePlacerTwoRows<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveTwoRows : public SymmetricLinesNoMoveTwoRows {
public:
	SymmetricLinesAttackerMoveTwoRows() : SymmetricLinesNoMoveTwoRows() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesNoMoveTwoRows::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = true;
		teams[0].update(attackerTL);
	}
};


class SymmetricLinesFar : public SymmetricLines {
public:
	SymmetricLinesFar() : SymmetricLines() {
		bfl_.iterations_ = 4000;
		gl_.distance_ = 30000;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveFar : public SymmetricLinesFar {
public:
	SymmetricLinesAttackerMoveFar() : SymmetricLinesFar() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesFar::configureTeams(teams);
		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesAttackerMoveFarFacingInward : public SymmetricLinesAttackerMoveFar {
public:
	SymmetricLinesAttackerMoveFarFacingInward() : SymmetricLinesAttackerMoveFar() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesAttackerMoveFar::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		teams[0].update(attackerTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, OppositeFacer, Spacer>({}, {M_PI}, {Scenario::gl_});
	}
};

class SymmetricLinesHuge : public SymmetricLines {
public:
	SymmetricLinesHuge() : SymmetricLines() {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 6000;
		gl_.center_ = {150000,150000};
		gl_.distance_ = 100000;
		gl_.spacing_ = 1000;
		phl_.timeStep_ = 1.0 / 30.0;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.disableProjectileFitness_ = true;
		attackerTL.max_ammo_ = 40;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.disableProjectileFitness_ = true;
		defenderTL.max_ammo_ = 40;
		teams[1].update(defenderTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};
};

class AimOnOne : public Scenario {
public:
	AimOnOne() : Scenario(){
		bfl_.iterations_ = 800;
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;

		gl_.center_ = {150000,150000};
		gl_.distance_ = 2500;
		gl_.spacing_ = 400;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = true;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 5;

		teams[0].update(attackerTL);

		teams[1].clear();
		Ship c = teams[0][0].clone();
		c.teamID_ = 1;
		teams[1].push_back(c);

		ShipLayout dummyTL = teams[1][0].layout_;
		dummyTL.isDummy_ = true;
		dummyTL.max_damage_ = 100;
		teams[1].update(dummyTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
		pools[1] = GeneticPool(); //dummy pool;
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};


	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class AimOnOneNoMove : public AimOnOne {
public:
	AimOnOneNoMove() : AimOnOne() {
		bfl_.iterations_ = 300;
		gl_.distance_ = 2500;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		AimOnOne::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		attackerTL.max_cooldown = 10;

		teams[0].update(attackerTL);
	}
};

class CrossHuge : public SymmetricLines {
public:
	CrossHuge() : SymmetricLines() {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 7000;
		gl_.center_ = {150000,150000};
		gl_.distance_ = 100000;
		gl_.spacing_ = 1000;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.disableProjectileFitness_ = true;
		attackerTL.max_ammo_ = 40;
		attackerTL.max_cooldown = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.disableProjectileFitness_ = true;
		defenderTL.max_ammo_ = 40;
		defenderTL.max_cooldown = 5;
		teams[1].update(defenderTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}
};

void playGame(size_t gameIter, Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools, const string& videoFile) {
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();
#ifndef _NO_VIDEOENC
	VideoEncoder& ve = *VideoEncoder::getInstance();
	if(!videoFile.empty())
		ve.init(videoFile.c_str(), AV_CODEC_ID_H264);
#endif
	while(gs.isRunning() && --gameIter > 0) {
		tt.execute("game", [&](){
/*
			if(gameIter % 50 == 0) {
				GameState::getInstance()->setSlow(true);
				Canvas::getInstance()->enableDrawGrid(true);
				Canvas::getInstance()->enableDrawEngines(false);
			}
*/
			Game game(scenario, teams, pools);
			gs.setCurrentGame(&game);
			teams = game.play(true);
			gs.setCurrentGame(NULL);
/*
			GameState::getInstance()->setSlow(false);
			Canvas::getInstance()->enableDrawGrid(true);
			Canvas::getInstance()->enableDrawEngines(true);
*/
		});
	}
#ifndef _NO_VIDEOENC
	if(!videoFile.empty())
		ve.close();
#endif
}

map<string, Scenario*> registeredScenarios;

void registerScenario(const string& name, Scenario* s) {
	auto it = registeredScenarios.find(name);
	assert(it == registeredScenarios.end());
	registeredScenarios[name] = s;
}

void loadScenarios() {
	registerScenario("AimOnOneNoMove", new AimOnOneNoMove());
	registerScenario("AimOnOne", new AimOnOne());
	registerScenario("SymmetricLinesNoMoveTwoRows", new SymmetricLinesNoMoveTwoRows());
	registerScenario("SymmetricLinesAttackerMoveTwoRows", new SymmetricLinesAttackerMoveTwoRows());
	registerScenario("SymmetricLines", new SymmetricLines());
	registerScenario("SymmetricLinesAttackerMove", new SymmetricLinesAttackerMove());
	registerScenario("SymmetricLinesNoMove", new SymmetricLinesNoMove());
	registerScenario("SymmetricLinesFar", new SymmetricLinesFar());
	registerScenario("SymmetricLinesAttackerMoveFar", new SymmetricLinesAttackerMoveFar());
	registerScenario("SymmetricLinesNoMoveShort", new SymmetricLinesNoMoveShort());
	registerScenario("SymmetricLinesHuge", new SymmetricLinesHuge());
	registerScenario("SymmetricLinesFacingInward", new SymmetricLinesFacingInward());
	registerScenario("SymmetricLinesFacingInwardFar", new SymmetricLinesFacingInwardFar());
	registerScenario("SymmetricLinesFacingOutward", new SymmetricLinesFacingOutward());
	registerScenario("SymmetricLinesFacingOutwardFar", new SymmetricLinesFacingOutwardFar());
	registerScenario("SymmetricLinesAttackerMoveFarFacingInward", new SymmetricLinesAttackerMoveFarFacingInward());
	registerScenario("SymmetricLinesAttackerMoveFacingInward", new SymmetricLinesAttackerMoveFacingInward());
	registerScenario("CrossHuge", new CrossHuge());
}

Scenario* getScenario(const string& name) {
	auto it = registeredScenarios.find(name);
	if(it == registeredScenarios.end())
		return NULL;
	else
		return (*it).second;
}

void multiplyTeams(vector<Population>& teams, size_t n) {
	Population newA = teams[0];
	for (Ship& t : teams[0]) {
		for (size_t j = 0; j < n - 1; ++j) {
			newA.push_back(t.clone());
		}
	}

	Population newB = teams[1];
	for (Ship& t : teams[1]) {
		for (size_t j = 0; j < n - 1; ++j) {
			newB.push_back(t.clone());
		}
	}

	teams[0] = newA;
	teams[1] = newB;
}

int main(int argc, char** argv) {
	loadScenarios();
	PopulationLayout pl = {
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
			1, // max_speed_
			1, // max_rotation
			5, // max_cooldown
			5, // max_ammo_
			6, // max_damage_

			1, // crashes_per_damage_
			4  // num_perf_desc_
		},
		//BrainLayout
		{
		    83, // inputs
			5,  // outputs
			8,  // layers
			11  // neurons per hidden layer
		}
	};

	GeneticParams gp = {
			0.1, // mutationRate
			0.7, // crossoverRate
			1,   // crossoverIterations
			0.3, // maxPertubation
			4,   // numElite
			1,   // numEliteCopies
			false// usePerfDesc_
	};

	string loadFile;
	string saveFile;
	string captureFile;
	string scenarioName;
	bool save = false;
	Scenario* scenario = NULL;
	size_t gameIterations = 1000;
	size_t multiply = 0;
	size_t width = 800;
	size_t height = 800;
	size_t frameRate = 25;
	vector<Population> teams;
	vector<GeneticPool> pools(2);
	pools[0] = GeneticPool(gp);
	pools[1] = GeneticPool(gp);

#ifndef _NO_PROGRAM_OPTIONS
	po::options_description genericDesc("Options");
	genericDesc.add_options()
		("iterations,i", po::value< size_t >(&gameIterations), "Run n iterations of the game")
		("multiply,m", po::value< size_t >(&multiply), "Multiply the number of tanks in the populations")
		("load,l", po::value< string >(&loadFile), "Load the population from a file before running the scenario")
		("save,s", po::value< string >(&saveFile), "Save the population to a file after running the scenario")
		("capture,c", po::value< string >(&captureFile), "Capture the game to a video file")
		("width,x", po::value< size_t >(&width), "The window width")
		("height,y", po::value< size_t >(&height), "The window height")
		("framerate,f", po::value< size_t >(&frameRate), "The frame rate of the renderer and video encoder")
		("help,h", "Produce help message");

    po::options_description hidden("Hidden options");
    hidden.add_options()
    	("scenario", po::value< string >(&scenarioName), "scenario");

	po::options_description cmdline_options;
    cmdline_options.add(genericDesc).add(hidden);

    po::positional_options_description p;
    p.add("scenario", -1);

    po::options_description visible;
    visible.add(genericDesc);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cerr << "Usage: neurocid [options]\n";
        std::cerr << visible;
        return 0;
    }

    save = vm.count("save");
    assert(vm.count("scenario"));

    if(vm.count("load")) {
    	ifstream is(loadFile);
    	read_teams(teams,is);
    } else {
    	teams = makeTeams(2,20, pl);
    }

    if(multiply > 1) {
    	multiplyTeams(teams, multiply);
    }

    Options& opt = *Options::getInstance();
	opt.WINDOW_WIDTH = width;
    opt.WINDOW_HEIGHT = height;
    opt.FRAMERATE = frameRate;

    GameState& gs = *GameState::getInstance();
	Canvas& canvas = *Canvas::getInstance();
	TimeTracker& timeTracker = *TimeTracker::getInstance();

	timeTracker.setEnabled(true);

	gs.setSlow(false);
    gs.setSlower(false);

    canvas.enableDrawGrid(false);
    canvas.enableDrawEngines(true);

	// initialize osd overlay
	Gui::init(canvas.getSurface());
	OsdScreenWidget::init(canvas.width(),canvas.height());
	Gui& gui = *Gui::getInstance();
	OsdScreenWidget& osd = *OsdScreenWidget::getInstance();
	gui.setScreen(&osd);

    scenario = getScenario(scenarioName);
    assert(scenario != NULL);
#endif

    std::thread gameThread([&]() {
    	teams[0].score_ = 0;
    	teams[1].score_ = 0;
    	playGame(gameIterations, scenario, teams, pools, captureFile);
        if(save) {
      	  if(teams[0].size() > 20)
      		  teams[0].resize(20);

      	  if(teams[1].size() > 20)
      		  teams[1].resize(20);

        	ofstream os(saveFile);
        	write_teams(teams,os);
        }

        //FIXME release pool allocator memory
        //FIXME destroy remaining brains
        //FIXME destroy remaining projectiles
        exit(0);
	});

	runEventHandler();
	gameThread.join();
	SDL_Quit();
}
