/*
 * scan.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: elchaschab
 */

#ifndef SCAN_HPP_
#define SCAN_HPP_

#include "2d.hpp"
#include <vector>

namespace neurocid {

using std::vector;

enum ScanObjectType {
	FRIEND,
	ENEMY,
	PROJECTILE_,
	FRIEND_FACILITY,
	ENEMY_FACILITY,
	INVALID
};

inline void scale(Vector2D& v, const Coord& distance, const Coord& maxDistance) {
	Coord dist = distance;
	if(dist > maxDistance)
		dist = maxDistance;

	// higher distance -> shorter vector  - but doesn't go under 0.1
	// http://www.wolframalpha.com/input/?i=plot+s+%3D+1+-+%280.9+*+%28d+%2F+600000%29+%29+from+d+%3D+0+to+600000
	Coord scale = 1.0 - (0.9 * (dist / maxDistance));

	v.x_ *= scale;
	v.y_ *= scale;
	assert(v.x_ >= -1 && v.x_ <= 1 && v.y_ >= -1 && v.y_ <= 1);
}

class Scan;
struct BattleFieldLayout;

class ScanObject {
public:
	ScanObjectType type_;
	Vector2D loc_;
	Coord dist_;

	Vector2D dir_ = NO_VECTOR2D;
	Coord	 angDist_ = NO_COORD;
	Vector2D vel_ = NO_VECTOR2D;

	ScanObject(const ScanObjectType& type, const Vector2D& loc, const Coord& dis, const Vector2D& vel) :
		type_(type),
		loc_(loc),
		dist_(dis),
		vel_(vel){
	}

	ScanObject() :
		type_(INVALID),
		loc_(NO_VECTOR2D),
		dist_(NO_COORD),
		vel_(NO_VECTOR2D){
	}

	void calculate(Scan& scan, const BattleFieldLayout& bfl);
};

typedef vector<ScanObject> ScanObjectVector;

class Object;

class Scan {
public:
	Object* object_;
	ScanObjectVector objects_;
	Vector2D normVel_;
	Vector2D normDir_;
	Vector2D normCenter_;

	Scan(Object* object);
	void makeScanObject(ScanObjectType type, Vector2D loc, Coord dis, Vector2D vel);
	void calculate(const BattleFieldLayout& bfl);
};
} /* namespace neurocid */

#endif /* SCAN_HPP_ */
