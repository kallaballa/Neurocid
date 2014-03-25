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
#include "KMlocal.h"

namespace tankwar {
using std::numeric_limits;
using std::vector;

class BattleField;
class Population;
class Tank;

enum ScanObjectType {
	FRIEND,
	ENEMY,
	PROJECTILE_
};

struct ScanObject {
	ScanObjectType type_;
	Vector2D loc_;
	Coord dis_;
};

typedef vector<ScanObject> ScanObjectVector;
struct Scan {
	Vector2D scale_ = NO_VECTOR2D;
	Vector2D dir_ = NO_VECTOR2D;
	Vector2D vel_ = NO_VECTOR2D;
	Coord angVel_ = NO_COORD;
	Vector2D loc_ = NO_VECTOR2D;
	ScanObjectVector objects_;
};

class ScannerImpl {
public:
	virtual void scan(BattleField& field) = 0;
};

class BspScanner : public ScannerImpl {
private:
	void teamScan(Population& active, Population& passive, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl);
public:
	Bsp bspA_;
	Bsp bspB_;
	Bsp bspPA_;
	Bsp bspPB_;

	void findInRange(Bsp& bsp, Object& from, ScanObjectType type, ScanObjectVector& result, size_t range);
	std::pair<Object*,Coord> findNearest(Bsp& bsp, Object& from);
	void buildBsps(BattleField& field);
	void scan(BattleField& field);
};

class SwarmScanner : public BspScanner {
private:
	void pickRandomN(ScanObjectType type, Tank& t, Population& team, ScanObjectVector& result, size_t n);
	void teamScan(Population& active, Population& passive, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl);
public:
	void scan(BattleField& field);
};

class ClusterScanner : public ScannerImpl {
private:
	void teamScan(Population& active, Population& passive, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, BattleFieldLayout& bfl);
public:
	pair<Vector2D, Coord> findNearestCenter(const vector<Vector2D>& centers, const Vector2D& loc);
	void scanClusterCenters(Population& team, vector<Vector2D>& result, size_t numCenters);

	vector<Vector2D> centersA_;
	vector<Vector2D> centersB_;
	void scan(BattleField& field);
};

class HybridScanner : public ClusterScanner, public BspScanner {
private:
	void teamScan(Population& active, Population& passive, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, Bsp& bspFriends, Bsp& bspEnemies, BattleFieldLayout& bfl);
public:
	void scan(BattleField& field);
};

typedef HybridScanner Scanner;

} /* namespace tankwar */

#endif /* SCANNER1_HPP_ */
