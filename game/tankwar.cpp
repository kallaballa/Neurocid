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

	virtual void configureTeams(vector<Population>& teams) = 0;
	virtual void configurePools(vector<GeneticPool>& pools) = 0;
};

class AimOnOneNoMove : public Scenario {
public:
	AimOnOneNoMove() :
		Scenario() {
		bfl_.iterations_ = 900;
		bfl_.width_ = 3000;
		bfl_.height_ = 3000;

		gl_.center_ = Vector2D(bfl_.width_ / 2, bfl_.height_/ 2);
		gl_.distance_ = std::min(bfl_.width_, bfl_.height_) / 3;
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
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 10;
		attackerTL.max_damage_ = 10;
		attackerTL.max_cooldown = 50;

		teams[0].update(attackerTL);

		TankLayout dummyTL = teams[1].layout_.tl_;
		dummyTL.canMove_ = false;
		dummyTL.canRotate_ = false;
		dummyTL.canShoot_ = false;
		dummyTL.max_damage_ = 1000;
		teams[1].update(dummyTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};

class AimOnOne : public Scenario {
public:
	AimOnOne() : Scenario(){
		bfl_.iterations_ = 600;
		bfl_.width_ = 3000;
		bfl_.height_ = 3000;

		gl_.center_ = Vector2D(bfl_.width_ / 2, bfl_.height_/ 2);
		gl_.distance_ = std::min(bfl_.width_, bfl_.height_) / 2;
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
		attackerTL.max_ammo_ = 3;
		attackerTL.max_cooldown = 20;
		attackerTL.max_damage_ = 6;
		attackerTL.max_speed_ = 0.2;

		teams[0].update(attackerTL);

		TankLayout dummyTL = teams[1].layout_.tl_;
		dummyTL.canMove_ = false;
		dummyTL.canRotate_ = false;
		dummyTL.canShoot_ = false;
		dummyTL.max_damage_ = 1000;
		teams[1].update(dummyTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};

class CrossNoMove : public Scenario {
public:
	CrossNoMove() : Scenario() {
		bfl_.width_ = 10000;
		bfl_.height_ = 10000;
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
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 3;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 8;
		attackerTL.max_speed_ = 1;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.canMove_ = false;
		defenderTL.canRotate_ = false;
		defenderTL.canShoot_ = false;
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
		bfl_.width_ = 10000;
		bfl_.height_ = 10000;
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
		defenderTL.canMove_ = false;
		defenderTL.canRotate_ = false;
		defenderTL.canShoot_ = false;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacerTwoRows<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};


class SymmetricLines : public Scenario {
public:
	SymmetricLines() : Scenario() {
		bfl_.width_ = 10000;
		bfl_.height_ = 10000;
		bfl_.iterations_ = 5000;

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
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 12;
		attackerTL.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.max_ammo_ = 30;
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

class SymmetricLinesNoMove : public Scenario {
public:
	SymmetricLinesNoMove() : Scenario() {
		bfl_.width_ = 10000;
		bfl_.height_ = 10000;
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
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 6;
		teams[0].update(attackerTL);

		TankLayout dummyTL = teams[1].layout_.tl_;
		dummyTL.canMove_ = false;
		dummyTL.canRotate_ = false;
		dummyTL.canShoot_ = false;
		dummyTL.max_damage_ = 1000;
		teams[1].update(dummyTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};

class SymmetricLinesBig : public Scenario {
public:
	SymmetricLinesBig() : Scenario() {
		bfl_.width_ = 10000;
		bfl_.height_ = 10000;
		bfl_.iterations_ = 2500;

		gl_.center_ = Vector2D(5000,5000);
		gl_.distance_ = 2000;
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
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 200;
		attackerTL.max_damage_ = 12;
		attackerTL.max_speed_ = 3;
		attackerTL.pl_.max_speed_ = 2;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.max_ammo_ = 30;
		defenderTL.max_cooldown = 200;
		defenderTL.max_damage_ = 6;
		defenderTL.max_speed_ = 3;
		defenderTL.pl_.max_speed_ = 2;

		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer* createPlacer() {
		return new OppositePlacer<RandomRot, RandomFacer, Layouter>({}, {}, {Scenario::gl_});
	}
};


class SymmetricLinesHuge : public Scenario {
public:
	SymmetricLinesHuge() : Scenario() {
		bfl_.width_ = 30000;
		bfl_.height_ = 30000;
		bfl_.iterations_ = 5000;

		gl_.center_ = Vector2D(15000,15000);
		gl_.distance_ = 5000;
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
		attackerTL.max_ammo_ = 30;
		attackerTL.max_cooldown = 100;
		attackerTL.max_damage_ = 6;
		attackerTL.max_speed_ = 3;
		attackerTL.pl_.max_speed_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
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

int main(int argc, char** argv) {
	string trainingFile;
	string populationFile;

	po::options_description genericDesc("Options");
	genericDesc.add_options()
		("train,t", "Train a population and save it to the file")
		("play,p", "Load a population from file and play the game")
		("help,h", "Produce help message");


    po::options_description hidden("Hidden options");
    hidden.add_options()
    	("file", po::value< string >(), "file");

	po::options_description cmdline_options;
    cmdline_options.add(genericDesc).add(hidden);


    po::positional_options_description p;
    p.add("file", -1);

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

    if(vm.count("train") > 0 && vm.count("play") > 0) {
		std::cerr << "You can only use either -t or -p" << std::endl;
		exit(1);
    }

    if(vm.count("train")) {
    	trainingFile = vm["file"].as<string>();
    } else if(vm.count("play")) {
    	populationFile = vm["file"].as<string>();
    }

	std::thread gameThread([&]() {
		PopulationLayout pl = {
			//Tank Layout
			{
			    //Projectile Layout
				{
					1, // max_speed
					5   // range
				},
				true,// canShoot
				true,// canRotate
				true,// canMove

				10.0,// range_
				1,// max_speed_
				1,// max_rotation

				200,// max_cooldown
				10,// max_ammo_
				3// max_damage_
			},
			//BrainLayout
			{
				12, // inputs
				3,  // outputs
				6,  // layers
				6   // neurons per hidden layer
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

		if(!trainingFile.empty()) {
			vector<Population> teams(2);
			teams[0] = makePopulation(0, 20, pl);
			teams[1] = makePopulation(1, 1, pl);
			vector<GeneticPool> pools(2);
			pools[0] = GeneticPool(gp);
			pools[1] = GeneticPool(); // dummy pool

			AimOnOneNoMove* scenario = new AimOnOneNoMove();
			scenario->configureTeams(teams);
			scenario->configurePools(pools);
			Placer* placer = scenario->createPlacer();
			playGame(500, scenario, teams, pools, *placer);
			//delete scenario;
			//delete &placer;

			teams[1].clear();
			//clone teamB from teamA
			for(Tank& t : teams[0]) {
				Tank c = t.clone();
				c.teamID_ = 1;
				teams[1].push_back(c);
			}

			SymmetricLinesNoMove* scenario1 = new SymmetricLinesNoMove();
			scenario1->configureTeams(teams);
			scenario1->configurePools(pools);
			placer = scenario1->createPlacer();

			playGame(500, scenario1, teams, pools, *placer);

			teams[1].clear();
			//clone teamB from teamA
			for(Tank& t : teams[0]) {
				Tank c = t.clone();
				c.teamID_ = 1;
				teams[1].push_back(c);
			}

			pools[1] = GeneticPool(gp);

			SymmetricLines* scenario3 = new SymmetricLines();
			scenario3->configureTeams(teams);
			scenario3->configurePools(pools);
			placer = scenario3->createPlacer();

			playGame(1000, scenario3, teams, pools, *placer);

			//delete scenario1;
			//delete &placer;
			ofstream os(trainingFile);
			write_teams(teams,os);
		} else if(!populationFile.empty()) {
			vector<Population> teams(2);
			ifstream is(populationFile);
			read_teams(teams,is);

			vector<GeneticPool> pools(2);
			pools[0] = GeneticPool(gp);
			pools[1] = GeneticPool(gp);

		/*	CrossNoMove* scenario1 = new CrossNoMove();
			scenario1->configureTeams(teams);
			scenario1->configurePools(pools);
			Placer* placer = scenario1->createPlacer();

			playGame(10000, scenario1, teams, pools, *placer);

			SymmetricLinesNoMoveTwoRows* scenario2 = new SymmetricLinesNoMoveTwoRows();
			scenario2->configureTeams(teams);
			scenario2->configurePools(pools);
			placer = scenario2->createPlacer();

			playGame(2000, scenario2, teams, pools, *placer);

			teams[1].clear();
			//clone teamB from teamA
			for(Tank& t : teams[0]) {
				Tank c = t.clone();
				c.teamID_ = 1;
				teams[1].push_back(c);
			}*/

			Population newA = teams[0];
			for(Tank& t : teams[0]) {
				for(size_t j = 0; j < 4; ++j) {
					newA.push_back(t.clone());
				}
			}

			Population newB = teams[1];
			for(Tank& t : teams[1]) {
				for(size_t j = 0; j < 4; ++j) {
					newB.push_back(t.clone());
				}
			}

			teams[0] = newA;
			teams[1] = newB;

/*			SymmetricLinesBig* scenario4 = new SymmetricLinesBig();
			scenario4->configureTeams(teams);
			scenario4->configurePools(pools);
			Placer* placer = scenario4->createPlacer();
			playGame(1000, scenario4, teams, pools, *placer);
*/
				//make 5 copies of each tank
			newA = teams[0];
			for(Tank& t : teams[0]) {
				for(size_t j = 0; j < 4; ++j) {
					newA.push_back(t.clone());
				}
			}

			newB = teams[1];
			for(Tank& t : teams[1]) {
				for(size_t j = 0; j < 4; ++j) {
					newB.push_back(t.clone());
				}
			}

			teams[0] = newA;
			teams[1] = newB;

			SymmetricLinesHuge* scenario5 = new SymmetricLinesHuge();
			scenario5->configureTeams(teams);
			scenario5->configurePools(pools);
			Placer* placer = scenario5->createPlacer();

			playGame(std::numeric_limits<size_t>().max(), scenario5, teams, pools, *placer);

			//make sure we destroyed all brains left so valgrind doesn't complain
			for(Population& p : teams) {
				for(Tank& t : p) {
					if(!t.brain_->isDestroyed())
						t.brain_->destroy();
				}
			}
			std::cerr << "Done" << std::endl;
		}
		exit(0);
	});

	runEventHandler();
	gameThread.join();
	SDL_Quit();
}
