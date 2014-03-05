/*
 * Battlefield.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */


#include "battlefield.hpp"
#include "population.hpp"
#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>

namespace tankwar {
using std::vector;
using std::list;
using std::cerr;
using std::endl;

BattleField::BattleField(Canvas& canvas, Population& teamA_, Population& teamB_) : canvas_(canvas), teamA_(teamA_), teamB_(teamB_) {
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
	moveTeamTanks(teamA_);
	moveTeamTanks(teamB_);
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
	moveTeamProjectiles(teamA_);
	moveTeamProjectiles(teamB_);
}


void BattleField::checkHit(Tank& t, Projectile& p) {
	if (!p.dead_ && !t.dead_ && t != (*p.owner_)) {
		if (!p.owner_->teamID_ != t.teamID_) {
			Coord distance = p.distance(t);
			if (distance < p.nearestEnemyDis_) {
				p.nearestEnemyDis_ = distance;
				p.nearestEnemyLoc_ = t.loc_;
			}
		}

		if (p.collides(t)) {
			p.dead_ = true;
			t.damage_++;
			if (p.owner_->teamID_ == t.teamID_) {
				p.owner_->friendly_fire_++;
				if (t.damage_ >= Params::MAX_DAMAGE) {
					t.dead_ = true;
				}
			} else {
				p.owner_->hits_++;
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
	for (Tank& a : attacker) {
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
	checkTeamHits(teamA_, teamB_);
	checkTeamHits(teamB_, teamA_);
}

//moves all tanks in random order
void BattleField::letTanksThink() {
	for(Tank& t : teamA_) {
		t.think(*this);
	}

	for(Tank& t : teamB_) {
		t.think(*this);
	}
}

void BattleField::step() {
	moveTanks();
	moveProjectiles();
	checkHits();
    letTanksThink();
}

} /* namespace tankwar */
