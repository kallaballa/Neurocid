#include "scan.hpp"

namespace tankwar {
	void ScanObject::scale(Vector2D& v, const Coord& distance, const Coord& maxDistance) const {
		Coord scale = 1 / ((distance / maxDistance) + 1);
		v.x_ *= scale;
		v.y_ *= scale;
		assert(v.x_ >= -1 && v.x_ <= 1 && v.y_ >= -1 && v.y_ <= 1);
	}

	void ScanObject::calculate(Scan& scan) {
		Vector2D toObject = (loc_ - scan.loc_).normalize();
		vector_ = toObject;
		vector_.rotate(scan.dir_);
		angle_ = radFromDir(vector_);
		scale(vector_, dis_, 9000);
	}
} /* namespace tankwar */