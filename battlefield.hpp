#ifndef BATTLEFIELD_HPP_
#define BATTLEFIELD_HPP_

#include "projectile.hpp"
#include "canvas.hpp"
#include <list>

namespace tankwar {

class Population;

class BattleField {

public:
	Canvas& canvas_;
	Population& teamA_;
	Population& teamB_;

	BattleField(Canvas& canvas, Population& teamA, Population& teamB);

	void step();
private:
	void moveTeamTanks(Population& team);
	void moveTanks();
	void moveTeamProjectiles(Population& team);
	void moveProjectiles();
	void checkHit(Tank& t, Projectile& p);
	void checkTeamHits(Population& attacker, Population& defender);
	void checkHits();
	void letTanksThink();
};

} /* namespace tankwar */

#endif /* BATTLEFIELD_HPP_ */
