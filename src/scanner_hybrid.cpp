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

#ifdef TANKWAR_GPU
#include "../src/KMeans.hpp"
#include <ctime>
#endif

namespace tankwar {

void HybridScanner::teamScan(Population& active, Population& passive, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl) {
	Coord numFriends = 10;
	Coord numEnemies = 10;

	#pragma omp parallel for
	for(size_t i = 0; i < active.size(); ++i) {
		Tank& t = active[i];
		if(t.dead_ || t.layout_.isDummy_)
			continue;

		t.scan_ = Scan(t);
		assert(!std::isnan(t.scan_.angVel_));

		findInRange(bspFriends, t, FRIEND, t.scan_.objects_, 600);

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
		Tank* tenemy = static_cast<Tank*>(result.first);

		t.scan_.makeScanObject(ENEMY,tenemy->loc_,result.second);

		for(Vector2D ce : ctrEnemies) {
			t.scan_.makeScanObject(ENEMY, ce, t.distance(ce));
		}

		findInRange(bspEnemies, t, ENEMY, t.scan_.objects_, 600);
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
		if(t.layout_.disableProjectileFitness_)
			continue;

		for(Projectile* p : t.projectiles_) {
			if(p->dead_)
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
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];

	buildBsps(field);
	centersA_.clear();
	centersB_.clear();
	scanClusterCenters(teamA, centersA_, 3);
	scanClusterCenters(teamB, centersB_, 3);

	assert(centersA_.size() == 3);
	assert(centersB_.size() == 3);

	teamScan(teamA, teamB, centersA_, centersB_, bspA_, bspB_, field.layout_);
	teamScan(teamB, teamA, centersB_, centersA_, bspB_, bspA_, field.layout_);
}

} /* namespace tankwar */
