#ifndef BATTLEFIELD_HPP_
#define BATTLEFIELD_HPP_

#include "physics.hpp"
#include "scanner.hpp"
#include "battlefieldlayout.hpp"
#include "scenario.hpp"
#include <vector>

namespace neurocid {

using std::vector;

class Population;

class BattleField {
public:
	BattleFieldLayout layout_;
	Physics physics_;
	Scanner scanner_;
	vector<Population>&  teams_;
	vector<Projectile*> spawned_;

	BattleField(Scenario* scenario, vector<Population>& teams);
	void step();
private:
	void scan();
	void think();
	void move();
	void cleanup();
};

} /* namespace neurocid */

#endif /* BATTLEFIELD_HPP_ */
