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
		for (Projectile& p : t.projectiles_) {
			if(!p.dead_)
				bspPA_.insert(&p);
		}
	}

	for (size_t i = 0; i < teamB.size(); ++i) {
		Tank& t = teamB[i];
		if(!t.dead_)
			bspB_.insert(&t);
		for (Projectile& p : t.projectiles_) {
			if(!p.dead_)
				bspPB_.insert(&p);
		}
	}
}

void Scanner::transform(Scan& scan) {
	Vector2D& dir = scan.dir_;

	for (ScanObject so : scan.objects_) {
		if (so.normLoc_ == NO_VECTOR2D) {
			//not found
			continue;
		} else {
			if (so.type_ == ENEMY) {
				// the closer the is to dir the closer it's output is to zero (+ = left, - = rights)
				double diff = (M_PI + radFromDir(dir))
						- (M_PI + radFromDir(so.normLoc_));

				if (diff > M_PI)
					diff = M_PI - diff;
				else if (diff < -M_PI)
					diff = M_PI + diff;

				//		std::cerr << "diff:" << diff << std::endl;

				assert(diff > -M_PI && diff < M_PI);
				so.normLoc_ = dirFromRad(diff).x;
				so.normLoc_ = dirFromRad(diff).y;
			} else if (so.type_ == FRIEND) {
				// the closer it is to one of the perpendicular vectors to dir the closer it's output is to zero (+ = left, - = rights)
				double diff = (M_PI + radFromDir(dir))
						- (M_PI + radFromDir(so.normLoc_));

				if (diff > M_PI)
					diff = M_PI - diff;
				else if (diff < -M_PI)
					diff = M_PI + diff;

				if (diff < 0)
					diff = (M_PI / 2) + diff;
				else if (diff > 0)
					diff = (M_PI / 2) - diff;

				//		std::cerr << "diff:" << diff << std::endl;

				assert(diff > -M_PI && diff < M_PI);
				so.normLoc_ = dirFromRad(diff).x;
				so.normLoc_ = dirFromRad(diff).y;
			}
		}
	}
}

std::pair<Object*,Coord> Scanner::findNearest(Bsp& bsp, Object& from) {
	assert(!from.dead_);
	vector<Object*> found;
	auto result = bsp.find_nearest(&from, std::numeric_limits<Coord>().max());
	return {*result.first, result.second};
}


void Scanner::pushBackNearest(Bsp& bsp, Object& from, ScanObjectType type, Scan& scan) {
	auto result = findNearest(bsp, from);
	Tank* enemy = static_cast<Tank*>(result.first);

	if (result.second != NO_COORD) {
		scan.objects_.push_back(ScanObject {
					ENEMY,
					(enemy->loc_ - from.loc_).normalize(scan.scale_.x, scan.scale_.y),
					result.second
				});
	} else {
		scan.objects_.push_back(ScanObject {
					ENEMY,
					NO_VECTOR2D,
					NO_COORD
				});
	}
}

void Scanner::teamScan(Population& active, Population& passive, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl) {
	for(size_t i = 0; i < active.size(); ++i) {
		Tank& t = active[i];
		Scan& scan = t.scan_;
		scan.dir_ = t.getDirection();
		scan.scale_ = {bfl.width_, bfl.height_};
		scan.objects_.clear();

		for(Tank& enemy : passive) {
			scan.objects_.push_back(ScanObject {
							ENEMY,
							(enemy.loc_ - t.loc_).normalize(scan.scale_.x, scan.scale_.y),
							NO_COORD
						});
		}

		for(Projectile& p : t.projectiles_) {
			auto result = findNearest(bspEnemies, t);
			Tank* penemy = static_cast<Tank*>(result.first);
			assert(p.scan_.objects_.empty() || p.scan_.objects_.size() == 2);

			if(result.second != NO_COORD) {
				if(p.scan_.objects_.empty()) {
					p.scan_.objects_.push_back(ScanObject{
						ENEMY,
						(penemy->loc_ - p.startLoc_).normalize(),
						result.second
					});
				} else if(result.second < p.scan_.objects_[0].normDis_){
					p.scan_.objects_[0].normLoc_ = (penemy->loc_ - p.startLoc_).normalize();
					p.scan_.objects_[0].normDis_ = result.second;
				}
			} else {
				p.scan_.objects_.push_back(ScanObject{
					ENEMY,
					NO_VECTOR2D,
					NO_COORD
				});
			}

			result = findNearest(bspFriends, t);
			Tank* pfriend = static_cast<Tank*>(result.first);
			if (result.second != NO_COORD) {
				if(p.scan_.objects_.size() == 1) {
					p.scan_.objects_.push_back(ScanObject {
								FRIEND,
								(pfriend->loc_ - p.startLoc_).normalize(),
								result.second
							});
				} else if(result.second < p.scan_.objects_[1].normDis_) {
					p.scan_.objects_[1].normLoc_ = (penemy->loc_ - p.startLoc_).normalize();
					p.scan_.objects_[1].normDis_ = result.second;
				}
			} else {
				p.scan_.objects_.push_back(ScanObject {
							ENEMY,
							NO_VECTOR2D,
							NO_COORD
						});
			}

		}
		transform(scan);
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
