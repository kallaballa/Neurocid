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

void sortByAngularDistance(map<Coord, ScanObject>& angMap, const ScanObjectVector& scanObjects, const ScanObjectType& type) {
	for (const ScanObject& so : scanObjects) {
		assert(so.type_ != INVALID);
		if(so.type_ == type) {
			Coord angDist = so.angDist_;
			auto it = angMap.find(angDist);
			while(it != angMap.end()) {
				if(angDist > 0)
					angDist -= fRand(0.00000001, 0.00009);
				else
					angDist += fRand(0.00000001, 0.00009);

				it = angMap.find(angDist);
			}

			angMap[angDist] = so;
		}
	}
}

void BrainSwarm::update(const BattleFieldLayout& bfl, const Scan& scan) {
	parentBrain_t::update(bfl, scan);

	map<Coord, ScanObject> friendObj;
	map<Coord, ScanObject> enemyObj;
	map<Coord, ScanObject> projectileObj;

	//sort the friendly and enemy scan objects by their angular distance
	sortByAngularDistance(friendObj, scan.objects_, FRIEND);
	sortByAngularDistance(enemyObj, scan.objects_, ENEMY);
	sortByAngularDistance(projectileObj, scan.objects_, PROJECTILE_);

	assert(enemyObj.size() + friendObj.size() + projectileObj.size() == scan.objects_.size());

	size_t inputCnt = 0;
	for (auto it : friendObj) {
		ScanObject& so = it.second;
		if (so.vector_ != NO_VECTOR2D) {
			applyInput(inputCnt * 2, so.vector_.x_);
			applyInput(inputCnt * 2 + 1, so.vector_.y_);
		}
		++inputCnt;
	}

	for (auto it : enemyObj) {
		ScanObject& so = it.second;
		if (so.vector_ != NO_VECTOR2D) {
			applyInput(inputCnt * 2, so.vector_.x_);
			applyInput(inputCnt * 2 + 1, so.vector_.y_);
		}
		++inputCnt;
	}

	for (auto it : projectileObj) {
		ScanObject& so = it.second;
		if (so.vector_ != NO_VECTOR2D) {
			applyInput(inputCnt * 2, so.vector_.x_);
			applyInput(inputCnt * 2 + 1, so.vector_.y_);
		}
		++inputCnt;
	}

	Vector2D vel = scan.vel_;
	vel.normalize();
	applyInput((inputCnt * 2), vel.x_);
	applyInput((inputCnt * 2) + 1, vel.y_);
	++inputCnt;

	Coord angVel = scan.angVel_;
	if(angVel > 10)
		angVel = 10;
	else if(angVel < -10)
		angVel = -10;

	applyInput((inputCnt * 2), angVel / 10);
	applyInput((inputCnt * 2) + 1, scan.fuel_ / scan.max_fuel_);
	++inputCnt;

	for(size_t i = 0; i < layout_.numInputs_; ++i) {
		assert(!std::isinf(inputs_[i]));
		assert(!std::isnan(inputs_[i]));
		assert(inputs_[i] != std::numeric_limits<fann_type>().max());
	}

	//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}
} /* namespace neurocid */
