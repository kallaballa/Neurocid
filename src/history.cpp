#include "history.hpp"
#include "ship.hpp"

namespace neurocid {

Vector2D History::average(const Vector2D& total) {
	return Vector2D(total.x_ / count, total.y_ / count);
}

void History::update(Ship& ship) {
	totalLoc_ += ship.loc_;
	totalVel_ += ship.vel_;
	++count;
}

void History::calculate() {
	avgLoc_ = average(totalLoc_);
	avgVel_ = average(totalVel_);
}

} /* namespace neurocid */
