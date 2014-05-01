#ifndef HISTORY_HPP_
#define HISTORY_HPP_

#include "2d.hpp"

namespace neurocid {

class Ship;

struct History {
	size_t count = 0;
	Vector2D totalLoc_;
	Vector2D totalVel_;

	Vector2D avgLoc_ = NO_VECTOR2D;
	Vector2D avgVel_  = NO_VECTOR2D;

	Vector2D average(const Vector2D& total);
	void update(Ship& ship);
	void calculate();
};

} /* namespace neurocid */

#endif /* HISTORY_HPP_ */
