/*
 * scanner1.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: elchaschab
 */

#include "scanner.hpp"
#include "population.hpp"
#include "battlefield.hpp"
#include "bsp.hpp"

namespace neurocid {

void BspScanner::buildBsps(BattleField& field) {
	assert(field.teams_.size() == 2);
	bspA_.clear();
	bspB_.clear();

	// interpreting friendly fire seems too irritating. so we only scan enemy projectiles.
	bspPA_.clear();
	bspPB_.clear();

	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];

	for (size_t i = 0; i < teamA.size(); ++i) {
		Ship& t = teamA[i];
		if(!t.dead_)
			bspA_.insert(&t);
		for (Projectile* p : t.projectiles_) {
			if(!p->dead_)
				bspPA_.insert(p);
		}
	}

	for (size_t i = 0; i < teamB.size(); ++i) {
		Ship& t = teamB[i];
		if(!t.dead_)
			bspB_.insert(&t);
		for (Projectile* p : t.projectiles_) {
			if(!p->dead_)
				bspPB_.insert(p);
		}
	}
}

void BspScanner::findInRange(ObjectBsp& bsp, Object& from, ScanObjectType type, ScanObjectVector& result, size_t range) {
	vector<Object*> objects;
	bsp.find_within_range(&from, range, std::back_inserter(objects));

	for(size_t i = 0; i < objects.size(); ++i) {
		result.push_back(ScanObject{type, objects[i]->loc_, objects[i]->distance(from)});
	}
}

bool BspScanner::findNearest(ObjectBsp& bsp, Object& from, ScanObjectType type, ScanObjectVector& result) {
	assert(!from.dead_);
	vector<Object*> found;
	auto nearest = bsp.find_nearest(&from, std::numeric_limits<Coord>().max());
	Object* obj = static_cast<Object*>(*nearest.first);
	if(obj != NULL) {
		result.push_back(ScanObject{type,obj->loc_,nearest.second});
		return true;
	}
	return false;
}

std::pair<Object*,Coord> BspScanner::findNearest(ObjectBsp& bsp, Object& from) {
	assert(!from.dead_);
	vector<Object*> found;
	auto result = bsp.find_nearest(&from, std::numeric_limits<Coord>().max());
	return {*result.first, result.second};
}

void BspScanner::prepare(BattleField& field) {
	buildBsps(field);
}

} /* namespace neurocid */
