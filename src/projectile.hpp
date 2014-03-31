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

#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

namespace tankwar {

class Tank;

struct ProjectileLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif

	Coord max_speed_;
	Coord max_travel_;
	size_t range_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & max_speed_;
	  ar & max_travel_;
	  ar & range_;
	}
#endif
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
		scan_(*this){
	}

	void move(BattleFieldLayout& bfl) {
	}

	void death() {
		dead_ = true;
		explode_ = true;
	}
};

}

#endif /* PROJECTILE_HPP_ */
