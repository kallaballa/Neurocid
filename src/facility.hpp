#ifndef FACILITY_HPP_
#define FACILITY_HPP_

#include "object.hpp"
#include "scanner.hpp"
#include <limits>

namespace neurocid {

struct FacilityLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	size_t radius_;
	size_t maxCooldown_;
	size_t maxDamage_;
	size_t crashesPerDamage_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & radius_;
	  ar & maxCooldown_;
	  ar & maxDamage_;
	  ar & crashesPerDamage_;
	}
#endif
};

class Facility : public Object {
public:
	size_t teamID_;
	FacilityLayout layout_;
	Scan scan_;
	size_t damage_;
	size_t crash_;
	size_t crashDamage_;

	Facility(const size_t& teamID, const FacilityLayout& layout, const Vector2D& loc) :
		Object(FACILITY, loc, 0, layout.radius_, false, false, false),
		teamID_(teamID),
		layout_(layout),
		scan_(this)
	{}

	void move(BattleFieldLayout& bfl) {
	}

	void resetGameState() {
	  dead_ = false;
	  explode_ = false;
	  crashed_ = false;
	}

	void damage() {
	  damage_++;
	  if (damage_ >= layout_.maxDamage_) {
	    death();
	  }
	}

	void death() {
	  damage_ = layout_.maxDamage_;
	  dead_ = true;
	  explode_ = true;
	}

	void crash() {
	  crash_++;
	  crashDamage_++;
	  crashed_ = true;

	  if(crashDamage_ >= layout_.crashesPerDamage_) {
	    crashDamage_ = 0;
	    damage();
	  }
	}
};

}

#endif /* FACILITY_HPP_ */
