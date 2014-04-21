#include "scan.hpp"
#include "object.hpp"

namespace neurocid {
	void ScanObject::calculate(Scan& scan) {
		Vector2D toObject = (loc_ - scan.object_->loc_).normalize();
		dir_ = toObject;
		dir_.rotate(scan.normDir_);

		Coord velDist = vel_.length();
		vel_.normalize();
		vel_.rotate(scan.normDir_);

		//performance hack. instead of actually calculating the angle we use the y component as a distance
		if(dir_.x_ > 0)
			angDist_ = dir_.y_;
		else
			angDist_ = -dir_.y_;

		scale(vel_, velDist, 10000);
		scale(dir_, dis_, 10000);
	}

	Scan::Scan(Object* object) : object_(object) {
	}

	void Scan::makeScanObject(ScanObjectType type, Vector2D loc, Coord dis, Vector2D vel) {
		objects_.push_back(ScanObject(type, loc, dis, vel));
	}

	void Scan::calculate() {
		normVel_ = object_->vel_;
		normDir_ = object_->getDirection();
		Coord dist = normVel_.length();
		normVel_.normalize();
		normVel_.rotate(normDir_);
		scale(normVel_, dist, 10000);

		for(ScanObject& so : objects_) {
			so.calculate(*this);
		}
	}
} /* namespace neurocid */
