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

//FIXME make move order random
void BattleField::moveTanks() {
	for(Tank& t : teamA_) {
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

			if(t.willShoot()) {
				projectiles_.push_back(Projectile(t, t.loc_, t.dir_));
			}
		}
	}

	for(Tank& t : teamB_) {
		if(!t.dead_) {
			//Tank clone = t;
			t.move();
/*
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

			if(t.willShoot()) {
				projectiles_.push_back(Projectile(t, t.loc_, t.dir_));
			}
		}
	}
}

void BattleField::moveProjectiles() {
	for(Projectile& p : projectiles_) {
		if(!p.dead_)
			p.move();
	}
}

void BattleField::checkTeamHits(Population& team) {
	for(Projectile& p : projectiles_) {
		if(p.dead_)
			continue;

		for(Tank& t : team) {
			if(!t.dead_ && p.collides(t) && t != p.owner_) {
				p.dead_ = true;
				t.damage_++;
				if(p.owner_.teamID_ == t.teamID_) {
					p.owner_.friendly_fire_++;
					if(t.damage_ >= Params::MAX_DAMAGE) {
						t.dead_ = true;
					}
				}
				else {
					p.owner_.hits_++;
					if(t.damage_ >= Params::MAX_DAMAGE) {
						t.dead_ = true;
					}
				}
			}
		}
	}
}

void BattleField::checkHits() {
	checkTeamHits(teamA_);
	checkTeamHits(teamB_);
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
