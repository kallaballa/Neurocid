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

Tank::Tank(size_t teamID, TankLayout tl) :
		Object({0,0}, 0, tl.range_, tl.max_speed_, false, false),
		teamID_(teamID),
		tl_(tl),
		brain_(NULL) {
	resetGameState();
}

void Tank::setBrain(Brain* b) {
	assert(brain_ == NULL);
	brain_ = b;
}

std::vector<Vector2D> Tank::scan(BattleFieldLayout& bfl) {
	Vector2D min(0,0);

	if(scanner_.nearestEnemyLoc_ == NO_VECTOR2D) {
			scanner_.nearestFriendLoc_ = Vector2D(0,0);
	}

	if(scanner_.nearestFriendLoc_ == NO_VECTOR2D) {
		scanner_.nearestFriendLoc_ = Vector2D(0,0);
	}

	if(scanner_.nearestFriend2Loc_ == NO_VECTOR2D) {
		scanner_.nearestFriend2Loc_ = Vector2D(0,0);
	}

	ASSERT_LOC(loc_);

	Coord w = bfl.width_;
	Coord h = bfl.height_;

	//std::cerr << "see: " <<  loc_ << " " << scanner_.nearestEnemyLoc_ << " " << scanner_.nearestFriendLoc_ << " " << scanner_.nearestFriend2Loc_ << " " << std::endl;

	Vector2D reference = getDirection();
	Vector2D toNearestEnemy = (scanner_.nearestEnemyLoc_ - loc_).normalize(w,h);
	Vector2D toNearestFriend = (scanner_.nearestFriendLoc_ - loc_).normalize(w,h);
	Vector2D toNearestFriend2 = (scanner_.nearestFriend2Loc_ - loc_).normalize(w,h);

//	if(teamID_ == 0)
//		std::cerr << "see: " <<  reference << "\t" << toNearestEnemy << std::endl;
	return {reference, toNearestEnemy, toNearestFriend, toNearestFriend2};
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;

	if (projectiles_.empty()) {
		totalDiff =  tl_.max_ammo_ * M_PI;
	} else {
		for (Projectile& p : projectiles_) {
			if(p.nearestEnemyLoc_ == NO_VECTOR2D) {
				continue;
			}

			if(p.nearestFriendLoc_ == NO_VECTOR2D) {
				totalDiff += M_PI;
				continue;
			}

			Coord diff = 0;
			Coord diffPerfect = 0;
			Coord diffWorst = 0;
			Vector2D perfect = (p.nearestEnemyLoc_ - p.startLoc_).normalize();
			Vector2D worst = (p.nearestFriendLoc_ - p.startLoc_).normalize();
			Vector2D candidate = (p.loc_ - p.startLoc_).normalize();

			ASSERT_DIR(perfect);
		    //FIXME we need collision detection
			//ASSERT_DIR(worst);
			ASSERT_DIR(candidate);

			diffPerfect = fabs(
					(M_PI + radFromDir(perfect))
							- (M_PI + radFromDir(candidate)));
			diffWorst = fabs(
					(M_PI + radFromDir(worst))
							- (M_PI + radFromDir(worst)));

			if (diffPerfect > M_PI)
				diffPerfect = M_PI - (diffPerfect - M_PI);

			if (diffWorst > M_PI)
				diffWorst = M_PI - (diffWorst - M_PI);

			//std::cerr << diffPerfect << " c:" << candidate << " p:" << perfect << " e:" << p.nearestEnemyLoc_ <<  std::endl;

			assert(diffPerfect >= 0);
			assert(diffWorst >= 0);
			assert(diffPerfect <= M_PI);
			assert(diffWorst <= M_PI);

			//diff = ((M_PI + diffWorst) - diffPerfect) / 2;

			diff = diffPerfect;
			//	}

			assert(diff >= 0);
			assert(diff <= M_PI);
			totalDiff += diff;
			//		std::cerr << "perfect: ("<< perfect.x << "," << perfect.y << ")" << "candidate: ("<< candidate.x << "," << candidate.y << ")" << "diff: " << diff << std::endl;
		}
	}

	assert(projectiles_.size() <= tl_.max_ammo_);

	double aimRatio = (M_PI - (totalDiff / tl_.max_ammo_)) / M_PI;
	double hitRatio = (double(hits_) / tl_.max_ammo_);
	double damageRatioInv = (1.0 / ((double(damage_) / tl_.max_damage_) + 1));

	//std::cerr << "aim:" << aimRatio << "\thit:" << hitRatio << "/" << hits_ << "\tdmg:" << damageRatioInv << "/" << damage_ << "\tammo:" << tl_.max_ammo_- projectiles_.size() << std::endl;
	fitness_ = aimRatio + (hitRatio * damageRatioInv);
	assert(fitness_ >= 0);
	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
	//std::cerr << "fitness:" << fitness_ << std::endl;

//	fitness_ = hits_;
//	fitness_ = Params::MAX_PROJECTILES - friendly_fire_;
}

void Tank::think(BattleFieldLayout& bfl) {
	assert(brain_ != NULL);
	brain_->update(this->scan(bfl));
}

void Tank::move(BattleFieldLayout& bfl) {
	assert(brain_ != NULL);
	//assign the outputs
	lthrust_ = brain_->lthrust_;
	rthrust_ = brain_->rthrust_;

	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(brain_->shoot_));

	bool canShoot = tl_.canShoot_ && (cool_down == 0 && ammonition_ > 0);
	bool wantsShoot = (brain_->shoot_ > 0.0);

	if(canShoot && wantsShoot) {
		this->shoot();
		cool_down = tl_.max_cooldown;
	} else if(cool_down > 0){
		--cool_down;
	}

	speed_ = ((lthrust_ + rthrust_) / 2) * tl_.max_speed_;

	//update rotation
	if(tl_.canRotate_) {
		//calculate steering forces
		Coord rotForce = lthrust_ - rthrust_;
		clamp(rotForce, -tl_.max_rotation_, tl_.max_rotation_);
		rotation_ += rotForce;
		rotation_ = normRotation(rotation_);
	}

	//update location
	if(tl_.canMove_) {
		loc_ += (getDirection() * speed_);
		Coord maxX = bfl.width_;
		Coord maxY = bfl.height_;

		if(loc_.x < 0)
			loc_.x = 0;
		else if(loc_.x > maxX)
			loc_.x = maxX;

		if(loc_.y < 0)
			loc_.y = 0;
		else if(loc_.y > maxY)
			loc_.y = maxY;
	}

	//std::cerr << dir_.x << "\t" << dir_.y << "\t" << speed_ << "\t" << loc_.x << "\t" <<  loc_.y << std::endl;
}

Tank Tank::makeChild() {
	assert(brain_ != NULL);
	Tank child(teamID_, tl_);
	Brain* fresh  = new Brain(brain_->layout_);
	child.setBrain(fresh);
	return child;
}

Tank Tank::clone() {
	assert(brain_ != NULL);
	Tank child(teamID_, tl_);
	Brain* fresh  = new Brain(brain_->layout_);
	child.setBrain(fresh);

	//copy brain
	for(size_t i = 0; i < brain_->size(); ++i) {
		child.brain_->weights()[i] = brain_->weights()[i];
	}
	return child;
}

void Tank::resetGameState() {
	ammonition_ = tl_.max_ammo_;
	projectiles_.clear();
	lthrust_ = 0;
	rthrust_ = 0;
	dead_ = false;
	explode_ = false;
	cool_down = 0;
}

void Tank::resetScore() {
	friendly_fire_ = 0;
	hits_ = 0;
	damage_ = 0;
	fitness_ = 0;
}

void Tank::update(TankLayout tl) {
	this->tl_ = tl;
	resetGameState();
}

bool Tank::operator==(const Tank& other) const {
	return this->loc_ == other.loc_ && this->teamID_ == other.teamID_;
}

bool Tank::operator!=(const Tank& other) const {
	return !this->operator ==(other);
}

bool Tank::operator<(const Tank& other) const {
	return (this->fitness_ < other.fitness_);
}

Projectile& Tank::shoot() {
	assert(ammonition_ > 0);
	--ammonition_;
	projectiles_.push_back(Projectile(*this, loc_, rotation_));
	return *(projectiles_.end() - 1);
}

} /* namespace tankwar */
