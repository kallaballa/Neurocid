#include "ship.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include "../src/lua.hpp"
#include "error.hpp"
#include <cstdlib>
#include <iostream>
#include "util.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>

namespace neurocid {


Ship::Ship(ObjectType type, size_t teamID, ShipLayout tl, Brain* brain) :
    Object(type, {0,0}, 0, tl.radius_, false, false, false),
    teamID_(teamID),
    layout_(tl),
    brain_(brain),
    scan_(this),
    history_(),
    ammo_(tl.startAmmo_){
  resetGameState();
}

Ship::Ship(size_t teamID, ShipLayout tl, Brain* brain) :
		Object(SHIP, {0,0}, 0, tl.radius_, false, false, false),
		teamID_(teamID),
		layout_(tl),
		brain_(brain),
		scan_(this),
		history_(),
		ammo_(tl.startAmmo_){
	resetGameState();
}

void Ship::setBrain(Brain* b) {
	CHECK(brain_ == NULL);
	brain_ = b;
}

void Ship::log() {
	if(!dead_)
		history_.update(*this);
}

void Ship::calculateFitness(const BattleFieldLayout& bfl, Facility& f, bool winner) {
	history_.calculate();
	fitness_ = neurocid::lua::run_fitness_function(layout_.fitnessFunction_,*this, f, bfl, winner);
}

void Ship::think(BattleFieldLayout& bfl) {
	if(layout_.isDummy_)
		return;

	CHECK(brain_ != NULL);
	brain_->update(bfl, this->scan_);
	brain_->run();
}

void Ship::move(BattleFieldLayout& bfl) {
	if(layout_.isDummy_)
		return;

  bool isStunned = stun_ > 0.0;
  bool canJump = layout_.canJump_ && fuel_ > layout_.jumpRate_;
  bool wantsJump = (brain_->jump_ > 0.0);

	CHECK(brain_ != NULL);
	//assign the outputs
	if(!isStunned) {
	  flthrust_ = brain_->lthrust_;
	  frthrust_ = brain_->rthrust_;
	  blthrust_ = brain_->fthrust_;
	  brthrust_ = brain_->bthrust_;
	} else {
	  flthrust_ = 0;
	  frthrust_ = 0;
	  blthrust_ = 0;
	  brthrust_ = 0;
	}

  isJumping_ = canJump && wantsJump && !isStunned;

  if(isJumping_)
    fuel_ -= layout_.jumpRate_;

  bool canShoot = layout_.canShoot_ && (cool_down == 0 && ammo_ > 0) && !isJumping_ && !isStunned;
	bool wantsShoot = (brain_->shoot_ > 0.0);

	if(canShoot && wantsShoot) {
		willShoot_ = true;
	} else if(cool_down > 0){
		if(wantsShoot)
			++failedShots_;

		willShoot_ = false;
		--cool_down;
	}

	if(isStunned)
	  --stun_;
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
	defensiveHits_ = 0;
  offensiveHits_ = 0;
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

void Ship::impact(Facility& f) {
  crash();
  f.crash();
}

void Ship::impact(Ship& other) {
	crash();
	other.crash();
}

void Ship::impact(Projectile& p) {
	damage();

	if (p.owner_->teamID_ != teamID_) {
		p.owner_->defensiveHits_++;
	} else {
		p.owner_->friendlyFire_++;
	}

	if(dead_ && p.owner_->teamID_ != teamID_)
		p.owner_->killed();
}

void Ship::killed() {
	++kills_;
}

void Ship::stun() {
  stun_ = layout_.maxStun_;
}

Ship Ship::makeChild() const {
	CHECK(brain_ != NULL);
	Ship child(teamID_, layout_);
	Brain* fresh  = new Brain();
	fresh->initialize(brain_->layout_);
	child.setBrain(fresh);
	return child;
}

Ship Ship::clone() const {
	CHECK(brain_ != NULL);
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
  auto pcopy = projectiles_;
  projectiles_.clear();

  for(Projectile* p : pcopy) {
	  if(p->blast_ != NULL)
	    delete p->blast_;
	  delete p;
	}

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
	isJumping_ = false;
	crashed_ = false;
}

void Ship::resetScore() {
	friendlyFire_ = 0;
	crash_ = 0;
	crashDamage_ = 0;
	defensiveHits_ = 0;
  offensiveHits_ = 0;
	damage_ = 0;
	fitness_ = 0;
	recharged_ = 0;
	failedShots_ = 0;
	kills_ = 0;
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
	CHECK(cool_down == 0);
	CHECK(ammo_ > 0);
	--ammo_;

	cool_down = layout_.maxCooldown_;
	Vector2D loc = loc_;
	loc += (getDirection() * radius_);
	Projectile* p = new Projectile(*this,layout_.pl_, loc, rotation_);
	projectiles_.push_back(p);
	return p;
}

} /* namespace neurocid */
