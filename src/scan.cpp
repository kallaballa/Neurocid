#include "scan.hpp"
#include "object.hpp"
#include "battlefieldlayout.hpp"

namespace neurocid {
	void ScanObject::calculate(Scan& scan, const BattleFieldLayout& bfl) {
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

		scale(vel_, velDist, std::max(bfl.width_,bfl.height_));
		scale(dir_, dist_, std::max(bfl.width_,bfl.height_));
	}

	Scan::Scan(Object* object) : object_(object) {
	}

	void Scan::makeScanObject(ScanObjectType type, Vector2D loc, Coord dis, Vector2D vel) {
		objects_.push_back(ScanObject(type, loc, dis, vel));
	}

	void Scan::calculate(const BattleFieldLayout& bfl) {
		normDir_ = object_->getDirection();

		normVel_ = object_->vel_;
		Coord dist = normVel_.length();
		normVel_.normalize();
		normVel_.rotate(normDir_);
		scale(normVel_, dist, std::max(bfl.width_,bfl.height_));

		normCenter_ = {bfl.width_/2, bfl.height_/2};
		dist = normCenter_.length();
		normCenter_.normalize();
		normCenter_.rotate(normDir_);
		scale(normCenter_, dist, std::max(bfl.width_,bfl.height_));

		for(ScanObject& so : objects_) {
			so.calculate(*this, bfl);
		}
	}
} /* namespace neurocid */
