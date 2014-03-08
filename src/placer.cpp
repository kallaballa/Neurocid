/*
 * placer.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#include "placer.hpp"

namespace tankwar {

void OppositeLines::place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing) {
	assert(teams.size() == 2);

	Coord lineLength = (Params::TANK_RANGE + spacing) * teams[0].size();
	for(size_t i = 0; i < teams[0].size(); i++) {
		teams[0][i].loc_ = {center.x - (distance / 2), center.y - (lineLength / 2) + ((Params::TANK_RANGE + spacing) * i)};
		teams[0][i].rotation_ = 0;
		teams[0][i].updateDirection();
	}

	for(size_t i = 0; i < teams[1].size(); i++) {
		teams[1][i].loc_ = {center.x + (distance / 2), center.y - (lineLength / 2) + ((Params::TANK_RANGE + spacing) * i)};
		teams[1][i].rotation_ = 0;
		teams[1][i].updateDirection();
	}
}

void OppositeLinesFacingInward::place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing) {
	assert(teams.size() == 2);

	Coord lineLength = (Params::TANK_RANGE + spacing) * teams[0].size();
	for(size_t i = 0; i < teams[0].size(); i++) {
		teams[0][i].loc_ = {center.x - (distance / 2), center.y - (lineLength / 2) + ((Params::TANK_RANGE + spacing) * i)};
		Vector2D inDir = {1,0};
		teams[0][i].rotation_ = rotationFromDirection(inDir);
		teams[0][i].updateDirection();
	}

	for(size_t i = 0; i < teams[1].size(); i++) {
		teams[1][i].loc_ = {center.x + (distance / 2), center.y - (lineLength / 2) + ((Params::TANK_RANGE + spacing) * i)};
		Vector2D inDir = {-1,0};
		teams[1][i].rotation_ = rotationFromDirection(inDir);;
		teams[1][i].updateDirection();
	}
}

void RandomOppositeLines::place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing) {
	assert(teams.size() == 2);

	double rotation = fRand(-2,2);
	axisDir_ = directionFromRotation(rotation);
	Vector2D sideDir = axisDir_;
	sideDir.rotate(90);
	centerA_ = center;
    centerB_ = center;

	centerA_ += (axisDir_ * (distance/2));
	centerB_ -= (axisDir_ * (distance/2));

	Coord lengthA = (((Params::TANK_RANGE + spacing) * (teams[0].size() - 1)) / 2);
	Coord lengthB = (((Params::TANK_RANGE + spacing) * (teams[1].size() - 1)) / 2);

	Vector2D startA = centerA_;
	startA += (sideDir * lengthA);
	Vector2D startB = centerB_;
	startB -= (sideDir * lengthB);

	for(size_t i = 0; i < teams[0].size(); i++) {
		teams[0][i].loc_ = startA;
		teams[0][i].loc_ -= (sideDir * ((Params::TANK_RANGE + spacing) * i));
	}

	for(size_t i = 0; i < teams[1].size(); i++) {
		teams[1][i].loc_ = startB;
		teams[1][i].loc_ += (sideDir * ((Params::TANK_RANGE + spacing) * i));
	}
}

void RandomOppositeLinesFacingRandom::place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing) {
	RandomOppositeLines::place(teams, center, distance, spacing);
	for(size_t i = 0; i < teams[0].size(); i++) {
		teams[0][i].rotation_ = fRand(0,2 * M_PI);
		teams[0][i].updateDirection();
	}

	for(size_t i = 0; i < teams[1].size(); i++) {
		teams[1][i].rotation_ = fRand(0,2 * M_PI);
		teams[1][i].updateDirection();
	}
}

//FIXME test me again
void RandomOppositeLinesFacingInward::place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing) {
	RandomOppositeLines::place(teams, center, distance, spacing);
	for(size_t i = 0; i < teams[0].size(); i++) {
		Vector2D inDir = (center - centerA_);
		inDir.rotate(-90);
		teams[0][i].rotation_ = rotationFromDirection(inDir);
		teams[0][i].updateDirection();
	}

	for(size_t i = 0; i < teams[1].size(); i++) {
		Vector2D inDir = (center - centerB_);
		inDir.rotate(-90);
		teams[1][i].rotation_ = rotationFromDirection(inDir);
		teams[1][i].updateDirection();
	}
}

} /* namespace tankwar */
