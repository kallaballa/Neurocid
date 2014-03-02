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

using namespace tankwar;
using std::cerr;
using std::endl;

int main(int argc, char** argv) {
	Canvas canvas(800,600);


	std::thread t([&]() {
		while(true) {
			XInitThreads();
			SDL_Event event;

			while( SDL_PollEvent( &event ) ){
				switch( event.type ){
				  case SDL_KEYDOWN:
					  if(event.key.keysym.scancode == 0x41) {
						  if(canvas.isEnabled()) {
							  if(canvas.getTimeout() == 20)
								  canvas.setTimeout(5);
							  else
								  canvas.setEnabled(false);
						  } else {
							  canvas.setTimeout(20);
							  canvas.setEnabled(true);
						  }
					  }
					  break;

				  case SDL_KEYUP:
					break;

				  default:
					break;
				}
			}
		}
	});

	GeneticAlgorithm genalgA(0.1, 0.7, 0.3, 4, 1);
	GeneticAlgorithm genalgB(0.1, 0.7, 0.3, 4, 1);
	Population teamA;
	Population teamB;

	size_t numTeamA = 20;
	size_t numTeamB = 20;
	Params::NUM_INPUTS = 3 + (2 * numTeamA) + (2 * numTeamB);
	Params::NUM_OUTPUTS = 3;
	Params::NUM_LAYERS = 6;
	Params::NUM_NEURONS_PER_HIDDEN = 12;

	std::cerr << "Make Teams" << endl;
	for(size_t i = 0; i < numTeamA; i++) {
		Tank t1(0, {50, (20 * i) + 20}, {1, 0});
		t1.brain_.randomize();
		teamA.push_back(t1);

/*		Tank t2(new Brain(numInputs, numOutputs, numLayers, numHiddenNeurons),0, {40, (10 * i) + 20}, {1, 0});
		t2.brain_.randomize();
		teamA.push_back(t2);*/
	}

	for(size_t i = 0; i < numTeamB; i++) {
		Tank t1(1, {750, (20 * i) + 20}, {-1, 0});
		t1.brain_.randomize();
		teamB.push_back(t1);
/*
		Tank t2(new Brain(numInputs, numOutputs, numLayers, numHiddenNeurons),1, {270, (10 * i) + 20}, {-1, 0});
		t2.brain_.randomize();
		teamB.push_back(t2);*/
	}


	while(true) {
		std::cerr << "Make Battlefield" << "\t" << teamA.size() << "\t" <<teamB.size() << endl;
		BattleField field(canvas, teamA, teamB);

		for(size_t i = 0; i < 300; ++i) {
			field.step();
			canvas.render(field);
		}

		Population newTeamA;
		Population newTeamB;
		genalgA.epoch(teamA, newTeamA);
		genalgB.epoch(teamB, newTeamB);

		for(Tank& t: teamA) {
			t.brain_.destroy();
		}

		for(Tank& t: teamB) {
			t.brain_.destroy();
		}

		teamA = newTeamA;
		teamB = newTeamB;

		std::cerr << "TeamA Gen/Best/Avg:\t" << genalgA.generation() << "\t" << genalgA.bestFitness() << "\t" << genalgA.averageFitness() << endl;
		std::cerr << "TeamB Gen/Best/Avg:\t" << genalgB.generation() << "\t" << genalgB.bestFitness() << "\t" << genalgB.averageFitness() << endl;

		for(size_t i = 0; i < numTeamA ; i++) {
			teamA[i].reset();
			teamA[i].loc_ = {50, (20 * i) + 20};
			teamA[i].dir_ = {1, 0};
			/*teamA[i + 1].reset();
			teamA[i + 1].loc_ = {40, (10 * i) + 20};
			teamA[i + 1].dir_ = {1, 0};*/
		}

		for(size_t i = 0; i < numTeamB; i++) {
			teamB[i].reset();
			teamB[i].loc_ = {750, (20 * i) + 20};
			teamB[i].dir_ = {-1, 0};
			/*teamB[i + 1].reset();
			teamB[i + 1].loc_ = {270, (10 * i) + 20};
			teamB[i + 1].dir_ = {-1, 0};*/
		}
	}
}
