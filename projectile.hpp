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

namespace tankwar {

class Tank;

class Projectile : public Object {
public:
	Tank& owner_;
	Projectile(Tank& owner, Vector2D& loc, Vector2D& dir) : Object(loc, dir, 0.0, Params::PROJECTILE_RANGE, Params::MAX_PROJECTILE_SPEED, false, false), owner_(owner) {
	}

	void move() {
		//update location
		loc_ += (dir_ * speed_);
	}
};

}

#endif /* PROJECTILE_HPP_ */
