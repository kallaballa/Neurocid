#include "ship.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include <cstdlib>
#include <iostream>
#include "util.hpp"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>

namespace neurocid {

Ship::Ship(size_t teamID, ShipLayout tl, Brain* brain) :
		Object(SHIP, {0,0}, 0, tl.radius_, false, false, false),
		teamID_(teamID),
		layout_(tl),
		brain_(brain),
		scan_(this),
		ammo_(tl.startAmmo_){
	resetGameState();
}

void Ship::setBrain(Brain* b) {
	assert(brain_ == NULL);
	brain_ = b;
}

void Ship::calculateFitness() {
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
					Coord maxDistance = 600000;
					if(distance > maxDistance)
						distance = maxDistance;

					//(higher distance -> higher diff)
					Coord distPerfect = (distance / maxDistance);

					// euler distance matters more then angular distance does.
					//http://tinyurl.com/kyhbu7c
					Coord diffPerfect = (std::sqrt(distPerfect) * (angDistPerfect + 1)) / 2;

					assert(diffPerfect >= 0);
					assert(diffPerfect <= 1);

					//boost in range shots
					if(so.dis_ < p->layout_.max_travel_)
						diffPerfect /= 2;

					totalDiff += diffPerfect;
					++ratedProjectiles;
				} else if(so.type_ == ScanObjectType::FRIEND) {
					continue;
					Coord angDistWorst = 0;
					Vector2D worst = (so.loc_ - p->startLoc_).normalize();
					Vector2D candidate = (p->loc_ - p->startLoc_).normalize();
					Vector2D vdiff = candidate;

					ASSERT_DIR(worst);
					ASSERT_DIR(candidate);

					vdiff.rotate(worst);
					// a perpendicular friend is a good friend
					angDistWorst = fabs(radFromDir(vdiff)) / M_PI;

					assert(angDistWorst >= 0);
					assert(angDistWorst <= 1);
					// for rating friends use the distance of the projectile end loc and the friend loc at closest point
					Coord distance = so.dis_;
					Coord maxDistance = 600000;
					if(distance > maxDistance)
						distance = maxDistance;

					//(higher distance -> higher diff)
					Coord distWorst = (distance / maxDistance);

					// euclidian distance matters more then angular distance does
					// http://tinyurl.com/kyhbu7c
					Coord diffWorst = 1.0 - (std::sqrt(distWorst) * (angDistWorst + 1)) / 2;

					assert(diffWorst >= 0);
					assert(diffWorst <= 1);

					//don't give an additional penalty for being in range because that's very likely in a swarm

					totalDiff += diffWorst;
					++ratedProjectiles;
				}
			}
		}
	}

	assert(layout_.numPerfDesc == 4);
	if(projectiles_.empty()) {
		fitness_ = 0;
		perfDesc_.reserve(layout_.numPerfDesc);
		std::fill(perfDesc_.begin(), perfDesc_.end(), 0);
	} else {
		// calulate projectile/aim fitness
		assert(ratedProjectiles > 0 || layout_.disableProjectileFitness_);
		Coord aimRatio = 0;
		if(!layout_.disableProjectileFitness_) {
			aimRatio = (1.0 - (totalDiff / (ratedProjectiles)));
			assert(aimRatio >= 0);
			assert(aimRatio <= 1);
		}

		// don't remove the assert!
		assert((totalDiff) <= (ratedProjectiles + 0.01));
		//get rid of noise spikes
		if((totalDiff) > ratedProjectiles)
			totalDiff = ratedProjectiles;

		// calculate actual hit/damage/friendly_fire score
		Coord shots = projectiles_.size();
		Coord failRatio = (failedShots_/shots);
		Coord hitRatio = (Coord(hits_) / shots);

		//friendly fire may cancel hit ratio completely in the worst case.
		Coord friendlyRatioInv = 1.0 - (Coord(friendlyFire_) / shots);

		//damage may half hit ratio in the worst case
		Coord damageRatioInv = 1.0 / ((Coord(damage_) / layout_.maxDamage_) + 1);
		assert(hitRatio >= 0);
		assert(hitRatio <= 1);
		assert(damageRatioInv >= 0.5);
		assert(damageRatioInv <= 1);
		assert(friendlyRatioInv >= 0);
		assert(friendlyRatioInv <= 1);

		perfDesc_.reserve(layout_.numPerfDesc);
		perfDesc_[0] = aimRatio;
		perfDesc_[1] = hitRatio;
		perfDesc_[2] = friendlyRatioInv;
		perfDesc_[3] = damageRatioInv;

		fitness_ = (((pow(aimRatio,2) * 6) / (failRatio + 1))
				+ ((hits_/2) * damageRatioInv * friendlyRatioInv)) * (((recharged_/layout_.maxFuel_)/3) + 1);

		if(dead_)
			fitness_ /= 2;

		if(fuel_ <= 0)
			fitness_ /= 4;
	}

	assert(!std::isnan(fitness_));
	assert(!std::isinf(fitness_));	//
	//std::cerr << "fitness:" << fitness_ << std::endl;
}

void Ship::think(BattleFieldLayout& bfl) {
	if(layout_.isDummy_)
		return;

	assert(brain_ != NULL);
	brain_->update(bfl, this->scan_);
	brain_->run();
}

void Ship::move(BattleFieldLayout& bfl) {
	if(layout_.isDummy_)
		return;

	assert(brain_ != NULL);
	//assign the outputs
	flthrust_ = brain_->lthrust_;
	frthrust_ = brain_->rthrust_;
	blthrust_ = brain_->fthrust_;
	brthrust_ = brain_->bthrust_;

	bool canShoot = layout_.canShoot_ && (cool_down == 0 && ammo_ > 0);
	bool wantsShoot = (brain_->shoot_ > 0.0);
	if(canShoot && wantsShoot) {
		willShoot_ = true;
	} else if(cool_down > 0){
		if(wantsShoot)
			++failedShots_;

		willShoot_ = false;
		--cool_down;
	}

	//std::cerr << "canMove: " << tl_.canMove_ << "\tcanRotate: " << tl_.canRotate_ << "\tspeed: " << speed_ << "\trotForce:" << rotForce_  << std::endl;
}

void Ship::damage() {
	damage_++;
	if (damage_ >= layout_.maxDamage_) {
		death();
	}
}

void Ship::death() {
	damage_ = layout_.maxDamage_;
	dead_ = true;
	explode_ = true;
}

// demote and execute this unit
void Ship::kill() {
	projectiles_.clear();
	scan_.objects_.clear();
	hits_ = 0;
	recharged_ = 0;
	fuel_ = 0;
	death();
}

void Ship::crash() {
	crash_++;
	crashDamage_++;
	crashed_ = true;

	if(crashDamage_ >= layout_.crashesPerDamage_) {
		crashDamage_ = 0;
		damage();
	}
}

void Ship::impact(Ship& other) {
	crash();
	other.crash();
}

void Ship::impact(Projectile& p) {
	p.death();
	damage();

	if (p.owner_->teamID_ != teamID_) {
		p.owner_->hits_++;
	} else {
		p.owner_->friendlyFire_++;
	}
}

void Ship::recharge() {
	Coord amount = (layout_.maxFuel_ - fuel_);
	assert(amount >= 0);
	recharged_ += amount;
	fuel_ = layout_.maxFuel_;
	ammo_ = layout_.maxAmmo_;
}

void Ship::capture() {
	++captured_;
}

Ship Ship::makeChild() const {
	assert(brain_ != NULL);
	Ship child(teamID_, layout_);
	Brain* fresh  = new Brain();
	fresh->initialize(brain_->layout_);
	child.setBrain(fresh);
	return child;
}

Ship Ship::clone() const {
	assert(brain_ != NULL);
	Ship child(teamID_, layout_);
	Brain* fresh  = new Brain();
	fresh->initialize(brain_->layout_);
	child.setBrain(fresh);

	//copy brain
	for(size_t b = 0; b < brain_->layout_.numBrains_ + 1; ++b) {
		for(size_t i = 0; i < brain_->size(b); ++i) {
			child.brain_->weights(b)[i] = brain_->weights(b)[i];
		}
	}
	return child;
}

void Ship::resetGameState() {
	for(Projectile* p : projectiles_) {
		delete p;
	}
	projectiles_.clear();
	flthrust_ = 0;
	frthrust_ = 0;
	blthrust_ = 0;
	brthrust_ = 0;
	fuel_ = layout_.startFuel_;
	ammo_ = layout_.startAmmo_;
	dead_ = false;
	explode_ = false;
	cool_down = 0;
	willShoot_ = false;
	crashed_ = false;
}

void Ship::resetScore() {
	friendlyFire_ = 0;
	crash_ = 0;
	crashDamage_ = 0;
	hits_ = 0;
	damage_ = 0;
	fitness_ = 0;
	recharged_ = 0;
	failedShots_ = 0;
	captured_ = 0;
}

void Ship::update(ShipLayout tl) {
	this->layout_ = tl;
	resetGameState();
}

bool Ship::operator==(const Ship& other) const {
	return this->loc_ == other.loc_ && this->teamID_ == other.teamID_;
}

bool Ship::operator!=(const Ship& other) const {
	return !this->operator ==(other);
}

bool Ship::operator<(const Ship& other) const {
	return (this->fitness_ < other.fitness_);
}

bool Ship::willShoot() {
	return willShoot_;
}

Projectile* Ship::shoot() {
	assert(cool_down == 0);
	assert(ammo_ > 0);
	--ammo_;

	cool_down = layout_.maxCooldown_;
	Vector2D loc = loc_;
	loc += (getDirection() * radius_);
	Projectile* p = new Projectile(*this,layout_.pl_, loc, rotation_);
	projectiles_.push_back(p);
	return p;
}

} /* namespace neurocid */
