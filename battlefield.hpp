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
	std::list<Projectile> projectiles_;
	Population& teamA_;
	Population& teamB_;

	BattleField(Canvas& canvas, Population& teamA, Population& teamB);

	void step();
private:
	void moveTanks();
	void moveProjectiles();
	void checkTeamHits(Population& team);
	void checkHits();
	void letTanksThink();
};

} /* namespace tankwar */

#endif /* BATTLEFIELD_HPP_ */
