#include "brain.hpp"
#include "battlefield.hpp"
#include "util.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>
#include <map>


namespace tankwar {
using std::map;

BrainSwarm::BrainSwarm() : BasicBrain() {
}

BrainSwarm::BrainSwarm(BrainLayout layout, fann_type* weight) : BasicBrain(layout, weight) {
	for(size_t i = 0; i < layout.numInputs_; i++) {
		inputs_[i] = 0;
	}
}

/*
 * Calculates the smaller angle between dir and loc (angular distance).
 */
Coord BrainSwarm::calculateEnemyAngularDistance(const Vector2D& dir, const Vector2D& loc) {
	Coord diff = (M_PI + radFromDir(dir))
			- (M_PI + radFromDir(loc));

	if (diff > M_PI)
		diff = M_PI - diff;

	else if (diff < -M_PI)
		diff = M_PI + diff;

	assert(diff > -M_PI && diff < M_PI);

	return diff;
}

Coord BrainSwarm::calculateFriendAngularDistance(const Vector2D& dir, const Vector2D& loc) {
	double diff = (M_PI + radFromDir(dir)) - (M_PI + radFromDir(loc));

	if (diff > M_PI)
		diff = M_PI - diff;
	else if (diff < -M_PI)
		diff = M_PI + diff;

	if (diff <= 0)
		diff = (M_PI / 2) + diff;

	if (diff > 0)
		diff = -(M_PI / 2) + diff;

//		std::cerr << "diff:" << diff << std::endl;
	assert(diff > -M_PI && diff < M_PI);
	return diff;
}

void BrainSwarm::scaleByBattleFieldDistance(Vector2D& v, const Coord& distance, const BattleFieldLayout& bfl) const {
	Coord scale = 1 / ((distance / hypot(bfl.width_,bfl.height_) + 1));
	v.x *= scale;
	v.y *= scale;
	assert(v.x >= -1 && v.x <= 1 && v.y >= -1 && v.y <= 1);
}

void BrainSwarm::applyInput(const size_t& i, const fann_type& value) {
	inputs_[i] = value;
	assert(!std::isnan(inputs_[i]) && !std::isinf(inputs_[i]) && inputs_[i] >= -1 && inputs_[i] <= 1);
}

void BrainSwarm::update(const BattleFieldLayout& bfl, const Scan& scan) {
	assert(nn_ != NULL);
	assert(inputs_ != NULL);
	assert(!destroyed_);

	size_t numInputs = layout_.numInputs_;
    assert(layout_.numInputs_ == (scan.objects_.size() * 2));
	assert(layout_.numInputs_ == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));

	map<Coord, ScanObject> friendObj;
	map<Coord, ScanObject> enemyObj;

	//sort the friendly and enemy scan objects by their angular distance
	for (const ScanObject& so : scan.objects_) {
		if(so.type_ == FRIEND) {
			Coord diff = calculateEnemyAngularDistance(scan.dir_, so.loc_ - scan.loc_);
			auto it = friendObj.find(diff);
			while(it != friendObj.end()) {
				if(diff > (M_PI - 0.1))
					diff -= fRand(0.00000001, 0.00009);
				else
					diff += fRand(0.00000001, 0.00009);

				it = friendObj.find(diff);
			}

			friendObj[diff] = so;
		}
	}

	for (const ScanObject& so : scan.objects_) {
		if(so.type_ == ENEMY) {
			Coord diff = calculateEnemyAngularDistance(scan.dir_, so.loc_ - scan.loc_);
			auto it = enemyObj.find(diff);
			while(it != enemyObj.end()) {
				if(diff > (M_PI - 0.1))
					diff -= fRand(0.00000001, 0.00009);
				else
					diff += fRand(0.00000001, 0.00009);

				it = enemyObj.find(diff);
			}

			enemyObj[diff] = so;
		}
	}

	assert(enemyObj.size() == scan.objects_.size() / 2);
	assert(friendObj.size() == scan.objects_.size() / 2);

	size_t inputCnt = 0;
	for (auto it : friendObj) {
		Coord diff = it.first;
		ScanObject& so = it.second;
		if (so.loc_ != NO_VECTOR2D) {
			Vector2D vdiff = dirFromRad(diff);
			scaleByBattleFieldDistance(vdiff, so.dis_, bfl);
			applyInput(inputCnt * 2, vdiff.x);
			applyInput(inputCnt * 2 + 1, vdiff.y);
		}
		inputCnt+=1;
	}

	for (auto it : enemyObj) {
		Coord diff = it.first;
		ScanObject& so = it.second;
		if (so.loc_ != NO_VECTOR2D) {
			Vector2D vdiff = dirFromRad(diff);
			scaleByBattleFieldDistance(vdiff, so.dis_, bfl);
			applyInput(inputCnt * 2, vdiff.x);
			applyInput(inputCnt * 2 + 1, vdiff.y);
		}
		inputCnt+=1;
	}

	for(size_t i = 0; i < numInputs; ++i) {
		assert(!std::isinf(inputs_[i]));
		assert(!std::isnan(inputs_[i]));
	}

	fann_type* outputs = fann_run(nn_, inputs_);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	shoot_ = outputs[2];
	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(shoot_));
	assert(!std::isinf(lthrust_) && !std::isinf(rthrust_) && !std::isinf(shoot_));

	//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}
} /* namespace tankwar */
