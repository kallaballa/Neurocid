/*
 * scan.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: elchaschab
 */

#ifndef SCAN_HPP_
#define SCAN_HPP_

#include "object.hpp"
#include "2d.hpp"
#include <vector>
#include <boost/pool/pool_alloc.hpp>

namespace tankwar {

using std::vector;

enum ScanObjectType {
	FRIEND,
	ENEMY,
	PROJECTILE_,
	INVALID
};

class Scan;
class ScanObject {
	void scale(Vector2D& v, const Coord& distance, const Coord& maxDistance) const;
public:
	ScanObjectType type_;
	Vector2D loc_;
	Coord dis_;

	Vector2D vector_ = NO_VECTOR2D;
	Coord	 angDist_ = NO_COORD;

	ScanObject(ScanObjectType type, Vector2D loc, Coord dis) :
		type_(type),
		loc_(loc),
		dis_(dis){
	}

	ScanObject() :
		type_(INVALID),
		loc_(NO_VECTOR2D),
		dis_(NO_COORD){
	}

	void calculate(Scan& scan);
};

typedef vector<ScanObject, boost::pool_allocator<ScanObject>> ScanObjectVector;
class Scan {
public:
	Vector2D dir_;
	Vector2D loc_;
	Vector2D vel_;
	Coord angVel_;

	Scan(Object& o) :
		dir_(o.getDirection()),
		loc_(o.loc_),
		vel_(o.vel_),
		angVel_(o.angVel_){
	}

	ScanObjectVector objects_;

	void makeScanObject(ScanObjectType type, Vector2D loc, Coord dis) {
		objects_.push_back(ScanObject(type, loc, dis));
	}

	void calculate() {
		vel_.normalize();
		vel_.rotate(dir_);

		for(ScanObject& so : objects_) {
			so.calculate(*this);
		}
	}
};
} /* namespace tankwar */

#endif /* SCAN_HPP_ */
