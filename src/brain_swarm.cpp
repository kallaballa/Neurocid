#include "brain.hpp"
#include "battlefield.hpp"
#include "util.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>
#include <map>


namespace neurocid {
using std::map;


void sortByAngularDistance(ScanObjectVector& sorted, const ScanObjectVector& scanObjects, const ScanObjectType& type) {
	for(const ScanObject& so : scanObjects) {
		if(so.type_ == type)
			sorted.push_back(so);
	}

	std::sort(sorted.begin(), sorted.end(), [&](const ScanObject& sl, const ScanObject& sr){
		return sl.angDist_ < sr.angDist_;
	});
}

void BrainSwarm::update(const BattleFieldLayout& bfl, const Scan& scan) {
	parentBrain_t::update(bfl, scan);

	ScanObjectVector friendObj;
	ScanObjectVector enemyObj;
	ScanObjectVector friendFacilityObj;
	//ScanObjectVector enemyFacilityObj;

	//sort the friendly and enemy scan objects by their angular distance
	sortByAngularDistance(friendObj, scan.objects_, FRIEND);
	sortByAngularDistance(enemyObj, scan.objects_, ENEMY);
	sortByAngularDistance(friendFacilityObj, scan.objects_, FRIEND_FACILITY);
	//sortByAngularDistance(enemyFacilityObj, scan.objects_, ENEMY_FACILITY);

	assert(enemyObj.size() + friendObj.size() + friendFacilityObj.size() == scan.objects_.size());

	size_t inputCnt = 0;
	for (ScanObject& so : friendObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
		}
		++inputCnt;
	}

	for (ScanObject& so : friendFacilityObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
		}
		++inputCnt;
	}

	for (ScanObject& so : enemyObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
		}
		++inputCnt;
	}

	/*for (ScanObject& so : enemyFacilityObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
		}
		++inputCnt;
	}*/

	/*for (ScanObject& so : projectileObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
		}
		++inputCnt;
	}*/
	Vector2D vel = scan.vel_;
	Coord angVel = scan.angVel_;
	if(angVel > 10)
		angVel = 10;
	else if(angVel < -10)
		angVel = -10;

	applyInput((inputCnt * 4), vel.x_);
	applyInput((inputCnt * 4) + 1, vel.y_);
	applyInput((inputCnt * 4) + 2, angVel / 10);
	applyInput((inputCnt * 4) + 3, scan.fuel_ / scan.max_fuel_);
	applyInput((inputCnt * 4) + 4, shoot_ ? 1 : 0);

	for(size_t i = 0; i < layout_.numInputs_; ++i) {
		assert(!std::isinf(inputs_[i]));
		assert(!std::isnan(inputs_[i]));
		assert(inputs_[i] != std::numeric_limits<fann_type>().max());
	}

	//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}
} /* namespace neurocid */
