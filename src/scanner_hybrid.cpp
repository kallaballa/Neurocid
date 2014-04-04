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
#include "util.hpp"

#ifdef _GPU_KMEANS
#include "../src/KMeans.hpp"
#include <ctime>
#endif

namespace neurocid {

void HybridScanner::teamScan(Population& active, Population& passive, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, ObjectBsp& bspFriends, ObjectBsp& bspEnemies, BattleFieldLayout& bfl) {
	Coord numFriends = ClusterScanner::layout_.numFriends_;
	Coord numEnemies = ClusterScanner::layout_.numEnemies_;

	for(size_t i = 0; i < active.size(); ++i) {
		Ship& t = active[i];
		if(t.dead_ || t.layout_.isDummy_)
			continue;

		t.scan_ = Scan(t);
		assert(!std::isnan(t.scan_.angVel_));

		findInRange(bspFriends, t, FRIEND, t.scan_.objects_, 3000);

		if(t.scan_.objects_.size() > numFriends) {
			t.scan_.objects_.resize(numFriends);
		} else if(t.scan_.objects_.empty()) {
			for(size_t i = 0; i < numFriends; ++i) {
				t.scan_.makeScanObject(FRIEND,NO_VECTOR2D,NO_COORD);
			}
		}

		while(t.scan_.objects_.size() < numFriends) {
			size_t s = t.scan_.objects_.size();
			for(size_t i = 0; i < s; ++i) {
				ScanObject& so = t.scan_.objects_[i];
				assert(so.type_ == FRIEND || so.type_ == ENEMY);
				t.scan_.objects_.push_back(so);
				if(t.scan_.objects_.size() == numFriends)
					break;
			}
		}

		assert(t.scan_.objects_.size() == numFriends);

		auto result = findNearest(bspEnemies, t);
		Ship* tenemy = static_cast<Ship*>(result.first);

		t.scan_.makeScanObject(ENEMY,tenemy->loc_,result.second);
		findInRange(bspEnemies, t, ENEMY, t.scan_.objects_, 3000);
		for(Vector2D ce : ctrEnemies) {
			t.scan_.makeScanObject(ENEMY, ce, t.distance(ce));
		}

		if(t.scan_.objects_.size() > (numFriends + numEnemies)) {
			t.scan_.objects_.resize(numFriends + numEnemies);
		}

		while(t.scan_.objects_.size() < (numFriends + numEnemies)) {
			size_t s = t.scan_.objects_.size();
			for(size_t i = numFriends; i < s; ++i) {
				ScanObject& so = t.scan_.objects_[i];
				t.scan_.objects_.push_back(so);
				if(t.scan_.objects_.size() == (numFriends + numEnemies))
					break;
			}
		}

		assert(t.scan_.objects_.size() == (numFriends + numEnemies));

		for(Projectile* p : t.projectiles_) {
			if(p->dead_ || t.layout_.disableProjectileFitness_)
				continue;

			p->scan_ = Scan(*p);

			auto result = findNearestCenter(ctrEnemies, p->loc_);
			Vector2D enemyLoc = result.first;
			assert(p->scan_.objects_.empty() || p->scan_.objects_.size() == 2);

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.empty()) {
					p->scan_.makeScanObject(ENEMY, enemyLoc, result.second);
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[0].loc_ = enemyLoc;
					p->scan_.objects_[0].dis_ = result.second;
				}
			} else {
				p->scan_.makeScanObject(ENEMY, NO_VECTOR2D, NO_COORD);
			}

			result = findNearestCenter(ctrFriends, p->loc_);
			Vector2D friendLoc = result.first;

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.size() == 1) {
					p->scan_.makeScanObject(FRIEND, friendLoc, result.second);
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[1].loc_ = friendLoc;
					p->scan_.objects_[1].dis_ = result.second;
				}
			} else {
				p->scan_.makeScanObject(FRIEND, NO_VECTOR2D, NO_COORD);
			}

			p->scan_.calculate();
		}

		t.scan_.calculate();
	}
}

void HybridScanner::scan(BattleField& field) {
	assert(field.teams_.size() == 2);
	prepare(field);
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];
	teamScan(teamA, teamB, centersA_, centersB_, bspA_, bspB_, field.layout_);
	teamScan(teamB, teamA, centersB_, centersA_, bspB_, bspA_, field.layout_);
}

void HybridScanner::prepare(BattleField& field) {
	BspScanner::prepare(field);
	ClusterScanner::prepare(field);
}

} /* namespace neurocid */
