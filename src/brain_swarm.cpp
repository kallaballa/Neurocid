#include "brain.hpp"
#include "battlefield.hpp"
#include "error.hpp"
#include "util.hpp"
#include <fann.h>
#include <iostream>
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
	ScanObjectVector enemyFacilityObj;
	Coord cntFriendMag = 0;
	Coord cntEnemyMag = 0;
	Coord cntFFacMag = 0;
	Coord cntEFacMag = 0;
	Coord totalFriendMag = 0;
	Coord totalEnemyMag = 0;
	Coord totalFFacMag = 0;
	Coord totalEFacMag = 0;

	//sort the friendly and enemy scan objects by their angular distance
	sortByAngularDistance(friendObj, scan.objects_, FRIEND);
	sortByAngularDistance(enemyObj, scan.objects_, ENEMY);
	sortByAngularDistance(friendFacilityObj, scan.objects_, FRIEND_FACILITY);
	sortByAngularDistance(enemyFacilityObj, scan.objects_, ENEMY_FACILITY);

	CHECK(enemyObj.size() + friendObj.size() + friendFacilityObj.size() + enemyFacilityObj.size() == scan.objects_.size());

	size_t inputCnt = 0;
	for (ScanObject& so : friendObj) {
		if (so.dir_ != NO_VECTOR2D) {

			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
			totalFriendMag += so.dir_.length();
			++cntFriendMag;
		}

		++inputCnt;
	}

	for (ScanObject& so : friendFacilityObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
			totalEnemyMag += so.dir_.length();
			++cntEnemyMag;
		}
		++inputCnt;
	}

	for (ScanObject& so : enemyObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
			totalFFacMag += so.dir_.length();
			++cntFFacMag;
		}
		++inputCnt;
	}

	for (ScanObject& so : enemyFacilityObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
			totalEFacMag += so.dir_.length();
			++cntEFacMag;
		}
		++inputCnt;
	}

	/*for (ScanObject& so : projectileObj) {
		if (so.dir_ != NO_VECTOR2D) {
			applyInput(inputCnt * 4, so.dir_.x_);
			applyInput(inputCnt * 4 + 1, so.dir_.y_);
			applyInput(inputCnt * 4 + 2, so.vel_.x_);
			applyInput(inputCnt * 4 + 3, so.vel_.y_);
		}
		++inputCnt;
	}*/

	Ship& ship = *static_cast<Ship*>(scan.object_);
	applyMeta(0, ((ship.fuel_ / ship.layout_.maxFuel_) * 2.0) -1.0);
	applyMeta(1, ((ship.ammo_ / ship.layout_.maxAmmo_) * 2.0) -1.0);
	applyMeta(2, ((ship.damage_ / ship.layout_.maxDamage_) * 2.0) -1.0);
	applyMeta(3, ((ship.cool_down / ship.layout_.maxCooldown_) * 2.0) -1.0);

	if(cntFriendMag)
		applyMeta(4, (totalFriendMag / cntFriendMag) * 2 - 1);
	else
		applyMeta(4, 0);

	if(cntEnemyMag)
		applyMeta(5, (totalEnemyMag / cntEnemyMag) * 2 - 1);
	else
		applyMeta(5, 0);

	if(cntFFacMag)
		applyMeta(6, (totalFFacMag / cntFFacMag) * 2 - 1);
	else
		applyMeta(6, 0);

	if(cntEFacMag)
		applyMeta(7, (totalEFacMag / cntEFacMag) * 2 - 1);
	else
		applyMeta(7, 0);

	//FIXME we need the scanner layout
	applyMeta(8, (cntFriendMag - cntEnemyMag) / 10 );
	applyMeta(9, (cntFFacMag - cntEFacMag) / 10 );

	Vector2D vel = scan.normVel_;
	Vector2D center = scan.normCenter_;
	Coord angVel = ship.angVel_;
	if(angVel > 10)
		angVel = 10;
	else if(angVel < -10)
		angVel = -10;

	applyInput(inputCnt * 4, vel.x_);
	applyInput(inputCnt * 4 + 1, vel.y_);
	applyInput(inputCnt * 4 + 2, center.x_);
	applyInput(inputCnt * 4 + 3, center.y_);
	applyInput(inputCnt * 4 + 4, angVel / 10);

	for(size_t i = 0; i < layout_.numInputs_; ++i) {
		CHECK(!std::isinf(inputs_[i]));
		CHECK(!std::isnan(inputs_[i]));
		CHECK(inputs_[i] != std::numeric_limits<fann_type>().max());
	}

	//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}
} /* namespace neurocid */
