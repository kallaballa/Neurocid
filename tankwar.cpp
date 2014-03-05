#include "battlefield.hpp"
#include "population.hpp"
#include "tank.hpp"
#include "2d.hpp"
#include <iostream>
#include "brain.hpp"
#include "genetic.hpp"
#include "util.hpp"
#include <thread>
#include <SDL/SDL_events.h>
#include <X11/Xlib.h>
#include <algorithm>

using namespace tankwar;
using std::cerr;
using std::endl;

int main(int argc, char** argv) {
	Canvas canvas(800, 800);
	bool run = true;

	std::thread t([&]() {
		GeneticAlgorithm genalgA(0.1, 0.7, 0.3, 2, 2);
		GeneticAlgorithm genalgB(0.1, 0.7, 0.3, 2, 2);

		size_t numTeamA = 20;
		size_t numTeamB = 20;
//	Params::NUM_INPUTS = 3 + (2 * numTeamA) + (2 * numTeamB);
		Params::NUM_INPUTS = 4;
		Params::NUM_OUTPUTS = 3;
		Params::NUM_LAYERS = 3;
		Params::NUM_NEURONS_PER_HIDDEN = 6;

		Population teamA;
		Population teamB;
		Population newTeamA;
		Population newTeamB;

		for(size_t i = 0; i < numTeamA; i++) {
			Tank t1(0, {0, 0}, 0);
			t1.brain_.randomize();
			teamA.push_back(t1);
		}

		for(size_t i = 0; i < numTeamB; i++) {
			Tank t1(1, {0, 0}, 0);
			t1.brain_.randomize();
			teamB.push_back(t1);
		}

		size_t side = 0;

		while(run) {
			side = iRand(0,3);
			for(size_t i = 0; i < numTeamA; i++) {
				teamA[i].resetGameState();
				if(side == 0) {
					teamA[i].loc_ = {750, (20 * i) + 20};
					teamA[i].rotation_ = M_PI / 2;
				} else if(side == 1) {
					teamA[i].loc_ = {50, (20 * i) + 20};
					teamA[i].rotation_ = -M_PI / 2;
				} else if(side == 2) {
					teamA[i].loc_ = {(20 * i) + 20, 750};
					teamA[i].rotation_ = M_PI;
				} else if(side == 3) {
					teamA[i].loc_ = {(20 * i) + 20, 50};
					teamA[i].rotation_ = 0;
				}
			}

			for(size_t i = 0; i < numTeamB; i++) {
				teamB[i].resetGameState();
				if(side == 0) {
					teamB[i].loc_ = {50, (20 * i) + 20};
					teamB[i].rotation_ = -M_PI / 2;
				} else if(side == 1) {
					teamB[i].loc_ = {750, (20 * i) + 20};
					teamB[i].rotation_ = M_PI / 2;
				} else if(side == 2) {
					teamB[i].loc_ = {(20 * i) + 20, 50};
					teamB[i].rotation_ = 0;
				} else if(side == 3) {
					teamB[i].loc_ = {(20 * i) + 20, 750};
					teamB[i].rotation_ = M_PI;
				}
			}

			BattleField field(canvas, teamA, teamB);

			for(size_t i = 0; (i < 300) && run; ++i) {
				field.step();
				canvas.render(field);
			}

			//	if(side == 3) {
			newTeamA.clear();
			newTeamB.clear();
			genalgA.epoch(teamA, newTeamA);
			genalgB.epoch(teamB, newTeamB);

			for(Tank& t: teamA) {
				assert(!t.brain_.isDestroyed());
				t.brain_.destroy();
			}

			for(Tank& t: teamB) {
				assert(!t.brain_.isDestroyed());
				t.brain_.destroy();
			}
			std::cerr << "TeamA Gen/Best/Avg:\t" << genalgA.generation() << "\t" << genalgA.bestFitness() << "\t" << genalgA.averageFitness() << endl;
			std::cerr << "TeamB Gen/Best/Avg:\t" << genalgB.generation() << "\t" << genalgB.bestFitness() << "\t" << genalgB.averageFitness() << endl;
			std::cerr << std::endl;

			assert(teamA.size() == teamB.size());
			assert(teamA.size() == newTeamA.size());
			assert(teamB.size() == newTeamB.size());

			teamA = newTeamA;
			teamB = newTeamB;

			//shuffle positions
			random_shuffle(teamA.begin(), teamA.end());
			random_shuffle(teamB.begin(), teamB.end());

			for(size_t i = 0; i < numTeamA; i++) {
				teamA[i].resetScore();
			}

			for(size_t i = 0; i < numTeamB; i++) {
				teamB[i].resetScore();
			}
			//	}

			/*		if(side == 3)
			 side = 0;
			 else
			 side++;*/
		}
	});
	while (run) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLKey::SDLK_SPACE) {
					if (canvas.isEnabled()) {
						if (canvas.getTimeout() == 20)
							canvas.setTimeout(1);
						else
							canvas.setEnabled(false);
					} else {
						canvas.setTimeout(20);
						canvas.setEnabled(true);
					}
				} else if (event.key.keysym.sym == SDLKey::SDLK_ESCAPE) {
					run = false;
					std::cerr << "Quitting" << std::endl;
				}

				break;

			case SDL_KEYUP:
				break;

			default:
				break;
			}
		}
	}

	t.join();
	SDL_Quit();
}
