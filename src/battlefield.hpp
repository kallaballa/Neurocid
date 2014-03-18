#ifndef BATTLEFIELD_HPP_
#define BATTLEFIELD_HPP_

#include "physics.hpp"
#include "scanner.hpp"
#include <vector>

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
	BattleFieldLayout layout_;
	vector<Population>&  teams_;
	Physics physics_;
	Scanner scanner_;

	BattleField(BattleFieldLayout& bfl, PhysicsLayout& pl, vector<Population>& teams);
	void step();
private:
	void scan();
	void think();
	void move();
	void cleanup();
};

} /* namespace tankwar */

#endif /* BATTLEFIELD_HPP_ */
