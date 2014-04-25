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

void HybridScanner::teamScan(Population& friends, Population& enemies, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, ObjectBsp& bspFriends, ObjectBsp& bspEnemies, ObjectBsp& bspFriendFacilities, ObjectBsp& bspEnemyFacilities, ObjectBsp& bspEnemyProjectiles, BattleFieldLayout& bfl) {
	Coord numFriends = ClusterScanner::layout_.numFriends_;
	Coord numEnemies = ClusterScanner::layout_.numEnemies_;
	Coord numFriendFacilities = ClusterScanner::layout_.numFriendFacilities_;
	Coord numEnemyFacilities = ClusterScanner::layout_.numEnemyFacilities_;

	//	Coord numProjectiles = ClusterScanner::layout_.numProjectiles_;
	size_t rangeOfSight = 10000;

	for(size_t i = 0; i < friends.size(); ++i) {
		Ship& t = friends[i];
		if(t.dead_ || t.layout_.isDummy_)
			continue;

		t.scan_ = Scan(&t);

		// Scan for friends.
		findNearest(bspFriends, t, FRIEND, t.scan_.objects_);
		findNearestN(bspFriends, t, FRIEND, t.scan_.objects_, numFriends - t.scan_.objects_.size());

		if(t.scan_.objects_.size() > numFriends) {
			t.scan_.objects_.resize(numFriends);
		} else if(t.scan_.objects_.empty()) {
			for(size_t i = 0; i < numFriends; ++i) {
				t.scan_.makeScanObject(FRIEND,NO_VECTOR2D,NO_COORD,NO_VECTOR2D);
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
			t.scan_.makeScanObject(ENEMY, ce, t.distance(ce), t.vel_);
		}

		if(t.scan_.objects_.size() > (numFriends + numEnemies)) {
			t.scan_.objects_.resize(numFriends + numEnemies);
		}

		if(t.scan_.objects_.size() == numFriends)
			t.scan_.makeScanObject(ENEMY, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);

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

		size_t startNum = (numFriends + numEnemies);
		size_t targetNum = (numFriends + numEnemies + numFriendFacilities);
		findNearestN(bspFriendFacilities, t, FRIEND_FACILITY, t.scan_.objects_, numFriendFacilities);

		if(t.scan_.objects_.size() > targetNum) {
			t.scan_.objects_.resize(targetNum);
		} else if(t.scan_.objects_.size() == startNum) {
			for(size_t i = startNum; i < targetNum; ++i) {
				t.scan_.makeScanObject(FRIEND_FACILITY,NO_VECTOR2D,NO_COORD, NO_VECTOR2D);
			}
		}

		while(t.scan_.objects_.size() < targetNum) {
			size_t s = t.scan_.objects_.size();
			for(size_t i = startNum; i < s; ++i) {
				ScanObject& so = t.scan_.objects_[i];
				assert(so.type_ == FRIEND_FACILITY);
				t.scan_.objects_.push_back(so);
				if(t.scan_.objects_.size() == targetNum)
					break;
			}
		}

		assert(t.scan_.objects_.size() == targetNum);

		startNum = (numFriends + numEnemies + numFriendFacilities);
		targetNum = (numFriends + numEnemies + numFriendFacilities + numEnemyFacilities);
		findNearestN(bspEnemyFacilities, t, ENEMY_FACILITY, t.scan_.objects_, numEnemyFacilities);

		if(t.scan_.objects_.size() > targetNum) {
			t.scan_.objects_.resize(targetNum);
		} else if(t.scan_.objects_.size() == startNum) {
			for(size_t i = startNum; i < targetNum; ++i) {
				t.scan_.makeScanObject(ENEMY_FACILITY,NO_VECTOR2D,NO_COORD, NO_VECTOR2D);
			}
		}

		while(t.scan_.objects_.size() < targetNum) {
			size_t s = t.scan_.objects_.size();
			for(size_t i = startNum; i < s; ++i) {
				ScanObject& so = t.scan_.objects_[i];
				assert(so.type_ == ENEMY_FACILITY);
				t.scan_.objects_.push_back(so);
				if(t.scan_.objects_.size() == targetNum)
					break;
			}
		}

		assert(t.scan_.objects_.size() == targetNum);

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

			p->scan_ = Scan(p);

			auto result = findNearest(bspEnemies, *p);
			assert(p->scan_.objects_.empty() || p->scan_.objects_.size() == 2);

			if(result.first != NULL && result.second != NO_COORD) {
				Vector2D enemyLoc = result.first->loc_;
				if(p->scan_.objects_.empty()) {
					p->scan_.makeScanObject(ENEMY, enemyLoc, result.second, NO_VECTOR2D);
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[0].loc_ = enemyLoc;
					p->scan_.objects_[0].dis_ = result.second;
				}
			} else {
				p->scan_.makeScanObject(ENEMY, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);
			}

			result = findNearest(bspFriends, *p);
			if(result.first != NULL && result.second != NO_COORD) {
				Vector2D friendLoc = result.first->loc_;
				if(p->scan_.objects_.size() == 1) {
					p->scan_.makeScanObject(FRIEND, friendLoc, result.second, NO_VECTOR2D);
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[1].loc_ = friendLoc;
					p->scan_.objects_[1].dis_ = result.second;
				}
			} else {
				p->scan_.makeScanObject(FRIEND, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);
			}

			p->scan_.calculate(bfl);
		}

		t.scan_.calculate(bfl);
	}
}

void HybridScanner::scan(BattleField& field) {
	assert(field.teams_.size() == 2);
	prepare(field);
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];
	teamScan(teamA, teamB, centersA_, centersB_, bspA_, bspB_, bspFA_, bspFB_, bspPB_, field.layout_);
	teamScan(teamB, teamA, centersB_, centersA_, bspB_, bspA_, bspFB_, bspFA_, bspPA_, field.layout_);


	vector<Object*> objsA;
	vector<Object*> objsB;

	for(Facility& f : teamA.facilities_) {
		findInRange(bspA_, f, objsA, f.layout_.range_);
		findInRange(bspB_, f, objsB, f.layout_.range_);
		signed long diff = objsA.size() - objsB.size();
		if(diff >= 3) {
			if(f.teamID_ != 0) {
				for(Object* o: objsA) {
					static_cast<Ship*>(o)->capture();
				}
			}
			f.teamID_ = 0;
		} else if(diff <= -3) {
			if(f.teamID_ != 1) {
				for(Object* o: objsB) {
					static_cast<Ship*>(o)->capture();
				}
			}
			f.teamID_ = 1;
		}

		if(f.teamID_ == 0) {
			for(Object* o: objsA) {
				static_cast<Ship*>(o)->recharge();
			}
		} else {
			for(Object* o: objsB) {
				static_cast<Ship*>(o)->recharge();
			}
		}
		objsA.clear();
		objsB.clear();
	}

	for(Facility& f : teamB.facilities_) {
		findInRange(bspA_, f, objsA, f.layout_.range_);
		findInRange(bspB_, f, objsB, f.layout_.range_);
		signed long diff = objsA.size() - objsB.size();
		if(diff >= 1) {
			if(f.teamID_ != 0) {
				for(Object* o: objsA) {
					static_cast<Ship*>(o)->capture();
				}
			}
			f.teamID_ = 0;
		} else if(diff <= -1) {
			if(f.teamID_ != 1) {
				for(Object* o: objsB) {
					static_cast<Ship*>(o)->capture();
				}
			}
			f.teamID_ = 1;
		}

		if(f.teamID_ == 0) {
			for(Object* o: objsA) {
				static_cast<Ship*>(o)->recharge();
			}
		} else {
			for(Object* o: objsB) {
				static_cast<Ship*>(o)->recharge();
			}
		}
		objsA.clear();
		objsB.clear();
	}
}

void HybridScanner::prepare(BattleField& field) {
	BspScanner::prepare(field);
	ClusterScanner::prepare(field);
}

} /* namespace neurocid */
