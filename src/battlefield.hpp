#ifndef BATTLEFIELD_HPP_
#define BATTLEFIELD_HPP_

#include "projectile.hpp"
#include "canvas.hpp"
#include <vector>
#include "bsp.hpp"

namespace tankwar {

using std::vector;

class Population;

class BattleField {
public:
	vector<Population>&  teams_;
	Bsp bsp_;

	BattleField(vector<Population>& teams);

	void step();
private:
	void moveTeamTanks(Population& team);
	void moveTanks();
	void initializeTankScanners();
	void stepBack();
	void moveTeamProjectiles(Population& team);
	void moveProjectiles();
	void buildBsp();
	void initializeTankScanner(Tank& ta);
	void updateScanner(Projectile& p);
	void collide(Projectile& p1, Projectile& p2);
	void collide(Projectile& p, Tank& t);
	void calculateHits(Projectile& t);
	void calculateHits(Projectile& p, Bsp::NodeVector inRange);
	void checkHits();
	void letTanksThink();
	void cleanup();
};

} /* namespace tankwar */

#endif /* BATTLEFIELD_HPP_ */
