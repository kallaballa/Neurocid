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
#include "blast.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace neurocid {

class Ship;

struct ProjectileLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	BlastLayout el_;
	Coord maxSpeed_;
	Coord maxTravel_;
	Coord radius_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    //suppress warning about unused variable version
    assert(version >= 0);
	  ar & el_;
	  ar & maxSpeed_;
	  ar & maxTravel_;
	  ar & radius_;
	}
#endif
};

class Projectile : public Object {
public:
	Ship* owner_;
	ProjectileLayout layout_;
	Vector2D startLoc_;
	Scan scan_;
	Blast* blast_;

	Projectile(Ship& owner, ProjectileLayout& layout, Vector2D& loc, Coord& rotation) :
		Object(PROJECTILE, loc, rotation, layout.radius_, false, false, false),
		owner_(&owner),
		layout_(layout),
		startLoc_(loc),
		scan_(this),
		blast_(NULL) {
	}

	~Projectile() {
	}

	void move(BattleFieldLayout& bfl) {
		unused(bfl);
	}

	void death() {
		dead_ = true;
    explode_ = true;
	}

  bool blast() {
    if(blast_ == NULL) {
      blast_ = new Blast(*this,layout_.el_, loc_);
      return true;
    }
    return false;
  }
};

}

#endif /* PROJECTILE_HPP_ */
