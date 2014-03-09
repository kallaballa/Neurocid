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
						} else if(gameState.isSlower()) {
							renderer.setEnabled(false);
							gameState.setSlower(false);
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
		size_t battleIterations = 200;
		size_t numTeams = 2;
		size_t teamSize = 10;

		BrainLayout l = {
				8, // inputs
				3, // outputs
				5, // layers
				6  // neurons per hidden layer
		};

		GeneticParams gp = {
				0.1, // mutationRate
				0.7, // crossoverRate
				1,   // crossoverIterations
				0.3, // maxPertubation
				2,   // numElite
				1    //  numEliteCopies
		};

		vector<Population> teams = makeTeams(numTeams, teamSize, l);
		vector<GeneticPool> pools = makePools(numTeams, gp);

		OppositeLinesFacingRandom placer;

		while(GameState::getInstance()->isRunning()) {
			Game game(battleIterations, teams, pools, placer);
			teams = game.play();
			assert(teams.size() == numTeams);
		}

		//make sure we destroyed all brains left so valgrind doesn't complain
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
