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

Tank::Tank(size_t teamID, BrainLayout layout, Vector2D loc, Coord rotation) :
		Object(loc, rotation, Params::TANK_RANGE, Params::MAX_TANK_SPEED, false, false),
		brain_(layout),
		teamID_(teamID){
	updateDirection();
}

void Tank::updateDirection() {
	dir_.x = -sin(rotation_);
	dir_.y = cos(rotation_);
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;

	for(Projectile& p : projectiles_) {
		Coord diff;
		Coord diffPerfect;
		Coord diffWorst;
		if(p.friendHitter_) {
			diff = M_PI;
		} else if(p.enemyHitter_) {
			diff = 0;
		} else {
			Vector2D perfect = (p.nearestEnemyLoc_ - p.startLoc_).normalize();
			Vector2D worst = (p.nearestFriendLoc_ - p.startLoc_).normalize();
			Vector2D candidate = (p.loc_ - p.startLoc_).normalize();

			assert(perfect != Vector2D(0,0));
			assert(worst != Vector2D(0,0));
			assert(candidate != Vector2D(0,0));
//FIXME use of atan2
			diffPerfect = fabs(fmod((4*M_PI) + atan2(perfect.y, perfect.x) - atan2(candidate.y, candidate.x), M_PI * 2));
			diffWorst = fabs(fmod((4*M_PI) + atan2(worst.y, worst.x) - atan2(candidate.y, candidate.x), M_PI * 2));

			if(diffPerfect > M_PI)
				diffPerfect = (2* M_PI) - diffPerfect;

			if(diffWorst > M_PI)
				diffWorst = (2* M_PI) - diffWorst;

	//		diff = diffPerfect / (diffWorst + 1);
			diff = diffPerfect;
		}

		assert(diff >= 0);
		assert(diff <= M_PI);
		totalDiff += diff;
//		std::cerr << "perfect: ("<< perfect.x << "," << perfect.y << ")" << "candidate: ("<< candidate.x << "," << candidate.y << ")" << "diff: " << diff << std::endl;
	}

	assert(projectiles_.size() <= Params::MAX_PROJECTILES);

	if(projectiles_.empty())
		fitness_=0;
	else
		fitness_ = (
				((M_PI - (totalDiff/ Params::MAX_PROJECTILES)))
		//		+ ((M_PI/2) / ((Params::MAX_DAMAGE - damage_) + 1))
//				+ ((M_PI/3) / ((Params::MAX_PROJECTILES - ammonition_ ) + 1))
		)
//		/ (double)((friendly_fire_ / 3) + 1)
		;

	//std::cerr << "f:" << fitness_ << std::endl;
	assert(fitness_ >= 0);
	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
//	fitness_ = hits_;
//	fitness_ = Params::MAX_PROJECTILES - friendly_fire_;
}

void Tank::think(BattleField& field) {
	assert(field.teams_.size() == 2);
	if(teamID_ == field.teams_[0][0].teamID_)
		brain_.update(*this, field.teams_[0], field.teams_[1]);
	else
		brain_.update(*this, field.teams_[1], field.teams_[0]);
}

void Tank::move() {
	//assign the outputs
	lthrust_ = brain_.lthrust_;
	rthrust_ = brain_.rthrust_;

	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_));
	wantsShoot_ = (brain_.shoot_ > 0);
	//std::cerr << brain_.shoot_ << std::endl;

	//calculate steering forces
	Thrust rotForce = lthrust_ - rthrust_;

	//clamp rotation
	clamp(rotForce, -Params::MAX_TURN_RATE, Params::MAX_TURN_RATE);

	rotation_ += rotForce;

	speed_ = (lthrust_ + rthrust_);

	updateDirection();

	//std::cerr << dir_.x << "\t" << dir_.y << "\t" << speed_ << "\t" << loc_.x << "\t" <<  loc_.y << std::endl;
	//update location
	loc_ += (dir_ * speed_);
}

Tank Tank::makeChild() {
	return Tank(this->teamID_, this->brain_.layout_, this->loc_, this->rotation_);
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
	projectiles_.clear();
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
