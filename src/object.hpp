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
#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
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
	Coord range_;
	Coord fuel_;
	Coord max_fuel_;
	bool explode_;
	bool dead_;

	Object(ObjectType type, Vector2D loc, Coord rotation, Coord range, 	Coord fuel, Coord max_fuel, bool explode, bool dead) :
			type_(type), loc_(loc), vel_(), angVel_(0), rotation_(rotation), range_(range), fuel_(fuel), max_fuel_(max_fuel), explode_(explode), dead_(dead) {
		assert(rotation != 10);
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
		return distance(other) < (this->range_ + other.range_);
	}

	virtual void move(BattleFieldLayout& bfl) = 0;

	ObjectType type() {
		return type_;
	}
	void setDirection(Vector2D dir) {
		ASSERT_DIR(dir);
		rotation_ = radFromDir(dir);
		assert(rotation_ != 10);
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
		ar & range_;
		ar & explode_;
		ar & dead_;
		ar & fuel_;
	}
#endif
};
}

#endif /* OBJECT_HPP_ */
