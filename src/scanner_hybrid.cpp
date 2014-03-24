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
	for(size_t i = 0; i < active.size(); ++i) {
		Tank& t = active[i];
		if(t.dead_ || t.layout_.isDummy_)
			continue;

		Scan& scan = t.scan_;
		scan.dir_ = t.getDirection();
		scan.scale_ = {bfl.width_, bfl.height_};
		scan.loc_ = t.loc_;
		scan.objects_.clear();

		findInRange(bspFriends, t, FRIEND, scan.objects_, 300);
		Coord numFriends = ctrFriends.size();
		if(scan.objects_.size() > numFriends) {
			scan.objects_.resize(numFriends);
		} else if(scan.objects_.empty()) {
			for(size_t i = 0; i < numFriends; ++i) {
				scan.objects_.push_back(ScanObject{
					FRIEND,
					NO_VECTOR2D,
					NO_COORD
				});
			}
		}

		while(scan.objects_.size() < numFriends) {
			size_t s = scan.objects_.size();
			for(size_t i = 0; i < s; ++i) {
				ScanObject& so = scan.objects_[i];
				assert(so.type_ == FRIEND || so.type_ == ENEMY);
				scan.objects_.push_back(so);
				if(scan.objects_.size() == numFriends)
					break;
			}
		}

		assert(scan.objects_.size() == 10);

		for(Vector2D ce : ctrEnemies) {
			scan.objects_.push_back(ScanObject{
				ENEMY,
				ce,
				t.distance(ce)
			});
		}

		assert(scan.objects_.size() == 20);
		if(t.layout_.disableProjectileFitness_)
			continue;

		for(Projectile* p : t.projectiles_) {
			if(p->dead_)
				continue;

			p->scan_.dir_ = p->getDirection();
			p->scan_.loc_ = p->loc_;
			auto result = findNearestCenter(ctrEnemies, p->loc_);
			Vector2D enemyLoc = result.first;
			assert(p->scan_.objects_.empty() || p->scan_.objects_.size() == 2);

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.empty()) {
					p->scan_.objects_.push_back(ScanObject{
						ENEMY,
						enemyLoc,
						result.second
					});
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[0].loc_ = enemyLoc;
					p->scan_.objects_[0].dis_ = result.second;
				}
			} else {
				p->scan_.objects_.push_back(ScanObject{
					ENEMY,
					NO_VECTOR2D,
					NO_COORD
				});
			}

			result = findNearestCenter(ctrFriends, p->loc_);
			Vector2D friendLoc = result.first;

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.size() == 1) {
					p->scan_.objects_.push_back(ScanObject{
						FRIEND,
						friendLoc,
						result.second
					});
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[1].loc_ = friendLoc;
					p->scan_.objects_[1].dis_ = result.second;
				}
			} else {
				p->scan_.objects_.push_back(ScanObject{
					FRIEND,
					NO_VECTOR2D,
					NO_COORD
				});
			}
		}
	}
}

void HybridScanner::scan(BattleField& field) {
	assert(field.teams_.size() == 2);
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];

	buildBsps(field);
	centersA_.clear();
	centersB_.clear();
	scanClusterCenters(teamA, centersA_, 10);
	scanClusterCenters(teamB, centersB_, 10);

	assert(centersA_.size() == 10);
	assert(centersB_.size() == 10);

	teamScan(teamA, teamB, centersA_, centersB_, bspA_, bspB_, field.layout_);
	teamScan(teamB, teamA, centersB_, centersA_, bspB_, bspA_, field.layout_);
}

} /* namespace tankwar */
