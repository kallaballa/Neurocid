/*
 * Object.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#include "2d.hpp"
#include <math.h>
#include <stdlib.h>

namespace tankwar {

struct Object {
	Vector2D loc_;
	Vector2D dir_;
	Coord rotation_;
	Coord range_;
	Coord speed_;
	bool explode_;
	bool dead_;

	Object(Vector2D loc, Vector2D dir, Coord rotation, Coord range, Coord speed, bool explode, bool dead) :
			loc_(loc), dir_(dir), rotation_(rotation), range_(range), speed_(speed), explode_(explode), dead_(dead) {
	}

	bool collides(const Object& other) {
		return sqrt(pow(abs(this->loc_.x - other.loc_.x),2) + pow(abs(this->loc_.y - other.loc_.y),2)) < (this->range_ + other.range_);
	}

	virtual void move() = 0;
};
}



#endif /* OBJECT_HPP_ */
