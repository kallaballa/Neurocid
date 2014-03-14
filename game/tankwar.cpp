#include "battlefield.hpp"
#include "population.hpp"
#include "tank.hpp"
#include "2d.hpp"
#include <iostream>
#include "brain.hpp"
#include "genetic.hpp"
#include "util.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "gamestate.hpp"
#include <ctime>
#include <thread>
#include <SDL/SDL_events.h>
#include <X11/Xlib.h>

using namespace tankwar;
using std::cerr;
using std::endl;
using std::vector;

void runEventHandler() {
	Canvas& canvas = *Canvas::getInstance();
	Renderer& renderer = *Renderer::getInstance();
	GameState& gameState = *GameState::getInstance();
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
	Scenario(PopulationLayout pl) :
		pl_(pl){
	}

	PopulationLayout pl_;

	size_t numTeams_ = 2;

	BattleFieldLayout bfl_ = {
			300,   // battle iterations
			3000, // width
			3000  // height;
	};

	GameLayout gl_ = {
			Vector2D(bfl_.width_ / 2, bfl_.height_/ 2), // center
			std::min(bfl_.width_, bfl_.height_) / 4,    // distance
			20										    // spacing
	};

	virtual void configureTeams(vector<Population>& teams) = 0;
	virtual void configurePools(vector<GeneticPool>& pools) = 0;
};

class AimOnOne : public Scenario {
public:
	size_t teamSizeAttacker = 20;
	AimOnOne(PopulationLayout pl) :
		Scenario(pl){
	}

	GameLayout gl_ = {
			Vector2D(bfl_.width_ / 2, bfl_.height_/ 2), // center
			std::min(bfl_.width_, bfl_.height_) / 3,    // distance
			40										    // spacing
	};

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_rotation_ = 0.1;
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

	Placer& createPlacer() {
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesInward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(        {0,M_PI/2}, {M_PI},       {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesOutward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(       {0,M_PI}, {0},          {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesSheerInward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(   {0,M_PI/2}, {M_PI/4 * 3}, {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesSheerOutward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(  {0,M_PI/2}, {M_PI/4},     {gl_});
//		OppositePlacer<IterRot, RandomFacer,   Layouter>* oppositeLinesRandom = new OppositePlacer<IterRot, RandomFacer, Layouter>(        {0,M_PI}, { },          {gl_});

//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesInward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(       { }, {M_PI},       {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesOutward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(      { }, {0},          {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesSheerInward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(  { }, {M_PI/4 * 3}, {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesSheerOutward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>( { }, {M_PI/4},     {gl_});
		OppositePlacer<RandomRot, RandomFacer,   Layouter>* randomOppositeLinesRandom = new OppositePlacer<RandomRot, RandomFacer, Layouter>(       { }, { },          {gl_});

		return *randomOppositeLinesRandom;
	}
};

class AimOnOneNoMove : public Scenario {
public:
	AimOnOneNoMove(PopulationLayout pl) :
		Scenario(pl) {
	}

	size_t teamSizeAttacker = 20;

	GameLayout gl_ = {
			Vector2D(bfl_.width_ / 2, bfl_.height_/ 2), // center
			std::min(bfl_.width_, bfl_.height_) / 3,    // distance
			40										    // spacing
	};

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.canMove_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_rotation_ = 0.1;
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

	Placer& createPlacer() {
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesInward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(        {0,M_PI/2}, {M_PI},       {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesOutward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(       {0,M_PI}, {0},          {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesSheerInward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(   {0,M_PI/2}, {M_PI/4 * 3}, {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesSheerOutward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(  {0,M_PI/2}, {M_PI/4},     {gl_});
//		OppositePlacer<IterRot, RandomFacer,   Layouter>* oppositeLinesRandom = new OppositePlacer<IterRot, RandomFacer, Layouter>(        {0,M_PI}, { },          {gl_});

//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesInward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(       { }, {M_PI},       {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesOutward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(      { }, {0},          {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesSheerInward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(  { }, {M_PI/4 * 3}, {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesSheerOutward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>( { }, {M_PI/4},     {gl_});
		OppositePlacer<RandomRot, RandomFacer,   Layouter>* randomOppositeLinesRandom = new OppositePlacer<RandomRot, RandomFacer, Layouter>(       { }, { },          {gl_});

		return *randomOppositeLinesRandom;
	}
};


class SymmetricLines : public Scenario {
public:
	SymmetricLines(PopulationLayout pl) :
		Scenario(pl) {
	}

	BattleFieldLayout bfl_ = {
			500,  // battle iterations
			6000, // width
			6000  // height;
	};

	GameLayout gl_ = {
			Vector2D(bfl_.width_ / 2, bfl_.height_/ 2), // center
			std::min(bfl_.width_, bfl_.height_) / 4,    // distance
			40										    // spacing
	};

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		TankLayout attackerTL = teams[0].layout_.tl_;
		attackerTL.max_cooldown = 20;
		attackerTL.max_ammo_ = 20;
		attackerTL.max_damage_ = 3;
		teams[0].update(attackerTL);

		TankLayout defenderTL = teams[1].layout_.tl_;
		defenderTL.max_cooldown = 20;
		defenderTL.max_ammo_ = 20;
		attackerTL.max_damage_ = 3;
		teams[1].update(defenderTL);
	}

	void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
	}

	Placer& createPlacer() {
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesInward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(        {0,M_PI/2}, {M_PI},       {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesOutward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(       {0,M_PI}, {0},          {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesSheerInward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(   {0,M_PI/2}, {M_PI/4 * 3}, {gl_});
//		OppositePlacer<IterRot, OppositeFacer, Layouter>* oppositeLinesSheerOutward = new OppositePlacer<IterRot, OppositeFacer, Layouter>(  {0,M_PI/2}, {M_PI/4},     {gl_});
//		OppositePlacer<IterRot, RandomFacer,   Layouter>* oppositeLinesRandom = new OppositePlacer<IterRot, RandomFacer, Layouter>(        {0,M_PI}, { },          {gl_});

//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesInward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(       { }, {M_PI},       {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesOutward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(      { }, {0},          {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesSheerInward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>(  { }, {M_PI/4 * 3}, {gl_});
//		OppositePlacer<RandomRot, OppositeFacer, Layouter>* randomOppositeLinesSheerOutward = new OppositePlacer<RandomRot, OppositeFacer, Layouter>( { }, {M_PI/4},     {gl_});
		OppositePlacer<RandomRot, RandomFacer,   Layouter>* randomOppositeLinesRandom = new OppositePlacer<RandomRot, RandomFacer, Layouter>(       { }, { },          {gl_});

		return *randomOppositeLinesRandom;
	}
};

void playGame(size_t gameIter, Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools, Placer& placer) {
	while(GameState::getInstance()->isRunning() && --gameIter > 0) {
		Game game(scenario->bfl_.iterations_, teams, pools, placer, scenario->bfl_);
		teams = game.play();
		assert(teams.size() == scenario->numTeams_);
	}
}

int main(int argc, char** argv) {
	std::thread gameThread([&]() {
		PopulationLayout pl;

		GeneticParams gp = {
				0.1, // mutationRate
				0.7, // crossoverRate
				1,   // crossoverIterations
				0.3, // maxPertubation
				4,   // numElite
				1    //  numEliteCopies
		};
		vector<Population> teams(2);
		teams[0] = makePopulation(0, 20, pl);
		teams[1] = makePopulation(1, 1, pl);
		vector<GeneticPool> pools(2);
		pools[0] = GeneticPool(gp);
		pools[1] = GeneticPool(); // dummy pool

		AimOnOneNoMove* scenario = new AimOnOneNoMove(pl);
		scenario->configureTeams(teams);
		scenario->configurePools(pools);
		Placer& placer = scenario->createPlacer();

		playGame(1000, scenario, teams, pools, placer);
		//delete scenario;
		//delete &placer;

		AimOnOne* scenario1 = new AimOnOne(pl);
		scenario1->configureTeams(teams);
		scenario1->configurePools(pools);
		placer = scenario1->createPlacer();

		playGame(2000, scenario1, teams, pools, placer);
		//delete scenario1;
		//delete &placer;

		SymmetricLines* scenario2 = new SymmetricLines(pl);
		teams[1].clear();
		pools[1] = GeneticPool(gp);

		for(Tank& t : teams[0]) {
			Tank c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}

		scenario2->configureTeams(teams);
		scenario2->configurePools(pools);
		placer = scenario2->createPlacer();

		playGame(100000, scenario2, teams, pools, placer);
		//delete scenario1;
		//delete &placer;


		//make sure we destroyed all brains left so valgrind doesn't complain
		for(Population& p : teams) {
			for(Tank& t : p) {
				if(!t.brain_->isDestroyed())
					t.brain_->destroy();
			}
		}
		std::cerr << "Done" << std::endl;
		exit(0);
	});

	runEventHandler();
	gameThread.join();
	SDL_Quit();
}
