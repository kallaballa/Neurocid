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
		Object(TANK, {0,0}, 0, tl.range_, false, false),
		teamID_(teamID),
		tl_(tl),
		brain_(NULL) {
	resetGameState();
}


void Tank::setBrain(Brain* b) {
	assert(brain_ == NULL);
	brain_ = b;
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;

	if (projectiles_.empty()) {
		totalDiff =  tl_.max_ammo_ * M_PI;
	} else {
		for (Projectile* p : projectiles_) {
			if(!p->scan_.objects_.empty()) {
				ScanObject & so = p->scan_.objects_[0];
				if(so.type_ == ScanObjectType::ENEMY) {
					Coord diffPerfect = 0;
					Vector2D perfect = (so.loc_ - p->startLoc_).normalize();
					Vector2D candidate = (p->loc_ - p->startLoc_).normalize();

					ASSERT_DIR(perfect);
					ASSERT_DIR(candidate);

					diffPerfect = fabs(
							(M_PI + radFromDir(perfect))
									- (M_PI + radFromDir(candidate)));

					if (diffPerfect > M_PI)
						diffPerfect = M_PI - (diffPerfect - M_PI);

					//std::cerr << diffPerfect << " c:" << candidate << " p:" << perfect << " e:" << p.nearestEnemyLoc_ <<  std::endl;

					assert(diffPerfect >= 0);
					assert(diffPerfect <= M_PI);

					totalDiff += diffPerfect;
					//		std::cerr << "perfect: ("<< perfect.x << "," << perfect.y << ")" << "candidate: ("<< candidate.x << "," << candidate.y << ")" << "diff: " << diff << std::endl;
				}
			}
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
	brain_->update(this->scan_);
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
		willShoot_ = true;
	} else if(cool_down > 0){
		willShoot_ = false;
		--cool_down;
	}

	//update location
	if(tl_.canMove_)
		speed_ = ((lthrust_ + rthrust_) / 2) * tl_.max_speed_;
	else
		speed_ = 0;


	//update rotation force
	if(tl_.canRotate_) {
		//calculate steering forces
		Coord rotForce = lthrust_ - rthrust_;
		clamp(rotForce, -tl_.max_rotation_, tl_.max_rotation_);
		rotForce_ = rotForce;
	} else
		rotForce_ = 0;

	//std::cerr << "canMove: " << tl_.canMove_ << "\tcanRotate: " << tl_.canRotate_ << "\tspeed: " << speed_ << "\trotForce:" << rotForce_  << std::endl;
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
	for(Projectile* p : projectiles_) {
		delete p;
	}
	projectiles_.clear();
	lthrust_ = 0;
	rthrust_ = 0;
	dead_ = false;
	explode_ = false;
	cool_down = 0;
	willShoot_ = false;
	speed_ = 0;
	rotForce_ = 0;
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

bool Tank::willShoot() {
	return willShoot_;
}

Projectile* Tank::shoot() {
	assert(cool_down == 0);
	assert(ammonition_ > 0);
	--ammonition_;
	cool_down = tl_.max_cooldown;
	Vector2D loc = loc_;
	loc += (getDirection() * range_);
	Projectile* p = new Projectile(*this, loc, rotation_);
	projectiles_.push_back(p);
	return p;
}

} /* namespace tankwar */
