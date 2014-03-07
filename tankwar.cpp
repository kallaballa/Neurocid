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

#include <thread>
#include <SDL/SDL_events.h>
#include <X11/Xlib.h>

using namespace tankwar;
using std::cerr;
using std::endl;
using std::vector;

/*
 * Make a population with a size of n using the brain layout
 */
Population makePopulation(size_t teamID, size_t size, BrainLayout layout) {
	Population p;
	for(size_t i = 0; i < size; i++) {
		Tank t(teamID, layout, {0, 0}, 0);
		t.brain_.randomize();
		p.push_back(t);
	}
	return p;
}

vector<Population> makeTeams(size_t numTeams, size_t teamSize, BrainLayout l) {
	vector<Population> teams(numTeams);
	size_t teamID = 0;
	std::generate(teams.begin(), teams.end(), [&]() { return makePopulation(teamID++, teamSize, l); });
	return teams;
}

vector<GeneticPool> makePools(size_t numTeams, GeneticParams gp) {
	vector<GeneticPool> pools(numTeams);
	std::generate(pools.begin(), pools.end(), [&]() { return GeneticPool(gp); });
	return pools;
}

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
							renderer.setEnabled(false);
							gameState.setSlow(false);
						} else {
							gameState.setSlow(true);
						}
					} else {
						renderer.setEnabled(true);
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

int main(int argc, char** argv) {
	std::thread gameThread([&]() {
		size_t battleIterations = 300;
		size_t numTeams = 2;
		size_t teamSize = 20;

		BrainLayout l = {
				4, // inputs
				3,  // outputs
				3, // layers
				6  // neurons per hidden layer
		};

		GeneticParams gp = {
				0.1, // mutationRate
				0.7, // crossoverRate
				0.3, // maxPertubation
				4,   // numElite
				1    //  numEliteCopies
		};

		vector<Population> teams = makeTeams(numTeams, teamSize, l);
		vector<GeneticPool> pools = makePools(numTeams, gp);

		RandomOppositeLinesFacingRandom placer;

		while(GameState::getInstance()->isRunning()) {
			Game game(battleIterations, teams, pools, placer);
			teams = game.play();
			assert(teams.size() == numTeams);
		}

		for(Population& p : teams) {
			for(Tank& t : p) {
				if(!t.brain_.isDestroyed())
					t.brain_.destroy();
			}
		}
	});

	runEventHandler();
	gameThread.join();
	SDL_Quit();
}
