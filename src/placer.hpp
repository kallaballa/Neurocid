/*
 * placer.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef PLACER_HPP_
#define PLACER_HPP_

#include <vector>
#include "util.hpp"
#include "2d.hpp"
#include "population.hpp"

namespace tankwar {
using std::vector;

class Placer {
public:
	virtual void place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing) = 0;
};

class OppositeLines  : public Placer {
public:
	virtual void place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing);
};

class OppositeLinesFacingInward : public Placer {
public:
	virtual void place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing);
};

class RandomOppositeLines  : public Placer {
public:
	Vector2D axisDir_;
	Vector2D centerA_;
	Vector2D centerB_;
	virtual void place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing);
};

class RandomOppositeLinesFacingRandom  : public RandomOppositeLines {
public:
	virtual void place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing);
};

class RandomOppositeLinesFacingInward  : public RandomOppositeLines {
public:
	virtual void place(vector<Population>& teams, Vector2D center, Coord distance, Coord spacing);
};

} /* namespace tankwar */

#endif /* PLACER_HPP_ */
