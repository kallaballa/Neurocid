#include "battlefield.hpp"
#include "population.hpp"
#include "canvas.hpp"
#include "tank.hpp"
#include "2d.hpp"
#include <iostream>
#include "brain.hpp"
#include "genetic.hpp"
#include "util.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "gamestate.hpp"
#include "time_tracker.hpp"
#include <ctime>
#include <thread>
#include <SDL/SDL_events.h>
#include <X11/Xlib.h>
#include <limits>
#include <boost/program_options.hpp>
#include <fstream>
#include <ctime>

using namespace tankwar;
namespace po = boost::program_options;
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
	Canvas& canvas = *Canvas::getInstance();
	Renderer& renderer = *Renderer::getInstance();
	GameState& gameState = *GameState::getInstance();
	TimeTracker& timeTracker = *TimeTracker::getInstance();
	SDL_Event event;

	while (gameState.isRunning()) {
		renderer.render();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
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
					if (timeTracker.isEnabled())
						timeTracker.setEnabled(false);
					else
						timeTracker.setEnabled(true);

				} else if (event.key.keysym.sym == SDLKey::SDLK_d) {
					dumpTeams();
				} else if (event.key.keysym.sym == SDLKey::SDLK_ESCAPE) {
					std::cerr << "Quitting" << std::endl;
					gameState.stop();
				}

				break;

			case SDL_KEYUP:
				break;

			default:
				break;
			}
		}
	}
}

class Scenario {
public:
	BattleFieldLayout bfl_;
	GameLayout gl_;
	PhysicsLayout phl_;

	virtual void configureTeams(vector<Population>& teams) {};
	virtual void configurePools(vector<GeneticPool>& pools) {};
	virtual void restoreTeams(vector<Population>& teams) {};
	virtual void restorePools(vector<GeneticPool>& pools) {};
	virtual Placer* createPlacer() = 0;
};

class AimOnOneNoMove : public Scenario {
public:
	AimOnOneNoMove() :
		Scenario() {
		bfl_.iterations_ = 800;
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;

		gl_.center_ = Vector2D(bfl_.width_ / 2, bfl_.height_/ 2);
		gl_.distance_ = 1000;
		gl_.spacing_ = 40;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_damage_ = 10;
		attackerTL.max_cooldown = 20;
		attackerTL.max_rotation_ = 0.5;

		teams[0].update(attackerTL);

		teams[1].clear();
		Tank c = teams[0][0].clone();
		c.teamID_ = 1;
		teams[1].push_back(c);

		TankLayout dummyTL = teams[1].layout_.tl_;
		dummyTL.isDummy_ = true;
		dummyTL.max_damage_ = 1000;
		teams[1].update(dummyTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
		pools[1] = GeneticPool(); //dummy pool;
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class AimOnOne : public Scenario {
public:
	AimOnOne() : Scenario(){
		bfl_.iterations_ = 600;
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;

		gl_.center_ = Vector2D(bfl_.width_ / 2, bfl_.height_/ 2);
		gl_.distance_ = 2500;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = true;
		attackerTL.max_ammo_ = 3;
		attackerTL.max_cooldown = 20;
		attackerTL.max_damage_ = 6;
		attackerTL.max_rotation_ = 0.5;

		teams[0].update(attackerTL);

		teams[1].clear();
		Tank c = teams[0][0].clone();
		c.teamID_ = 1;
		teams[1].push_back(c);

		TankLayout dummyTL = teams[1].layout_.tl_;
		dummyTL.isDummy_ = true;
		dummyTL.max_damage_ = 1000;
		teams[1].update(dummyTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
		pools[1] = GeneticPool(); //dummy pool;
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};


	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class CrossNoMove : public Scenario {
public:
	CrossNoMove() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 600;

		gl_.center_ = Vector2D(5000,5000);
		gl_.distance_ = 800;
		gl_.spacing_ = 15;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 3;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 8;
		attackerTL.max_speed_ = 1;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = true;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class CrossAttackerMove : public Scenario {
public:
	CrossAttackerMove() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 1000;

		gl_.center_ = Vector2D(5000,5000);
		gl_.distance_ = 800;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.canMove_ = true;
		attackerTL.max_ammo_ = 10;
		attackerTL.max_cooldown = 100;
		attackerTL.max_damage_ = 10;
		attackerTL.max_speed_ = 1;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = false;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

};
class SymmetricLinesNoMoveTwoRows : public Scenario {
public:
	SymmetricLinesNoMoveTwoRows() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 600;

		gl_.center_ = Vector2D(5000,5000);
		gl_.distance_ = 1500;
		gl_.spacing_ = 15;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 3;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 8;
		attackerTL.max_speed_ = 1;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = true;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};


class SymmetricLinesAttackerMoveTwoRows : public Scenario {
public:
	SymmetricLinesAttackerMoveTwoRows() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 1000;

		gl_.center_ = Vector2D(5000,5000);
		gl_.distance_ = 1500;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 3;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 8;
		attackerTL.max_speed_ = 1;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = true;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};


class SymmetricLines : public Scenario {
public:
	SymmetricLines() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 1000;

		gl_.center_ = Vector2D(5000,5000);
		gl_.distance_ = 1500;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 12;
		attackerTL.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = false;
		defenderTL.max_ammo_ = 30;
		defenderTL.max_cooldown = 200;
		defenderTL.max_damage_ = 12;
		defenderTL.max_speed_ = 3;
		defenderTL.pl_.max_speed_ = 3;
		teams[1].update(defenderTL);
	}

	virtual Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

};

class SymmetricLinesNoMove : public SymmetricLines {
public:
	SymmetricLinesNoMove() : SymmetricLines() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		SymmetricLines::configureTeams(teams);
		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = true;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesFar : public Scenario {
public:
	SymmetricLinesFar() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 5000;

		gl_.center_ = Vector2D(15000,15000);
		gl_.distance_ = 5000;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.04f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 12;
		attackerTL.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = false;
		defenderTL.max_ammo_ = 5;
		defenderTL.max_cooldown = 200;
		defenderTL.max_damage_ = 12;
		defenderTL.max_speed_ = 3;
		defenderTL.pl_.max_speed_ = 3;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};

class SymmetricLinesAttackerMoveFar : public Scenario {
public:
	SymmetricLinesAttackerMoveFar() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 5000;

		gl_.center_ = Vector2D(15000,15000);
		gl_.distance_ = 5000;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.04f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 12;
		attackerTL.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = true;
		attackerTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class CrossSmall : public Scenario {
public:
	CrossSmall() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 2000;

		gl_.center_ = Vector2D(7500,7500);
		gl_.distance_ = 5000;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 100;
		attackerTL.max_damage_ = 6;
		attackerTL.max_speed_ = 3;
		attackerTL.pl_.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = false;
		defenderTL.max_ammo_ = 5;
		defenderTL.max_cooldown = 100;
		defenderTL.max_damage_ = 6;
		defenderTL.max_speed_ = 3;
		defenderTL.pl_.max_speed_ = 3;

		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

};


class CrossBig : public Scenario {
public:
	CrossBig() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 5000;

		gl_.center_ = Vector2D(15000,15000);
		gl_.distance_ = 3000;
		gl_.spacing_ = 200;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.disableProjectileFitness_ = true;
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 6;
		attackerTL.max_speed_ = 3;
		attackerTL.pl_.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = false;
		defenderTL.disableProjectileFitness_ = true;
		defenderTL.max_ammo_ = 30;
		defenderTL.max_cooldown = 200;
		defenderTL.max_damage_ = 6;
		defenderTL.max_speed_ = 3;
		defenderTL.pl_.max_speed_ = 3;

		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};


class SymmetricLinesHuge : public Scenario {
public:
	SymmetricLinesHuge() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 5000;

		gl_.center_ = Vector2D(15000,15000);
		gl_.distance_ = 10000;
		gl_.spacing_ = 200;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.isDummy_ = false;
		attackerTL.disableProjectileFitness_ = true;
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 100;
		attackerTL.max_damage_ = 6;
		attackerTL.max_speed_ = 3;
		attackerTL.pl_.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.isDummy_ = false;
		defenderTL.disableProjectileFitness_ = true;
		defenderTL.max_ammo_ = 30;
		defenderTL.max_cooldown = 100;
		defenderTL.max_damage_ = 6;
		defenderTL.max_speed_ = 3;
		defenderTL.pl_.max_speed_ = 3;

		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}

};

void playGame(size_t gameIter, Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools, Placer& placer) {
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();
	while(gs.isRunning() && --gameIter > 0) {
		tt.execute("game", [&](){
			Game game(teams, pools, placer, scenario->bfl_, scenario->phl_);
			gs.setCurrentGame(&game);
			teams = game.play();
			gs.setCurrentGame(NULL);
		});
	}
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
	registerScenario("CrossNoMove", new CrossNoMove());
	registerScenario("CrossAttackerMove", new CrossAttackerMove());
	registerScenario("SymmetricLinesNoMoveTwoRows", new SymmetricLinesNoMoveTwoRows());
	registerScenario("SymmetricLinesAttackerMoveTwoRows", new SymmetricLinesAttackerMoveTwoRows());
	registerScenario("SymmetricLines", new SymmetricLines());
	registerScenario("SymmetricLinesFar", new SymmetricLinesFar());
	registerScenario("SymmetricLinesAttackerMoveFar", new SymmetricLinesAttackerMoveFar());
	registerScenario("SymmetricLinesNoMove", new SymmetricLinesNoMove());
	registerScenario("CrossSmall", new CrossSmall());
	registerScenario("CrossBig", new CrossBig());
	registerScenario("SymmetricLinesHuge", new SymmetricLinesHuge());
}

Scenario* getScenario(const string& name) {
	auto it = registeredScenarios.find(name);
	if(it == registeredScenarios.end())
		return NULL;
	else
		return (*it).second;
}

void runScenario(Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools, size_t iterations) {
	scenario->configureTeams(teams);
	scenario->configurePools(pools);
	Placer* placer = scenario->createPlacer();
	playGame(iterations, scenario, teams, pools, *placer);
	scenario->restoreTeams(teams);
}

void multiplyTeams(vector<Population>& teams, size_t n) {
	Population newA = teams[0];
	for (Tank& t : teams[0]) {
		for (size_t j = 0; j < n - 1; ++j) {
			newA.push_back(t.clone());
		}
	}

	Population newB = teams[1];
	for (Tank& t : teams[1]) {
		for (size_t j = 0; j < n - 1; ++j) {
			newB.push_back(t.clone());
		}
	}

	teams[0] = newA;
	teams[1] = newB;
}

int main(int argc, char** argv) {
	loadScenarios();
	GameState::getInstance()->setSlow(false);
	GameState::getInstance()->setSlower(false);
	PopulationLayout pl = {
		//Tank Layout
		{
		    //Projectile Layout
			{
				1,    // max_speed
				3000, // max_travel
				5     // range
			},
			false, // isDummy
			true,// canShoot
			true,// canRotate
			true,// canMove
			false,// disableProjectileFitness

			10.0,// range_
			1,// max_speed_
			1,// max_rotation

			200,// max_cooldown
			10,// max_ammo_
			3,  // max_damage_
			3  // crashes_per_damage
		},
		//BrainLayout
		{
		    43, // inputs
			3,  // outputs
			7,  // layers
			6  // neurons per hidden layer
		}
	};

	GeneticParams gp = {
			0.1, // mutationRate
			0.7, // crossoverRate
			1,   // crossoverIterations
			0.3, // maxPertubation
			4,   // numElite
			1    //  numEliteCopies
	};

	string loadFile;
	string saveFile;
	string scenarioName;
	Scenario* scenario = NULL;
	size_t gameIterations = 1000;
	size_t multiply = 0;
	vector<Population> teams;
	vector<GeneticPool> pools(2);
	pools[0] = GeneticPool(gp);
	pools[1] = GeneticPool(gp);

	po::options_description genericDesc("Options");
	genericDesc.add_options()
		("iterations,i", po::value< size_t >(&gameIterations), "Run n iterations of the game")
		("multiply,m", po::value< size_t >(&multiply), "Multiply the number of tanks in the populations")
		("load,l", po::value< string >(&loadFile), "Load the population from a file before running the scenario")
		("save,s", po::value< string >(&saveFile), "Save the population to a file after running the scenario")
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
        std::cerr << "Usage: tankwar [options]\n";
        std::cerr << visible;
        return 0;
    }

    if(vm.count("load")) {
    	ifstream is(loadFile);
    	read_teams(teams,is);
    } else {
    	teams = makeTeams(2,20, pl);
    }

    if(multiply > 1) {
    	multiplyTeams(teams, multiply);
    }

    assert(vm.count("scenario"));
    scenario = getScenario(scenarioName);
    assert(scenario != NULL);

    std::thread gameThread([&]() {
    	teams[0].score_ = 0;
    	teams[1].score_ = 0;
    	runScenario(scenario, teams, pools,gameIterations);
        if(vm.count("save")) {
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
