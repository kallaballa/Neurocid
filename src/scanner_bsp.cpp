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
#include "error.hpp"

namespace neurocid {

void BspScanner::buildBsps(BattleField& field) {
	bspA_.clear();
	bspB_.clear();

	// interpreting friendly fire seems too irritating. so we only scan enemy projectiles.
	bspPA_.clear();
	bspPB_.clear();

	bspFA_.clear();
	bspFB_.clear();

	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];

	for(Facility& f : teamA.facilities_) {
		if(f.teamID_ == 0)
			bspFA_.insert(&f);
		else
			bspFB_.insert(&f);
	}

	for(Facility& f : teamB.facilities_) {
		if(f.teamID_ == 0)
			bspFA_.insert(&f);
		else
			bspFB_.insert(&f);
	}

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

void BspScanner::findInRange(ObjectBsp& bsp, Object& from, vector<Object*>& result, size_t range) {
	vector<Object*> objects;
	bsp.find_within_range(&from, range, std::back_inserter(objects));

	for(size_t i = 0; i < objects.size(); ++i) {
    if(!objects[i]->dead_)
      result.push_back(objects[i]);
	}
}

void BspScanner::findInRange(ObjectBsp& bsp, Object& from, ScanObjectType type, ScanObjectVector& result, size_t range) {
	vector<Object*> objects;
	bsp.find_within_range(&from, range, std::back_inserter(objects));

	for(size_t i = 0; i < objects.size(); ++i) {
	  if(!objects[i]->dead_)
	    result.push_back(ScanObject{type, objects[i]->loc_, objects[i]->distance(from), objects[i]->vel_});
	}
}


bool BspScanner::findNearest(ObjectBsp& bsp, Object& from, ScanObjectType type, ScanObjectVector& result) {
	CHECK(!from.dead_);
	auto p = bsp.find_nearest_if(&from, std::numeric_limits<Coord>().max(), [&](Object* candidate) {
		return !candidate->dead_ && candidate != &from;
	});
	CHECK(&from != (*p.first));

	if(p.second != std::numeric_limits<Coord>().max()) {
		Object* obj = static_cast<Object*>(*p.first);
		result.push_back(ScanObject{type,obj->loc_,p.second, obj->vel_});
		return true;
	}

	//there is no one else
	return false;
}

size_t BspScanner::findNearestN(ObjectBsp& bsp, Object& from, ScanObjectType type, ScanObjectVector& result, size_t num) {
	CHECK(!from.dead_);
	size_t cnt = 0;
	auto p = bsp.find_nearest_if(&from, std::numeric_limits<Coord>().max(), [&](Object* candidate) {
		if(!candidate->dead_ && candidate != &from)
			++cnt;
		else
			return false;

		Object* obj = static_cast<Object*>(candidate);
		result.push_back(ScanObject{type,obj->loc_,from.distance(*candidate), obj->vel_});
		return (cnt == num);
	});
	CHECK(&from != (*p.first));

	return cnt;
}

std::pair<Object*,Coord> BspScanner::findNearest(ObjectBsp& bsp, Object& from) {
	CHECK(!from.dead_);
	vector<Object*> found;
	auto result = bsp.find_nearest(&from, std::numeric_limits<Coord>().max());
	return {*result.first, result.second};
}

void BspScanner::prepare(BattleField& field) {
	buildBsps(field);
}

} /* namespace neurocid */
