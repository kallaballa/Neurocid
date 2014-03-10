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
		teamID_(teamID),
		lastLoc_(loc){
	setRotation(rotation);
}

std::vector<Vector2D> Tank::scan() const {
	Coord max = std::numeric_limits<Coord>().max();
	assert(scanner_.nearestEnemyLoc_ != Vector2D(max,max));
	assert(scanner_.nearestEnemyLoc_ != Vector2D(0,0));

	assert(scanner_.nearestFriendLoc_ != Vector2D(max,max));
	assert(scanner_.nearestFriendLoc_ != Vector2D(0,0));

	assert(scanner_.nearestFriend2Loc_ != Vector2D(max,max));
	assert(scanner_.nearestFriend2Loc_ != Vector2D(0,0));

	Vector2D reference = dir_;
	Vector2D toNearestEnemy = (scanner_.nearestEnemyLoc_ - loc_).normalize();
	Vector2D toNearestFriend = (scanner_.nearestFriendLoc_ - loc_).normalize();
	Vector2D toNearestFriend2 = (scanner_.nearestFriend2Loc_ - loc_).normalize();

	std::cerr << "see: " << reference << "\t" << toNearestEnemy << "\t" << toNearestFriend << "\t" << toNearestFriend2 << std::endl;

	return {reference, toNearestEnemy, toNearestFriend, toNearestFriend2};
}

void Tank::setDirection(Vector2D dir) {
	dir_= dir;
	assert(dir_.x >= -1 && dir_.x <= 1 && dir_.y >= -1 && dir_.y <= 1);
	assert(! (dir_.x == 0 && dir_.y == 0));

	rotation_ = rotationFromDirection(dir);
}

void Tank::setRotation(double r) {
	rotation_ = normRotation(r);
	dir_= directionFromRotation(rotation_);
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;

	if(projectiles_.empty()) {
		totalDiff = Params::MAX_AMMO * M_PI;
	} else {
		for(Projectile& p : projectiles_) {
			Coord diff = 0;
			Coord diffPerfect = 0;
			Coord diffWorst = 0;
	/*		if(p.friendHitter_) {
				diff = M_PI;
			} else if(p.enemyHitter_) {
				diff = 0;
			} else {*/
				Vector2D perfect = (p.nearestEnemyLoc_ - p.startLoc_).normalize();
				//Vector2D worst = (p.nearestFriendLoc_ - p.startLoc_).normalize();
				Vector2D candidate = (p.loc_ - p.startLoc_).normalize();

				//assert(p.nearestFriendLoc_ != Vector2D(0,0));
				assert(p.nearestEnemyLoc_ != Vector2D(0,0));
				assert(perfect != Vector2D(0,0));
				//assert(worst != Vector2D(0,0));
				assert(candidate != Vector2D(0,0));

				diffPerfect = fabs((M_PI + rotationFromDirection(perfect)) - (M_PI + rotationFromDirection(candidate)));
//				diffWorst = fabs((M_PI + rotationFromDirection(worst)) - (M_PI + rotationFromDirection(candidate))) / 2;

				if(diffPerfect > M_PI)
					diffPerfect = M_PI - (diffPerfect - M_PI);

				//std::cerr << diffPerfect << " c:" << candidate << " p:" << perfect << " e:" << p.nearestEnemyLoc_ <<  std::endl;

				assert(diffPerfect >= 0);
				//assert(diffWorst >= 0);
				assert(diffPerfect <= M_PI);
				//assert(diffWorst <= M_PI);

				//diff = ((M_PI + diffWorst) - diffPerfect) / 2;

				diff = diffPerfect;
		//	}

			assert(diff >= 0);
			assert(diff <= M_PI);
			totalDiff += diff;
	//		std::cerr << "perfect: ("<< perfect.x << "," << perfect.y << ")" << "candidate: ("<< candidate.x << "," << candidate.y << ")" << "diff: " << diff << std::endl;
		}
	}

	assert(projectiles_.size() <= Params::MAX_AMMO);


	double aimRatio = (M_PI - (totalDiff/ Params::MAX_AMMO)) / M_PI;
	double hitRatio = (double(hits_)/Params::MAX_AMMO);
	double damageRatioInv = (1.0 /((double(damage_)/ Params::MAX_DAMAGE)+1));

	//std::cerr << "aim:" << aimRatio << "\thit:" << hitRatio << "/" << hits_ << "\tdmg:" << damageRatioInv << "/" << damage_ << "\tammo:" << Params::MAX_AMMO - projectiles_.size() << std::endl;
	fitness_ = aimRatio + (hitRatio * damageRatioInv);

	//std::cerr << "fitness:" << fitness_ << std::endl;
	//std::cerr << "f:" << fitness_ << std::endl;
	assert(fitness_ >= 0);
	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
//	fitness_ = hits_;
//	fitness_ = Params::MAX_PROJECTILES - friendly_fire_;
}

void Tank::think(BattleField& field) {
	brain_.update(this->scan());
}

void Tank::move() {
	//assign the outputs
	lthrust_ = brain_.lthrust_;
	rthrust_ = brain_.rthrust_;

	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(brain_.shoot_));

	bool canShoot = (cool_down == 0 && ammonition_ > 0);
	bool wantsShoot = (brain_.shoot_ > 0.0);

	if(canShoot && wantsShoot) {
		this->shoot();
		cool_down = Params::COOL_DOWN;
	} else if(cool_down > 0){
		--cool_down;
	}

	//std::cerr << brain_.shoot_ << std::endl;

	//calculate steering forces
	Coord rotForce = lthrust_ - rthrust_;

	//clamp rotation
	clamp(rotForce, -Params::MAX_TURN_RATE, Params::MAX_TURN_RATE);

	rotation_ += rotForce;

	speed_ = ((lthrust_ + rthrust_) / 2) * Params::MAX_TANK_SPEED;

	setRotation(rotation_);

	//std::cerr << dir_.x << "\t" << dir_.y << "\t" << speed_ << "\t" << loc_.x << "\t" <<  loc_.y << std::endl;

	lastLoc_ = loc_;
	//update location
	loc_ += (dir_ * speed_);
}

void Tank::stepBack() {
	loc_ = lastLoc_;
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
	ammonition_ = Params::MAX_AMMO;
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
