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

namespace tankwar {

void Scanner::buildBsps(BattleField& field) {
	assert(field.teams_.size() == 2);
	bspA_.clear();
	bspB_.clear();
	bspPA_.clear();
	bspPB_.clear();

	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];

	for (size_t i = 0; i < teamA.size(); ++i) {
		Tank& t = teamA[i];
		if(!t.dead_)
			bspA_.insert(&t);
		for (Projectile* p : t.projectiles_) {
			if(!p->dead_)
				bspPA_.insert(p);
		}
	}

	for (size_t i = 0; i < teamB.size(); ++i) {
		Tank& t = teamB[i];
		if(!t.dead_)
			bspB_.insert(&t);
		for (Projectile* p : t.projectiles_) {
			if(!p->dead_)
				bspPB_.insert(p);
		}
	}
}

std::pair<Object*,Coord> Scanner::findNearest(Bsp& bsp, Object& from) {
	assert(!from.dead_);
	vector<Object*> found;
	auto result = bsp.find_nearest(&from, std::numeric_limits<Coord>().max());
	return {*result.first, result.second};
}

void Scanner::teamScan(Population& active, Population& passive, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl) {
	for(size_t i = 0; i < active.size(); ++i) {
		Tank& t = active[i];
		if(t.dead_)
			continue;

		Scan& scan = t.scan_;
		scan.dir_ = t.getDirection();
		scan.scale_ = {bfl.width_, bfl.height_};
		scan.loc_ = t.loc_;
		scan.objects_.clear();

		auto result = findNearest(bspEnemies, t);
		Tank* tenemy = static_cast<Tank*>(result.first);
		result = findNearest(bspFriends, t);
		Tank* tfriend = static_cast<Tank*>(result.first);

		scan.objects_.push_back(ScanObject {
						ENEMY,
						tenemy->loc_,
						result.second
					});

		scan.objects_.push_back(ScanObject {
								FRIEND,
								tfriend->loc_ ,
								result.second
							});

		for(Projectile* p : t.projectiles_) {
			if(p->dead_)
				continue;

			p->scan_.loc_ = p->loc_;
			auto result = findNearest(bspEnemies, *p);
			Tank* penemy = static_cast<Tank*>(result.first);
			assert(p->scan_.objects_.empty() || p->scan_.objects_.size() == 1);

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.empty()) {
					p->scan_.objects_.push_back(ScanObject{
						ENEMY,
						penemy->loc_,
						result.second
					});
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[0].loc_ = penemy->loc_;
					p->scan_.objects_[0].dis_ = result.second;
				}
			} else {
				p->scan_.objects_.push_back(ScanObject{
					ENEMY,
					NO_VECTOR2D,
					NO_COORD
				});
			}
		}
	}
}

void Scanner::scan(BattleField& field) {
	buildBsps(field);
	assert(field.teams_.size() == 2);
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];
	teamScan(teamA, teamB, bspA_, bspB_, field.layout_);
	teamScan(teamB, teamA, bspB_, bspA_, field.layout_);
}

} /* namespace tankwar */
