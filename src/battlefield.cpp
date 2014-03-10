/*
 * Battlefield.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */


#include "battlefield.hpp"
#include "population.hpp"
#include "gamestate.hpp"
#include "bsp.hpp"

#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>

namespace tankwar {
using std::vector;
using std::list;
using std::cerr;
using std::endl;

BattleField::BattleField(vector<Population>& teams) : teams_(teams) {
}

void BattleField::moveTeamTanks(Population& team) {
	for(Tank& t : team) {
		if(!t.dead_) {
			t.move();
		}
	}
}

//FIXME make move order random
void BattleField::moveTanks() {
	for(Population& team : teams_) {
		moveTeamTanks(team);
	}
}

void BattleField::moveTeamProjectiles(Population& team) {
	for(Tank& t : team) {
		for(size_t j = 0; j < t.projectiles_.size(); ++j) {
			Projectile& p = t.projectiles_[j];
			if(!p.dead_)
				p.move();
		}
	}
}

void BattleField::buildBsp() {
	for (size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		for (size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			bsp_.insert(&t);
			for (Projectile& p : t.projectiles_) {
				bsp_.insert(&p);
			}
		}
	}
}

void BattleField::moveProjectiles() {
	for(Population& team : teams_) {
		moveTeamProjectiles(team);
	}
}

void BattleField::calculateHit(Projectile& p1, Projectile& p2) {
	if (!p1.dead_ && !p2.dead_ && p1.owner_->teamID_ != p2.owner_->teamID_) {
		Coord distance = p1.distance(p2);
		if (distance < (Params::PROJECTILE_RANGE * 2)){
			p1.dead_ = true;
			p2.dead_ = true;
		}
	}
}

void BattleField::calculateHit(Projectile& p, Tank& t) {
	if (!p.dead_ && !t.dead_ && t != (*p.owner_)) {
		p.dead_ = true;
		t.damage_++;
		if (t.damage_ >= Params::MAX_DAMAGE) {
			t.dead_ = true;
		}

		if (p.owner_->teamID_ != t.teamID_) {
			p.owner_->hits_++;
			p.enemyHitter_ = true;
		} else {
			p.owner_->friendly_fire_++;
			p.friendHitter_ = true;
		}
	}
}

void BattleField::calculateHits(Projectile& p, Bsp::NodeVector inRange) {
	Tank* to;
	Projectile* po;
	for(size_t i = 0; i < inRange.size(); ++i) {
		if((to = dynamic_cast<Tank*>(inRange[i]))) {
			calculateHit(p,*to);
		} else if((po = dynamic_cast<Projectile*>(inRange[i]))) {
			calculateHit(p,*po);
		}
	}
}

void BattleField::initializeScanners(Tank& ta) {
	Tank* nearest = NULL;
	Tank* nearestEnemy = NULL;
	Tank* nearestFriend = NULL;
	Tank* nearestFriend2 = NULL;

	auto result =
			bsp_.find_nearest_if(&ta, std::numeric_limits<Coord>().max(),
					[&](Object* o) {
						Tank* t;
						//using dynamic_cast is slow
						if(o->type() == ObjectType::TANK && !t->dead_ && (*t) != ta) {
							t = static_cast<Tank*>(o);
							//assert we are not encountering the same tank twice
							assert(t != nearest);
							assert(t != nearestEnemy);
							assert(t != nearestFriend);
							assert(t != nearestFriend2);

							if(nearest == NULL) {
								nearest = t;
							} else if(t->teamID_ != ta.teamID_) {
								if(nearestEnemy == NULL) {
									nearestEnemy = t;
								}
							} else {
								if(nearestFriend == NULL) {
									nearestFriend = t;
								} else if(nearestFriend2 == NULL) {
									nearestFriend2 == t;
								}
							}

							return nearestEnemy != NULL && nearestFriend2 != NULL && nearestFriend2 != NULL;
						} else
						return false;
					});

	assert(result.second != std::numeric_limits<Coord>().max());

	if(nearest != NULL) {
		ta.scanner_.nearestLoc_ = nearest->loc_;
		ta.scanner_.nearestDis_ = ta.distance(*nearest);
	}

	if (nearestEnemy != NULL)
		ta.scanner_.nearestEnemyLoc_ = nearestEnemy->loc_;

	if (nearestFriend != NULL)
		ta.scanner_.nearestFriendLoc_ = nearestFriend->loc_;

	if (nearestFriend2 != NULL)
		ta.scanner_.nearestFriend2Loc_ = nearestFriend2->loc_;
}

void BattleField::initializeScanners() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			if(!t.dead_)
				initializeScanners(t);
		}
	}
}

void BattleField::stepBack() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			if(!t.dead_) {
				//std::cerr << "nearest:" << nearestTank.second << std::endl;

				if(t.scanner_.nearestDis_ < (Params::TANK_RANGE * 2)) {
					t.stepBack();
					//std::cerr << "step back" << std::endl;
				}
			}
		}
	}
}

void BattleField::findNearestTanks(Projectile& p) {
	auto nearestEnemy = bsp_.find_nearest_if(&p,std::numeric_limits<Coord>().max(),[&](Object* o){
		Tank* t;
		if((t = dynamic_cast<Tank*>(o)) && !t->dead_ && t->teamID_ != p.owner_->teamID_) {
			return true;
		} else
			return false;
	});

	assert(nearestEnemy.second != std::numeric_limits<Coord>().max());

	if(p.nearestEnemyDis_ > nearestEnemy.second) {
		p.nearestEnemyLoc_ = (*nearestEnemy.first)->loc_;
		p.nearestEnemyDis_  = nearestEnemy.second;
	}

	auto nearestFriend = bsp_.find_nearest_if(&p,std::numeric_limits<Coord>().max(),[&](Object* o){
		Tank* t;
		if((t = dynamic_cast<Tank*>(o)) && !t->dead_ && (*t) != (*p.owner_) && t->teamID_ == p.owner_->teamID_) {
			return true;
		} else
			return false;
	});

	assert(nearestFriend.second != std::numeric_limits<Coord>().max());

	if(p.nearestFriendDis_ > nearestFriend.second) {
		p.nearestFriendLoc_ = (*nearestFriend.first)->loc_;
		p.nearestFriendDis_  = nearestFriend.second;
	}
}

void BattleField::checkHits() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		#pragma omp parallel
		for(size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			Bsp::NodeVector result;

			#pragma omp for ordered schedule(dynamic)
			for(size_t k = 0; k < t.projectiles_.size(); ++k) {
				Projectile& p = t.projectiles_[k];

				if(p.dead_)
					continue;

				findNearestTanks(p);
				bsp_.find_within_range(&p, p.range_ + Params::TANK_RANGE, std::back_inserter(result));

				#pragma omp ordered
				calculateHits(p, result);
				result.clear();
			}
		}
	}
}

void BattleField::letTanksThink() {
	#pragma omp parallel
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		#pragma omp for
		for(size_t j = 0; j < team.size(); ++j) {
			team[j].think(*this);
		}
	}
}

void BattleField::cleanup() {
	bsp_.clear();
}

void BattleField::step() {
	if(GameState::getInstance()->isRunning()) moveTanks();
	if(GameState::getInstance()->isRunning()) moveProjectiles();
	if(GameState::getInstance()->isRunning()) buildBsp();
	if(GameState::getInstance()->isRunning()) initializeScanners();
	if(GameState::getInstance()->isRunning()) stepBack();
	if(GameState::getInstance()->isRunning()) checkHits();
	if(GameState::getInstance()->isRunning()) letTanksThink();
	cleanup();
}

} /* namespace tankwar */
