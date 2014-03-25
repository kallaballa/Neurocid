/*
 * Tank.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#include "tank.hpp"
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

Tank::Tank(size_t teamID, TankLayout tl, Brain* brain) :
		Object(TANK, {0,0}, 0, tl.range_, false, false),
		teamID_(teamID),
		layout_(tl),
		brain_(brain) {
	resetGameState();
}


void Tank::setBrain(Brain* b) {
	assert(brain_ == NULL);
	brain_ = b;
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;

	if(layout_.disableProjectileFitness_) {
		totalDiff = 0;
	} else if (projectiles_.empty()) {
		totalDiff =  M_PI;
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
					Coord distance = so.dis_;
					Coord maxDistance = p->layout_.max_travel_ * 2;
					if(distance > maxDistance)
						distance = maxDistance;

					diffPerfect *= ((distance / maxDistance) + 1);
					if(diffPerfect > M_PI)
						diffPerfect = M_PI;

					totalDiff += diffPerfect;
					//		std::cerr << "perfect: ("<< perfect.x << "," << perfect.y << ")" << "candidate: ("<< candidate.x << "," << candidate.y << ")" << "diff: " << diff << std::endl;
				} else if(so.type_ == ScanObjectType::FRIEND) {
					Coord diffWorst = 0;
					Vector2D worst = (so.loc_ - p->startLoc_).normalize();
					Vector2D candidate = (p->loc_ - p->startLoc_).normalize();

					ASSERT_DIR(worst);
					ASSERT_DIR(candidate);

					diffWorst = fabs(
							(M_PI + radFromDir(worst))
									- (M_PI + radFromDir(candidate)));

					if (diffWorst > M_PI)
						diffWorst = M_PI - (diffWorst - M_PI);

					// a perpendicular friend is a good friend
					diffWorst = fabs(M_PI/2 - diffWorst) * 2;

					//std::cerr << diffPerfect << " c:" << candidate << " p:" << perfect << " e:" << p.nearestEnemyLoc_ <<  std::endl;

					assert(diffWorst >= 0);
					assert(diffWorst <= M_PI);

					totalDiff += diffWorst;
					//		std::cerr << "perfect: ("<< perfect.x << "," << perfect.y << ")" << "candidate: ("<< candidate.x << "," << candidate.y << ")" << "diff: " << diff << std::endl;
				}
			}
		}
	}

	assert(projectiles_.size() <= layout_.max_ammo_);

	Coord aimRatio = (M_PI - (totalDiff / (projectiles_.size() + 1))) / M_PI;
	Coord hitRatio = (Coord(hits_) / layout_.max_ammo_);
	Coord friendlyRatioInv = (1.0 / ((Coord(friendlyFire_) / layout_.max_ammo_) + 1));
	Coord damageRatioInv = (1.0 / ((Coord(damage_) / layout_.max_damage_) + 1));

	//std::cerr << "aim:" << aimRatio << "\thit:" << hitRatio << "/" << hits_ << "\tdmg:" << damageRatioInv << "/" << damage_ << "\tammo:" << tl_.max_ammo_- projectiles_.size() << std::endl;
	fitness_ = (aimRatio + ((hitRatio * damageRatioInv)));

	assert(fitness_ >= 0);
	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
	//std::cerr << "fitness:" << fitness_ << std::endl;

//	fitness_ = hits_;
//	fitness_ = Params::MAX_PROJECTILES - friendly_fire_;
}

void Tank::think(BattleFieldLayout& bfl) {
	if(layout_.isDummy_)
		return;

	assert(brain_ != NULL);
	brain_->update(bfl, this->scan_);
}

void Tank::move(BattleFieldLayout& bfl) {
	if(layout_.isDummy_)
		return;

	assert(brain_ != NULL);
	//assign the outputs
	lthrust_ = brain_->lthrust_;
	rthrust_ = brain_->rthrust_;

	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(brain_->shoot_));

	bool canShoot = layout_.canShoot_ && (cool_down == 0 && ammonition_ > 0);
	bool wantsShoot = (brain_->shoot_ > 0.0);

	if(canShoot && wantsShoot) {
		willShoot_ = true;
	} else if(cool_down > 0){
		willShoot_ = false;
		--cool_down;
	}

	//update location
	if(layout_.canMove_)
		speed_ = ((lthrust_ + rthrust_) / 2) * layout_.max_speed_;
	else
		speed_ = 0;


	//update rotation force
	if(layout_.canRotate_) {
		//calculate steering forces
		Coord rotForce = lthrust_ - rthrust_;
		clamp(rotForce, -layout_.max_rotation_, layout_.max_rotation_);
		rotForce_ = rotForce;
	} else
		rotForce_ = 0;

	//std::cerr << "canMove: " << tl_.canMove_ << "\tcanRotate: " << tl_.canRotate_ << "\tspeed: " << speed_ << "\trotForce:" << rotForce_  << std::endl;
}

Tank Tank::makeChild() {
	assert(brain_ != NULL);
	Tank child(teamID_, layout_);
	Brain* fresh  = new Brain(brain_->layout_);
	child.setBrain(fresh);
	return child;
}

Tank Tank::clone() {
	assert(brain_ != NULL);
	Tank child(teamID_, layout_);
	Brain* fresh  = new Brain(brain_->layout_);
	child.setBrain(fresh);

	//copy brain
	for(size_t i = 0; i < brain_->size(); ++i) {
		child.brain_->weights()[i] = brain_->weights()[i];
	}
	return child;
}

void Tank::resetGameState() {
	ammonition_ = layout_.max_ammo_;
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
	friendlyFire_ = 0;
	crash_ = 0;
	hits_ = 0;
	damage_ = 0;
	fitness_ = 0;
}

void Tank::update(TankLayout tl) {
	this->layout_ = tl;
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
	cool_down = layout_.max_cooldown;
	Vector2D loc = loc_;
	loc += (getDirection() * range_);
	Projectile* p = new Projectile(*this,layout_.pl_, loc, rotation_);
	projectiles_.push_back(p);
	return p;
}

} /* namespace tankwar */
