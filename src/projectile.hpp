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
	Object* nearestObject_;
	Vector2D nearestEnemyLoc_;
	Coord nearestEnemyDis_;
	Vector2D nearestFriendLoc_;
	Coord nearestFriendDis_;
	Vector2D startLoc_;
	bool enemyHitter_;
	bool friendHitter_;

	Projectile(Tank& owner, Vector2D loc, Coord rotation) :
		Object(PROJECTILE, loc, rotation, Params::PROJECTILE_RANGE, false, false),
		owner_(&owner),
		nearestObject_(NULL),
		nearestEnemyLoc_(NO_COORD, NO_COORD),
		nearestEnemyDis_(NO_COORD),
		nearestFriendLoc_(NO_COORD, NO_COORD),
		nearestFriendDis_(NO_COORD),
		startLoc_(loc),
		enemyHitter_(false),
		friendHitter_(false){
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
