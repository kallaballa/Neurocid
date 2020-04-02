#ifndef FACILITY_HPP_
#define FACILITY_HPP_

#include "object.hpp"
#include "scanner.hpp"

namespace neurocid {

class Ship;

struct FacilityLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	size_t radius_;
	size_t maxCooldown_;
	size_t maxDamage_;
	size_t crashesPerDamage_;
	size_t maxRecharge_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    //suppress warning about unused variable version
    assert(version >= 0);
	  ar & radius_;
	  ar & maxCooldown_;
	  ar & maxDamage_;
	  ar & crashesPerDamage_;
	  ar & maxRecharge_;
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
	size_t recharged_;

	Facility(const size_t& teamID, const FacilityLayout& layout, const Vector2D& loc) :
		Object(FACILITY, loc, 0, layout.radius_, false, false, false),
		teamID_(teamID),
		layout_(layout),
		scan_(this)
	{}

	void move(BattleFieldLayout& bfl);
	void resetGameState();
	void resetScore();
	void damage();
	void death();
	void crash();
	bool recharge(Ship& s);
};

}

#endif /* FACILITY_HPP_ */
