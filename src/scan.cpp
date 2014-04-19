#include "scan.hpp"

namespace neurocid {
	void ScanObject::calculate(Scan& scan) {
		Vector2D toObject = (loc_ - scan.loc_).normalize();
		dir_ = toObject;
		dir_.rotate(scan.dir_);

		Coord velDist = vel_.length();
		vel_.normalize();
		vel_.rotate(scan.dir_);

		//performance hack. instead of actually calculating the angle we use the y component as a distance
		if(dir_.x_ > 0)
			angDist_ = dir_.y_;
		else
			angDist_ = -dir_.y_;

		scale(vel_, velDist, 10000);
		scale(dir_, dis_, 10000);
	}
} /* namespace neurocid */
