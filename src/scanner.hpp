/*
 * scanner1.hpp
 *
 *  Created on: Mar 14, 2014
 *      Author: elchaschab
 */

#ifndef SCANNER1_HPP_
#define SCANNER1_HPP_

#include <limits>
#include <vector>
#include "2d.hpp"
#include "bsp.hpp"

namespace tankwar {
using std::numeric_limits;
using std::vector;

class BattleField;
class Population;

enum ScanObjectType {
	FRIEND,
	ENEMY,
	PROJECTILE_
};

struct ScanObject {
	ScanObjectType type_;
	Vector2D normLoc_;
	Coord normDis_;
};

struct Scan {
	Vector2D scale_ = NO_VECTOR2D;
	Vector2D dir_ = NO_VECTOR2D;
	vector<ScanObject> objects_;
};

class Scanner {
private:
	Bsp bspA_;
	Bsp bspB_;
	Bsp bspPA_;
	Bsp bspPB_;

	std::pair<Object*,Coord> findNearest(Bsp& bsp, Object& from);
	void pushBackNearest(Bsp& bsp, Object& from, ScanObjectType type, Scan& scan);
	void teamScan(Population& active, Population& passive, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl);
	void buildBsps(BattleField& field);
	void transform(Scan& scan);
public:
	void scan(BattleField& field);
};

} /* namespace tankwar */

#endif /* SCANNER1_HPP_ */
