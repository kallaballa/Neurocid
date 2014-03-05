/*
 * Tank.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#include "tank.hpp"
#include "params.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include <cstdlib>
#include <iostream>
#include "util.hpp"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
namespace tankwar {

Tank::Tank(size_t teamID, Vector2D loc, Coord rotation) :
		Object(loc, rotation, Params::TANK_RANGE, Params::MAX_TANK_SPEED, false, false),
		brain_(),
		teamID_(teamID){
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;

	if(projectiles_.empty()) {
		totalDiff = (Params::MAX_PROJECTILES * 1.5708);
	} else {
		for(Projectile& p : projectiles_) {
			Vector2D perfect = (p.startLoc_ - p.nearestEnemyLoc_);
			Vector2D candidate = (p.startLoc_ - p.loc_);
			Coord arcPerfect = atan2(perfect.x, perfect.y);
			Coord arcCanditate = atan2(candidate.x, candidate.y);
			Coord diff = fabs(fmod(fabs(arcPerfect) - fabs(arcCanditate), M_PI));
			totalDiff += diff;
	//		std::cerr << "perfect:" << arcPerfect << "\tcanidate:" << arcCanditate << "\tdiff: " << diff << std::endl;
		}
	}

	//std::cerr << "tdiff:" << totalDiff << std::endl;
	if(ammonition_ == Params::MAX_PROJECTILES)
		fitness_=0;
	else
		fitness_ = (
				(M_PI - (totalDiff/Params::MAX_PROJECTILES))
				+ ((M_PI/1) / ((ammonition_ / Params::MAX_PROJECTILES) + 1))
				)
				/ (double)(friendly_fire_ + 1);

	//std::cerr << "f:" << fitness_ << std::endl;
	assert(fitness_ >= 0);
	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
//	fitness_ = hits_;
//	fitness_ = Params::MAX_PROJECTILES - friendly_fire_;
}

void Tank::think(BattleField& field) {
	if(teamID_ == field.teamA_[0].teamID_)
		brain_.update(*this, field.teamA_, field.teamB_);
	else
		brain_.update(*this, field.teamB_, field.teamA_);
}

void Tank::move() {
	//assign the outputs
	lthrust_ = brain_.lthrust_;
	rthrust_ = brain_.rthrust_;

	assert(!isnan(lthrust_) && !isnan(rthrust_));
	wantsShoot_ = (brain_.shoot_ > 0);
	//std::cerr << brain_.shoot_ << std::endl;

	//calculate steering forces
	Thrust rotForce = lthrust_ - rthrust_;

	//clamp rotation
	clamp(rotForce, -Params::MAX_TURN_RATE, Params::MAX_TURN_RATE);

	rotation_ += rotForce;

	speed_ = (lthrust_ + rthrust_);

	//update direction
	dir_.x = -sin(rotation_);
	dir_.y = cos(rotation_);

	//std::cerr << dir_.x << "\t" << dir_.y << "\t" << speed_ << "\t" << loc_.x << "\t" <<  loc_.y << std::endl;
	//update location
	loc_ += (dir_ * speed_);
}

Tank Tank::makeChild() {
	return Tank(this->teamID_, this->loc_, this->rotation_);
}

Tank Tank::clone() {
	//make child
	Tank t = makeChild();

	//copy brain
	for(size_t i = 0; i < brain_.size(); ++i) {
		t.brain_.weights()[i] = brain_.weights()[i];
	}
	return t;
}

void Tank::resetGameState() {
	ammonition_ = Params::MAX_PROJECTILES;
	lthrust_ = 0;
	rthrust_ = 0;
	dead_ = false;
	explode_ = false;
}

void Tank::resetScore() {
	friendly_fire_ = 0;
	hits_ = 0;
	damage_ = 0;
	fitness_ = 0;
}
} /* namespace tankwar */
