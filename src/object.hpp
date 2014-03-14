/*
 * Object.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#include "2d.hpp"
#include "object_types.hpp"
#include <math.h>
#include <stdlib.h>

namespace tankwar {

class BattleFieldLayout;

struct Object {
	Vector2D loc_;
	Coord rotation_;
	Coord range_;
	Coord speed_;
	bool explode_;
	bool dead_;

	Object(Vector2D loc, Coord rotation, Coord range, Coord speed, bool explode, bool dead) :
			loc_(loc), rotation_(rotation), range_(range), speed_(speed), explode_(explode), dead_(dead) {
	}

	virtual ~Object() {
	}

	Coord distance(const Object& other) const {
		return hypot(this->loc_.x - other.loc_.x, this->loc_.y - other.loc_.y);
	}

	bool collides(const Object& other) const {
		return distance(other) < (this->range_ + other.range_);
	}

	virtual void move(BattleFieldLayout& bfl) = 0;
	virtual ObjectType type() = 0;
	void setDirection(Vector2D dir) {
		ASSERT_DIR(dir);
		rotation_ = radFromDir(dir);
	}

	Vector2D getDirection() const {
		return 	dirFromRad(rotation_);
	}
};
}

#endif /* OBJECT_HPP_ */
