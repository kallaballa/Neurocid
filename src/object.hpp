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
#include "error.hpp"

#include <math.h>
#include <stdlib.h>
#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace neurocid {

struct BattleFieldLayout;

class Object {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
public:
	ObjectType type_;
	Vector2D loc_;
	Vector2D vel_;
	Coord angVel_;
	Coord rotation_;
	Coord radius_;
	bool explode_;
	bool dead_;
	bool crashed_;

	Object(ObjectType type, Vector2D loc, Coord rotation, Coord radius, bool explode, bool dead, bool crashed) :
			type_(type), loc_(loc), vel_(), angVel_(0), rotation_(rotation), radius_(radius), explode_(explode), dead_(dead), crashed_(crashed) {
		CHECK(rotation != 10);
	}

	virtual ~Object() {
	}

	inline Coord distance(const Object& other) const {
		return loc_.distance(other.loc_);
	}

	inline Coord distance(const Vector2D& loc) const {
		return loc_.distance(loc);
	}

	inline bool collides(const Object& other) const {
		return distance(other) < (this->radius_ + other.radius_);
	}

	virtual void move(BattleFieldLayout& bfl) = 0;

	ObjectType type() {
		return type_;
	}
	void setDirection(Vector2D dir) {
		ASSERT_DIR(dir);
		rotation_ = radFromDir(dir);
		CHECK(rotation_ != 10);
	}

	Vector2D getDirection() const {
		return 	dirFromRad(rotation_);
	}

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & type_;
		ar & loc_;
		ar & vel_;
		ar & angVel_;
		ar & rotation_;
		ar & radius_;
		ar & explode_;
		ar & dead_;
		ar & crashed_;
	}
#endif
};
}

#endif /* OBJECT_HPP_ */
