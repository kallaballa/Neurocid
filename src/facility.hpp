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
#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & radius_;
	  ar & maxCooldown_;
	}
#endif
};

class Facility : public Object {
public:
	size_t teamID_;
	FacilityLayout layout_;
	Scan scan_;
	bool captured_;
	size_t cooldown_;

	Facility(const size_t& teamID, const FacilityLayout& layout, const Vector2D& loc) :
		Object(FACILITY, loc, 0, layout.radius_, false, false, false),
		teamID_(teamID),
		layout_(layout),
		scan_(this),
		captured_(false),
		cooldown_(0) {
	}

	void move(BattleFieldLayout& bfl) {
		cool();
	}

	void captured() {
		cooldown_ = layout_.maxCooldown_;
		captured_ = true;
	}

	void cool() {
		if(cooldown_ > 0)
			--cooldown_;
	}

	bool isCool() {
		return cooldown_ == 0;
	}

	void reset() {
		cooldown_ = 0;
	}
	void death() {
		dead_ = true;
		explode_ = true;
		captured_ = false;
	}
};

}

#endif /* FACILITY_HPP_ */
