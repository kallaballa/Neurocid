/*
 * projectile.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef PROJECTILE_HPP_
#define PROJECTILE_HPP_

#include "object.hpp"
#include "scanner.hpp"
#include <limits>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace tankwar {

class Tank;

struct ProjectileLayout {
	friend class boost::serialization::access;

	Coord max_speed_;
	size_t range_;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & max_speed_;
	  ar & range_;
	}
};

class Projectile : public Object {
public:
	Tank* owner_;
	ProjectileLayout layout_;
	Vector2D startLoc_;
	Scan scan_;

	Projectile(Tank& owner, ProjectileLayout& layout, Vector2D& loc, Coord& rotation) :
		Object(PROJECTILE, loc, rotation, layout.range_, false, false),
		owner_(&owner),
		layout_(layout),
		startLoc_(loc),
		scan_(){
		speed_ = layout_.max_speed_;
		rotForce_ = 0;
	}

	void move(BattleFieldLayout& bfl) {
		speed_ = layout_.max_speed_;
		rotForce_ = 0;
	}
};

}

#endif /* PROJECTILE_HPP_ */
