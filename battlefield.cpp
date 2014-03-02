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
#include <thread>
#include <chrono>
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
		p.move();
	}
}


void BattleField::checkTeamHits(Population& team) {
	for(Projectile& p : projectiles_) {
		for(Tank& t : team) {
			if(!t.dead_ && p.collides(t) && t != p.owner_) {
				p.dead_ = true;
				t.damage_++;
				if(p.owner_.teamID_ == t.teamID_)
					p.owner_.friendly_fire++;
				else
					p.owner_.hits++;

				if(t.damage_ >= Params::MAX_DAMAGE) {
					cerr << "Dead" << endl;
					t.dead_ = true;
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

void BattleField::render() {
	Color white = {255,255,255};
	Color green = {0,255,0};
	Color purple = {255,0,255};
	Color red = {255,0,0};

	canvas_.clear();
	for(Tank& t : teamA_) {
		if(t.explode_)
			canvas_.drawExplosion(t, red);
		else if(!t.dead_)
			canvas_.drawTank(t, green);
		t.explode_ = false;
	}

	for(Tank& t : teamB_) {
		if(t.explode_)
			canvas_.drawExplosion(t, red);
		else if(!t.dead_)
			canvas_.drawTank(t, purple);
		t.explode_ = false;
	}

	for(Projectile& p : projectiles_) {
		if(p.explode_)
			canvas_.drawExplosion(p, red);
		else if(!p.dead_)
			canvas_.drawProjectile(p,white);
		p.explode_ = false;
	}
	canvas_.update();
}

void BattleField::step() {
	moveTanks();
	moveProjectiles();
	checkHits();
	render();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	letTanksThink();
}

} /* namespace tankwar */
