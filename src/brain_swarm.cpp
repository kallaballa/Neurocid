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

void BrainSwarm::scaleByBattleFieldDistance(Vector2D& v, const Coord& distance, const BattleFieldLayout& bfl) const {
	Coord scale = 1 / ((distance / hypot(bfl.width_,bfl.height_) + 1));
	v.x_ *= scale;
	v.y_ *= scale;
	assert(v.x_ >= -1 && v.x_ <= 1 && v.y_ >= -1 && v.y_ <= 1);
}

void BrainSwarm::applyInput(const size_t& i, const fann_type& value) {
	assert(i < layout_.numInputs_);
	inputs_[i] = value;
	assert(!std::isnan(inputs_[i]) && !std::isinf(inputs_[i]) && inputs_[i] >= -1 && inputs_[i] <= 1);
}

void BrainSwarm::update(const BattleFieldLayout& bfl, const Scan& scan) {
	assert(nn_ != NULL);
	assert(inputs_ != NULL);
	assert(!destroyed_);

	size_t numInputs = layout_.numInputs_;
	assert(layout_.numInputs_ == (scan.objects_.size() * 2) + 3);
	assert(layout_.numInputs_ == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));

	map<Coord, pair<Vector2D,Coord>> friendObj;
	map<Coord, pair<Vector2D,Coord>> enemyObj;

	//sort the friendly and enemy scan objects by their angular distance
	for (const ScanObject& so : scan.objects_) {
		if(so.type_ == FRIEND) {
			Vector2D toFriend = (so.loc_ - scan.loc_).normalize();
			Vector2D vdiff = toFriend;
			vdiff.rotate(scan.dir_);
			Coord diff = radFromDir(vdiff);

			auto it = friendObj.find(diff);
			while(it != friendObj.end()) {
				if(diff > 0)
					diff -= fRand(0.00000001, 0.00009);
				else
					diff += fRand(0.00000001, 0.00009);

				it = friendObj.find(diff);
			}

			friendObj[diff] = {vdiff, so.dis_};
		}
	}

	for (const ScanObject& so : scan.objects_) {
		if(so.type_ == ENEMY) {
			Vector2D toEnemy = (so.loc_ - scan.loc_).normalize();
			Vector2D vdiff = toEnemy;
			vdiff.rotate(scan.dir_);
			Coord diff = radFromDir(vdiff);

			auto it = enemyObj.find(diff);
			while(it != enemyObj.end()) {
				if(diff > 0)
					diff -= fRand(0.00000001, 0.00009);
				else
					diff += fRand(0.00000001, 0.00009);

				it = enemyObj.find(diff);
			}

			enemyObj[diff] = {vdiff, so.dis_};
		}
	}

	assert(enemyObj.size() + friendObj.size() == scan.objects_.size());

	size_t inputCnt = 0;
	for (auto it : friendObj) {
		Vector2D& vdiff = it.second.first;
		Coord& distance = it.second.second;
		if (vdiff != NO_VECTOR2D) {
			scaleByBattleFieldDistance(vdiff, distance, bfl);
			applyInput(inputCnt * 2, vdiff.x_);
			applyInput(inputCnt * 2 + 1, vdiff.y_);
		}
		inputCnt+=1;
	}

	for (auto it : enemyObj) {
		Vector2D& vdiff = it.second.first;
		Coord& distance = it.second.second;

		if (vdiff != NO_VECTOR2D) {
			scaleByBattleFieldDistance(vdiff, distance, bfl);
			applyInput(inputCnt * 2, vdiff.x_);
			applyInput(inputCnt * 2 + 1, vdiff.y_);
		}
		inputCnt+=1;
	}

	Vector2D vel = scan.vel_;
	vel.normalize();
	applyInput(inputCnt, vel.x_);
	applyInput(inputCnt+1, vel.y_);

	Coord angVel = scan.angVel_;
	if(angVel > 20)
		angVel = 20;
	else if(angVel < -20)
		angVel = -20;

	applyInput(inputCnt+2, angVel / 20);

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
