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

BattleField::BattleField(BattleFieldLayout& bfl, PhysicsLayout& pl, vector<Population>& teams) :
		layout_(bfl),
		teams_(teams),
		bsp_(),
		physics_(pl) {
	physics_.create(*this);
}

void BattleField::move() {
	//mpve tanks and update the simulation with spawned projectiles
	vector<Projectile*> spawned;

	for(size_t i = 0; i < teams_.size(); ++i)  {
		Population& team  = teams_[i];
		for(size_t j = 0; j < team.size(); ++j)  {
			Tank& t = team[j];
			t.move(layout_);
			if(t.willShoot()) {
				Projectile* p = t.shoot();
				spawned.push_back(p);
			}
		}
	}

	physics_.update(spawned);
	physics_.step();
    spawned.clear();
}

void BattleField::buildBsp() {
	for (size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		for (size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			bsp_.insert(&t);
			for (Projectile* p : t.projectiles_) {
				bsp_.insert(p);
			}
		}
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
	#pragma omp parallel
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		#pragma omp for
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

void BattleField::checkHits() {
	#pragma omp parallel
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];

		for(size_t j = 0; j < team.size(); ++j) {
			Tank& t = team[j];
			Bsp::NodeVector result;

			//find nearest tanks before calculating hits or we might up with a projectile without the information
			#pragma omp for
			for(size_t k = 0; k < t.projectiles_.size(); ++k) {
				Projectile* p = t.projectiles_[k];

				if(p->dead_)
					continue;

				updateScanner(*p);
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
			team[j].think(layout_);
		}
	}
}

void BattleField::cleanup() {
  bsp_.clear();
}

void BattleField::step() {
	if(GameState::getInstance()->isRunning()) buildBsp();
	if(GameState::getInstance()->isRunning()) initializeTankScanners();
	if(GameState::getInstance()->isRunning()) checkHits();
	if(GameState::getInstance()->isRunning()) letTanksThink();
	cleanup();
	if(GameState::getInstance()->isRunning()) move();
}

} /* namespace tankwar */
