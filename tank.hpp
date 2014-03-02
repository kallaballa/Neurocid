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
	size_t projectiles_ = Params::MAX_PROJECTILES;
	bool wantsShoot_ = false;

	void shoot() {
		if(projectiles_ > 0)
			projectiles_--;
	}
public:
	Brain_ptr brain_;
	size_t teamID_;
	Thrust lthrust_ = 0;
	Thrust rthrust_ = 0;
	size_t friendly_fire = 0;
	size_t hits = 0;
	size_t damage_ = 0;

	Tank(Brain_ptr brain_, size_t teamID, Vector2D loc, Vector2D dir);

	bool operator==(const Tank& other){
		return this->teamID_ == other.teamID_ && this->loc_ == other.loc_;
	}

	bool operator!=(const Tank& other){
		return !this->operator ==(other);
	}

	bool willShoot() {
		return projectiles_ > 0 && wantsShoot_;
	}

	template<class T>
	void clamp(T &arg, T min, T max)
	{
	  if (arg < min) {
	    arg = min;
	  }

	  if (arg > max) {
	    arg = max;
	  }
	}

	void think(BattleField& field);
	void move();
};

} /* namespace tankwar */

#endif /* TANK_HPP_ */
