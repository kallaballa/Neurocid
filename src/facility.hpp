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
	size_t range_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & range_;
	  ar & radius_;
	}
#endif
};

class Facility : public Object {
public:
	size_t teamID_;
	FacilityLayout layout_;
	Scan scan_;

	Facility(const size_t& teamID, const FacilityLayout& layout, const Vector2D& loc) :
		Object(FACILITY, loc, 0, layout.radius_, 0, 0, false, false, false),
		teamID_(teamID),
		layout_(layout),
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

#endif /* FACILITY_HPP_ */
