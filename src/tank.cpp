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
		brain_(brain),
		scan_(*this) {
	resetGameState();
}


void Tank::setBrain(Brain* b) {
	assert(brain_ == NULL);
	brain_ = b;
}

void Tank::calculateFitness() {
	Coord totalDiff = 0;
	size_t ratedProjectiles = 0;

	if (!layout_.disableProjectileFitness_) {
		for (Projectile* p : projectiles_) {
			if(p->scan_.objects_.empty()) {
				++ratedProjectiles;
				totalDiff += 1;
				continue;
			}

			assert(p->scan_.objects_.size() == 2);
			assert(p->scan_.objects_[0].type_ == ENEMY);
			assert(p->scan_.objects_[1].type_ == FRIEND);

			for(ScanObject& so : p->scan_.objects_) {
				if(so.type_ == ScanObjectType::ENEMY) {
					Coord angDistPerfect = 0;
					Vector2D perfect = (so.loc_ - p->startLoc_).normalize();
					Vector2D candidate = (p->loc_ - p->startLoc_).normalize();
					Vector2D vdiff = candidate;

					ASSERT_DIR(perfect);
					ASSERT_DIR(candidate);

					vdiff.rotate(perfect);
					angDistPerfect = fabs(radFromDir(vdiff)) / M_PI;

					assert(angDistPerfect >= 0);
					assert(angDistPerfect <= 1);
					Coord distance = so.dis_;
					Coord maxDistance = p->layout_.max_travel_ * 4;
					if(distance > maxDistance)
						distance = maxDistance;

					//(higher distance -> higher diff) * (times angular distance)
					Coord distPerfect = (distance / maxDistance);
					angDistPerfect =  distPerfect / (angDistPerfect/1 + 1);

					assert(angDistPerfect >= 0);
					assert(angDistPerfect <= 1);
					totalDiff += angDistPerfect;
					++ratedProjectiles;
				} else if(so.type_ == ScanObjectType::FRIEND) {
					Coord angDistWorst = 0;
					Vector2D worst = (so.loc_ - p->startLoc_).normalize();
					Vector2D candidate = (p->loc_ - p->startLoc_).normalize();

					ASSERT_DIR(worst);
					ASSERT_DIR(candidate);

					candidate.rotate(worst);
					angDistWorst = 1 - (fabs(radFromDir(candidate)) / M_PI);
					// a perpendicular friend is a good friend
					//angDistWorst = fabs(M_PI_2 - angDistWorst) / M_PI_2;

					assert(angDistWorst >= 0);
					assert(angDistWorst <= 1);

					totalDiff += angDistWorst;
					++ratedProjectiles;
				}
			}
		}
	}

	if(projectiles_.empty()) {
		fitness_ = 0;
	} else {
		assert(ratedProjectiles > 0);
		assert(projectiles_.size() <= layout_.max_ammo_);
		// don't remove the assert!
		assert((totalDiff) <= (ratedProjectiles + 0.01));
		//get rid of noise spikes
		if((totalDiff) > ratedProjectiles)
			totalDiff = ratedProjectiles;

		Coord shots = projectiles_.size();
		Coord aimRatio = (1.0 - (totalDiff / (ratedProjectiles)));
		Coord hitRatio = (Coord(hits_) / shots);
		Coord friendlyRatioInv = (1.0 / ((Coord(friendlyFire_) / shots) + 1));
		Coord damageRatioInv = (1.0 / ((Coord(damage_) / layout_.max_damage_) + 1));
		assert(aimRatio >= 0);
		assert(aimRatio <= 1);
		assert(hitRatio >= 0);
		assert(hitRatio <= 1);
		assert(damageRatioInv >= 0.5);
		assert(damageRatioInv <= 1);
		assert(friendlyRatioInv >= 0.5);
		assert(friendlyRatioInv <= 1);

		fitness_ = (aimRatio + ((hitRatio * damageRatioInv))) * friendlyRatioInv;
	}

	assert(fitness_ >= 0);
	assert(fitness_ <= 2);
	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
	//std::cerr << "fitness:" << fitness_ << std::endl;
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

void Tank::damage() {
	damage_++;
	if (damage_ >= layout_.max_damage_) {
		death();
	}
}

void Tank::death() {
	damage_ = layout_.max_damage_;
	dead_ = true;
	explode_ = true;
}

// demote and execute this unit
void Tank::kill() {
	friendlyFire_ = layout_.max_ammo_;
	hits_ = 0;
	death();
}

void Tank::crash() {
	crash_++;
	crashDamage_++;

	if(crashDamage_ >= layout_.crashes_per_damage_) {
		crashDamage_ = 0;
		damage();
	}
}

void Tank::impact(Tank& other) {
	crash();
	other.crash();
}

void Tank::impact(Projectile& p) {
	p.death();
	damage();

	if (p.owner_->teamID_ != teamID_) {
		p.owner_->hits_++;
	} else {
		p.owner_->friendlyFire_++;
	}
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
	crashDamage_ = 0;
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
