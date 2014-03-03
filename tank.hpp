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

	void shoot() {
		if(projectiles_ > 0)
			projectiles_--;
	}
public:
	size_t projectiles_ = Params::MAX_PROJECTILES;
	Brain brain_;
	size_t teamID_;
	Thrust lthrust_ = 0;
	Thrust rthrust_ = 0;
	size_t friendly_fire_ = 0;
	size_t hits_ = 0;
	size_t damage_ = 0;
	double fitness_ = 0;

	Tank(size_t teamID, Vector2D loc, Vector2D dir);
	~Tank() {};

	bool operator==(const Tank& other) const {
		return this->teamID_ == other.teamID_ && this->loc_ == other.loc_;
	}

	bool operator!=(const Tank& other) const {
		return !this->operator ==(other);
	}

	bool willShoot() {
		return projectiles_ > 0 && wantsShoot_;
	}
	void calculateFitness();
	void think(BattleField& field);
	void move();
	Tank makeChild();
	Tank clone();
	void reset();

	bool operator<(const Tank& other) const	{
		return (this->fitness_ < other.fitness_);
	}
};


} /* namespace tankwar */

#endif /* TANK_HPP_ */
