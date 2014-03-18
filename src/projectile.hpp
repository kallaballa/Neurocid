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
#include "scanner.hpp"
#include <limits>

namespace tankwar {

class Tank;

class Projectile : public Object {
public:
	Tank* owner_;
	Vector2D startLoc_;
	bool enemyHitter_;
	bool friendHitter_;
	Scan scan_;

	Projectile(Tank& owner, Vector2D loc, Coord rotation) :
		Object(PROJECTILE, loc, rotation, Params::PROJECTILE_RANGE, false, false),
		owner_(&owner),
		startLoc_(loc),
		enemyHitter_(false),
		friendHitter_(false),
		scan_(){
		speed_ = Params::MAX_PROJECTILE_SPEED;
		rotForce_ = 0;
	}

	void move(BattleFieldLayout& bfl) {
		speed_ = Params::MAX_PROJECTILE_SPEED;
		rotForce_ = 0;
	}
};

}

#endif /* PROJECTILE_HPP_ */
