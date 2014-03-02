#include "battlefield.hpp"
#include "population.hpp"
#include "tank.hpp"
#include "2d.hpp"
#include <iostream>
#include "brain.hpp"

using namespace tankwar;
using std::cerr;
using std::endl;

int main(int argc, char** argv) {
	Canvas canvas(400,300);

	Population teamA;
	Population teamB;

	size_t numTeamA = 3;
	size_t numTeamB = 3;
	size_t numInputs = 2 + (2 * numTeamA) + (2 * numTeamB);
	size_t numOutputs = 3;
	size_t numLayers = 3;
	size_t numHiddenNeurons = 6;

	std::cerr << "Make Teams" << endl;
	for(int i = 0; i < numTeamA; ++i) {
		Tank t(Brain_ptr(new Brain(numInputs, numOutputs, numLayers, numHiddenNeurons)),0, {20, (20 * i) + 20}, {-1, 0});
		t.brain_->randomize();
		teamA.push_back(t);
	}

	for(int i = 0; i < numTeamB; ++i) {
		Tank t(Brain_ptr(new Brain(numInputs, numOutputs, numLayers, numHiddenNeurons)),1, {250, (20 * i) + 20}, {-1, 0});
		t.brain_->randomize();
		teamB.push_back(t);
	}

	std::cerr << "Make Battlefield" << endl;
	BattleField field(canvas, teamA, teamB);

	while(true) {
		std::cerr << "step" << endl;
		field.step();
	}
}
