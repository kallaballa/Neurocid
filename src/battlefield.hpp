#ifndef BATTLEFIELD_HPP_
#define BATTLEFIELD_HPP_

#include "projectile.hpp"
#include "canvas.hpp"
#include <vector>
#include "bsp.hpp"

namespace tankwar {

using std::vector;

class Population;

struct BattleFieldLayout {
	size_t iterations_;
	Coord width_;
	Coord height_;
};

class BattleField {
public:
	BattleFieldLayout bfl_;
	vector<Population>&  teams_;
	Bsp bsp_;

	BattleField(BattleFieldLayout& bl, vector<Population>& teams);
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
