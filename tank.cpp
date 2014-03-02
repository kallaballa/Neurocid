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

namespace tankwar {

Tank::Tank(Brain_ptr bptr, size_t teamID, Vector2D loc, Vector2D dir) : Object(loc, dir, 0, Params::TANK_RANGE, Params::MAX_TANK_SPEED, false, false), brain_(bptr), teamID_(teamID){
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void Tank::think(BattleField& field) {
	if(teamID_ == 0)
		brain_->update(*this, field.teamA_, field.teamB_);
	else
		brain_->update(*this, field.teamB_, field.teamA_);
}

void Tank::move() {
	//assign the outputs
	lthrust_ = brain_->lthrust_;
	rthrust_ = brain_->rthrust_;
	wantsShoot_ = (brain_->shoot_ > 0);

	std::cerr << brain_->lthrust_ << "\t" << brain_->rthrust_ << "\t" << brain_->shoot_ << std::endl;

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
} /* namespace tankwar */
