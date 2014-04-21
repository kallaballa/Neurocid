/*
 * background.hpp
 *
 *  Created on: Apr 9, 2014
 *      Author: elchaschab
 */

#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_

#include <SDL/SDL.h>
#include "2d.hpp"
#include <vector>

namespace neurocid {

using std::vector;

class Canvas;
class BattleFieldLayout;

struct Star {
	Coord x;
	Coord y;
	Coord radius;
	Uint8 r;
	Uint8 g;
	Uint8 b;
	double alpha;
	double step;
	size_t discontinuity;
	double scale;
};

class Background {
private:
	Canvas* canvas_;
	vector<Star> buffers_;
public:
	Background(Canvas* canvas);
	void init(BattleFieldLayout& bfl);
	void draw(SDL_Surface* surface);
};

} /* namespace tankwar */

#endif /* BACKGROUND_HPP_ */
