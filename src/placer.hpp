/*
 * placer.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef PLACER_HPP_
#define PLACER_HPP_

#include "2d.hpp"
#include <string>

namespace neurocid {
using std::string;

struct PlacerLayout {
	Vector2D center_;
	Coord distance_;
	Coord spacing_;
	Coord rotation_;
	string placer_;
};

} /* namespace neurocid */

#endif /* PLACER_HPP_ */
