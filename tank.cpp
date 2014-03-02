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

namespace tankwar {

Tank::Tank(size_t teamID, Vector2D loc, Vector2D dir) :
		Object(loc, dir, 0, Params::TANK_RANGE, Params::MAX_TANK_SPEED, false, false),
		brain_(),
		teamID_(teamID){
}

void Tank::calculateFitness() {
	if(projectiles_ == Params::MAX_PROJECTILES || friendly_fire_ > 0)
		fitness_=0;
	else
		fitness_ = (double)((hits_ * 3) + (Params::MAX_DAMAGE - damage_) + (Params::MAX_PROJECTILES - projectiles_)) / (double)(((friendly_fire_ * 2) + 1));

	//fitness_ = Params::MAX_PROJECTILES - friendly_fire_;
}

void Tank::think(BattleField& field) {
	if(teamID_ == 0)
		brain_.update(*this, field.teamA_, field.teamB_);
	else
		brain_.update(*this, field.teamB_, field.teamA_);
}

void Tank::move() {
	//assign the outputs
	lthrust_ = brain_.lthrust_;
	rthrust_ = brain_.rthrust_;
	wantsShoot_ = (brain_.shoot_ > 0);

	if(wantsShoot_)
		this->shoot();

	//calculate steering forces
	Thrust rotForce = lthrust_ - rthrust_;

	//clamp rotation
	clamp(rotForce, -Params::MAX_TURN_RATE, Params::MAX_TURN_RATE);

	rotation_ += rotForce;

	speed_ = (lthrust_ + rthrust_);

	//update direction
	dir_.x = -sin(rotation_);
	dir_.y = cos(rotation_);

	//update location
	loc_ += (dir_ * speed_);
}


void Tank::reset() {
	projectiles_ = Params::MAX_PROJECTILES;
	lthrust_ = 0;
	rthrust_ = 0;
	friendly_fire_ = 0;
	hits_ = 0;
	damage_ = 0;
	fitness_ = 0;
	dead_ = false;
	explode_ = false;
}
} /* namespace tankwar */
