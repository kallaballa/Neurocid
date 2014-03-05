/*
 * projectile.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef PROJECTILE_HPP_
#define PROJECTILE_HPP_

#include "object.hpp"
#include "params.hpp"
#include <limits>

namespace tankwar {

class Tank;

class Projectile : public Object {
public:
	Tank* owner_;
	Vector2D nearestEnemyLoc_;
	Coord nearestEnemyDis_;
	Vector2D startLoc_;

	Projectile(Tank& owner, Vector2D& loc, Coord rotation) :
		Object(loc, rotation, Params::PROJECTILE_RANGE, Params::MAX_PROJECTILE_SPEED, false, false),
		owner_(&owner),
		nearestEnemyLoc_(0,0),
		nearestEnemyDis_(std::numeric_limits<Coord>::max()),
		startLoc_(loc) {
		dir_.x = -sin(rotation_);
		dir_.y = cos(rotation_);
	}

	void move() {
		//update location
		loc_ += (dir_ * speed_);
	}

	Projectile& operator=(const Projectile& other);
};

}

#endif /* PROJECTILE_HPP_ */
