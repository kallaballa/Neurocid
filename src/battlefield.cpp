/*
 * Battlefield.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */


#include "battlefield.hpp"
#include "population.hpp"
#include "gamestate.hpp"

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
			/*Tank clone = t;
			clone.move();

			bool stepBack = false;
			for(Tank& ta : teamA_) {
				if(!ta.dead_ && ta.collides(clone)) {
					stepBack = true;
					break;
				}
			}

			if(!stepBack) {
				for(Tank& tb : teamA_) {
					if(!tb.dead_ && tb.collides(clone)) {
						stepBack = true;
						break;
					}
				}
			}

			if(!stepBack) {
				t = clone;
			}*/

			if(t.shoot()) {
				t.makeProjectile();
			}
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
		for(Projectile& p : t.projectiles_) {
			if(!p.dead_)
				p.move();
		}
	}
}

void BattleField::moveProjectiles() {
	for(Population& team : teams_) {
		moveTeamProjectiles(team);
	}
}

void BattleField::checkHit(Tank& t, Projectile& p) {
	if (!p.dead_ && !t.dead_ && t != (*p.owner_)) {
		if (p.owner_->teamID_ != t.teamID_) {
			Coord distance = p.distance(t);
			if (distance < p.nearestEnemyDis_) {
				p.nearestEnemyDis_ = distance;
				p.nearestEnemyLoc_ = t.loc_;
			}
		} else {
			Coord distance = p.distance(t);
			if (distance < p.nearestEnemyDis_) {
				p.nearestFriendDis_ = distance;
				p.nearestFriendLoc_ = t.loc_;
			}
		}

		if (p.collides(t)) {
			p.dead_ = true;
			t.damage_++;
			if (p.owner_->teamID_ == t.teamID_) {
				p.owner_->friendly_fire_++;
				p.friendHitter_ = true;
				p.nearestFriendDis_ = 0;
				p.nearestFriendLoc_ = t.loc_;
				if (t.damage_ >= Params::MAX_DAMAGE) {
					t.dead_ = true;
				}
			} else {
				p.owner_->hits_++;
				p.enemyHitter_ = true;
				p.nearestEnemyDis_ = 0;
				p.nearestEnemyLoc_ = t.loc_;
				if (t.damage_ >= Params::MAX_DAMAGE) {
					t.dead_ = true;
				}
			}
		}
	}
}

void BattleField::checkTeamHits(Population& attacker, Population& defender) {
	for(size_t i = 0; i < attacker.size(); ++i) {
		Tank& a = attacker[i];
		for (Projectile& p : a.projectiles_) {
			for (Tank& a : attacker) {
				checkHit(a,p);
			}

			for (Tank& d : defender) {
				checkHit(d,p);
			}
		}
	}
}

void BattleField::checkHits() {
	#pragma omp parallel for
	for(size_t i = 0; i < teams_.size(); ++i) {
		for(size_t j = 0; j < teams_.size(); ++j) {
			if(i == j)
				continue;

			checkTeamHits(teams_[i], teams_[j]);
		}
	}
}

void BattleField::letTanksThink() {
	#pragma omp parallel for
	for(size_t i = 0; i < teams_.size(); ++i) {
		Population& team = teams_[i];
		for(size_t i = 0; i < team.size(); ++i) {
			team[i].think(*this);
		}
	}
}

void BattleField::step() {
	if(GameState::getInstance()->isRunning()) moveTanks();
	if(GameState::getInstance()->isRunning()) moveProjectiles();
	if(GameState::getInstance()->isRunning()) checkHits();
	if(GameState::getInstance()->isRunning()) letTanksThink();
}

} /* namespace tankwar */
