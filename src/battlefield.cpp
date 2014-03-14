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

BattleField::BattleField(BattleFieldLayout& bl, vector<Population>& teams) :
		bfl_(bl),
		teams_(teams) {
}

void BattleField::moveTeamTanks(Population& team) {
	for(Tank& t : team) {
		if(!t.dead_) {
			t.move(bfl_);
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
				p.move(bfl_);
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

void BattleField::collide(Projectile& p1, Projectile& p2) {
	if (!p1.dead_ && !p2.dead_ && p1.owner_->teamID_ != p2.owner_->teamID_) {
		Coord distance = p1.distance(p2);

		if(distance <= (p1.range_ + p2.range_)) {
			p1.dead_ = true;
			p2.dead_ = true;
		}
	}
}

void BattleField::collide(Projectile& p, Tank& t) {
	if (!p.dead_ && !t.dead_ && t != (*p.owner_)) {
		Coord distance = p.distance(t);

		if (distance <= (p.range_ + t.range_)) {
			p.dead_ = true;
			t.damage_++;
			if (t.damage_ >= t.tl_.max_damage_) {
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
}

void BattleField::calculateHits(Projectile& p) {
	if(p.nearestObject_->type() == ObjectType::TANK) {
		collide(p,*static_cast<Tank*>(p.nearestObject_));
	} else if(p.nearestObject_->type() == ObjectType::PROJECTILE) {
		collide(p,*static_cast<Projectile*>(p.nearestObject_));
	}
}

void BattleField::initializeTankScanner(Tank& ta) {
	assert(!ta.dead_);

	Tank* nearest = NULL;
	Tank* nearestEnemy = NULL;
	Tank* nearestFriend = NULL;
	Tank* nearestFriend2 = NULL;

	auto result =
			bsp_.find_nearest_if(&ta, std::numeric_limits<Coord>().max(),
					[&](Object* o) {
						Tank* t = NULL;
						//using dynamic_cast is slow
						if(o->type() == ObjectType::TANK) {
							t = static_cast<Tank*>(o);

							//skip self and dead
							if(t->dead_ || (*t) == ta)
								return false;

							//assert we are not encountering the same tank twice
							assert(t != nearest);
							assert(t != nearestEnemy);
							assert(t != nearestFriend);
							assert(t != nearestFriend2);

						    //assign the first encounter
							if(nearest == NULL) {
								nearest = t;
							}

							if(t->teamID_ != ta.teamID_) {
								if(nearestEnemy == NULL) {
									nearestEnemy = t;
								}
							} else {
								if(nearestFriend == NULL) {
									nearestFriend = t;
								} else if(nearestFriend2 == NULL) {
									nearestFriend2 = t;
								}
							}

							return nearestEnemy != NULL && nearestFriend != NULL && nearestFriend2 != NULL;
						}

						return false;
					});

/*	if(result.second == std::numeric_limits<Coord>().max()) {
		std::cerr << "Team:" << ta.teamID_ << endl;
		std::cerr << "Dead:" << endl;
		for(Population& p : teams_)
			std::cerr << p.countDead() << std::endl;
		std::cerr << endl;
		assert(false);
	}*/

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

void BattleField::initializeTankScanners() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			if(!t.dead_)
				initializeTankScanner(t);
		}
	}
}

void BattleField::updateScanner(Projectile& p) {
	Object* nearest = NULL;
	Tank* nearestEnemy = NULL;
	Tank* nearestFriend = NULL;

	/*
	size_t found0 = 0;
	size_t found1 = 0;
	for(Object* o: bsp_) {
		Tank* t = NULL;
		if(o->type() == ObjectType::TANK)
			t = static_cast<Tank*>(o);
		else
			continue;

		if(t->teamID_ == 0)
			++found0;
		else if(t->teamID_ == 1)
			++found1;
		else
			assert(false);
	}

	assert(found0 == found1);
	assert(found0 == teams_[0].size());
	assert(found1 == teams_[1].size());
*/
	auto result =
			bsp_.find_nearest_if(&p, std::numeric_limits<Coord>().max(),
					[&](Object* o) {
						//skip self and dead
						if(o->dead_ || o == p.owner_)
							return false;

						assert(o != nearest);

						//assign the first encounter
						if(nearest == NULL) {
							nearest = o;
						}

						Tank* t = NULL;
						//using dynamic_cast is slow
						if(o->type() == ObjectType::TANK) {
							t = static_cast<Tank*>(o);

							//assert we are not encountering the same tank twice
							assert(t != nearestEnemy);
							assert(t != nearestFriend);

							if(t->teamID_ != p.owner_->teamID_) {
								if(nearestEnemy == NULL) {
									nearestEnemy = t;
								}
							} else if(nearestFriend == NULL) {
								nearestFriend = t;
							}

							return nearestEnemy != NULL && nearestFriend != NULL;
						}

						return false;
					});

/*	Coord max = std::numeric_limits<Coord>().max();

 	if(nearestFriend == NULL) {
		std::cerr << "dead friends:" << teams_[p.owner_->teamID_].countDead() << std::endl;
	}

	assert(nearest != NULL);
	assert(nearestFriend != NULL);
	assert(nearestEnemy != NULL);
	assert(result.second != max);*/

	if(nearest != NULL)
		p.nearestObject_ = nearest;

	if (nearestEnemy != NULL) {
		Coord enemyDistance = p.distance(*nearestEnemy);
		assert(enemyDistance > 0);
		if (p.nearestEnemyDis_ > enemyDistance) {
			p.nearestEnemyLoc_ = nearestEnemy->loc_;
			p.nearestEnemyDis_ = enemyDistance;
		}
	}

	if(nearestFriend != NULL) {
		Coord friendDistance = p.distance(*nearestFriend);
		assert(friendDistance > 0);
		if(p.nearestFriendDis_ > friendDistance) {
			p.nearestFriendLoc_ = nearestFriend->loc_;
			p.nearestFriendDis_  = friendDistance;
		}
	}
}

void BattleField::calculateHits(Projectile& p, Bsp::NodeVector inRange) {
	Tank* to;
	Projectile* po;
	for (size_t i = 0; i < inRange.size(); ++i) {
		if ((to = dynamic_cast<Tank*>(inRange[i]))) {
			collide(p, *to);
		} else if ((po = dynamic_cast<Projectile*>(inRange[i]))) {
			collide(p, *po);
		}
	}
}

void BattleField::checkHits() {
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		for(size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			Bsp::NodeVector result;

			//find nearest tanks before calculating hits or we might up with a projectile without the information
			#pragma omp parallel for
			for(size_t k = 0; k < t.projectiles_.size(); ++k) {
				Projectile& p = t.projectiles_[k];

				if(p.dead_)
					continue;

				updateScanner(p);
			}

			for (size_t k = 0; k < t.projectiles_.size(); ++k) {
				Projectile& p = t.projectiles_[k];

				if (p.dead_)
					continue;

				bsp_.find_within_range(&p, p.range_ + t.range_, std::back_inserter(result));
				calculateHits(p, result);
				result.clear();

				//FIXME collision detection using only this doesn't work
				//calculateHits(p);
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
			team[j].think(bfl_);
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
	if(GameState::getInstance()->isRunning()) initializeTankScanners();
	if(GameState::getInstance()->isRunning()) checkHits();
	if(GameState::getInstance()->isRunning()) letTanksThink();
	cleanup();
}

} /* namespace tankwar */
