#ifndef BATTLEFIELD_HPP_
#define BATTLEFIELD_HPP_

#include "projectile.hpp"
#include "canvas.hpp"
#include <vector>

namespace tankwar {

using std::vector;

class Population;

class BattleField {
public:
	vector<Population>&  teams_;

	BattleField(vector<Population>& teams);

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
