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

void HybridScanner::teamScan(Population& friends, Population& enemies, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, ObjectBsp& bspFriends, ObjectBsp& bspEnemies, ObjectBsp& bspEnemyProjectiles, BattleFieldLayout& bfl) {
	Coord numFriends = ClusterScanner::layout_.numFriends_;
	Coord numEnemies = ClusterScanner::layout_.numEnemies_;
	Coord numProjectiles = ClusterScanner::layout_.numProjectiles_;
	size_t rangeOfSight = 3000;

	for(size_t i = 0; i < friends.size(); ++i) {
		Ship& t = friends[i];
		if(t.dead_ || t.layout_.isDummy_)
			continue;

		t.scan_ = Scan(t);
		assert(!std::isnan(t.scan_.angVel_));

		// Scan for friends.
		findNearest(bspFriends, t, FRIEND, t.scan_.objects_);
		findInRange(bspFriends, t, FRIEND, t.scan_.objects_, rangeOfSight);

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
				t.scan_.objects_.push_back(so);
				if(t.scan_.objects_.size() == numFriends)
					break;
			}
		}

		assert(t.scan_.objects_.size() == numFriends);

		// Scan for enemies
		findNearest(bspEnemies, t, ENEMY, t.scan_.objects_);
		findInRange(bspEnemies, t, ENEMY, t.scan_.objects_, rangeOfSight);
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
/*
		// Scan for projectiles
		size_t startNum = (numFriends + numEnemies);
		size_t targetNum = (numFriends + numEnemies + numProjectiles);

		findNearest(bspEnemyProjectiles, t, PROJECTILE_, t.scan_.objects_);
		findInRange(bspEnemyProjectiles, t, PROJECTILE_, t.scan_.objects_, rangeOfSight);

		if(t.scan_.objects_.size() > targetNum) {
			t.scan_.objects_.resize(targetNum);
		} else if(t.scan_.objects_.size() == startNum) {
			for(size_t i = startNum; i < targetNum; ++i) {
				t.scan_.makeScanObject(PROJECTILE_,NO_VECTOR2D,NO_COORD);
			}
		}

		while(t.scan_.objects_.size() < targetNum) {
			size_t s = t.scan_.objects_.size();
			for(size_t i = startNum; i < s; ++i) {
				ScanObject& so = t.scan_.objects_[i];
				assert(so.type_ == PROJECTILE_);
				t.scan_.objects_.push_back(so);
				if(t.scan_.objects_.size() == targetNum)
					break;
			}
		}

		assert(t.scan_.objects_.size() == targetNum);
*/
		//Scan for near misses
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
	teamScan(teamA, teamB, centersA_, centersB_, bspA_, bspB_, bspPB_, field.layout_);
	teamScan(teamB, teamA, centersB_, centersA_, bspB_, bspA_, bspPA_, field.layout_);
}

void HybridScanner::prepare(BattleField& field) {
	BspScanner::prepare(field);
	ClusterScanner::prepare(field);
}

} /* namespace neurocid */
