#ifndef TANK_HPP_
#define TANK_HPP_

#include "object.hpp"
#include "projectile.hpp"
#include "params.hpp"
#include "battlefield.hpp"
#include "2d.hpp"
#include <cstring>
#include <iostream>
#include "brain.hpp"

namespace tankwar {

class Tank : public Object {
	bool wantsShoot_ = false;

public:
	std::vector<Projectile> projectiles_;

	size_t ammonition_ = Params::MAX_AMMO;
	Brain brain_;
	size_t teamID_;
	Thrust lthrust_ = 0;
	Thrust rthrust_ = 0;
	size_t friendly_fire_ = 0;
	size_t hits_ = 0;
	size_t damage_ = 0;
	double fitness_ = 0;

	Tank(size_t teamID, BrainLayout layout, Vector2D loc, Coord rotation);
	~Tank() {};

	bool operator==(const Tank& other) const {
		return this->teamID_ == other.teamID_ && this->loc_ == other.loc_;
	}

	bool operator!=(const Tank& other) const {
		return !this->operator ==(other);
	}

	bool shoot() {
		if(wantsShoot_ && ammonition_ > 0) {
			--ammonition_;
			return true;
		}
		return false;
	}


	void setDirection(Vector2D dir);
	void setRotation(double r);

	void calculateFitness();
	void think(BattleField& field);
	void move();
	Tank makeChild();

	Projectile& makeProjectile() {
		projectiles_.push_back(Projectile(*this, loc_, rotation_));
		return *(projectiles_.end() - 1);
	}

	Tank clone();
	void resetGameState();
	void resetScore();


	bool operator<(const Tank& other) const	{
		return (this->fitness_ < other.fitness_);
	}
};


} /* namespace tankwar */

#endif /* TANK_HPP_ */
