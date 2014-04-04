/*
 * battlefieldlayout.hpp
 *
 *  Created on: Mar 30, 2014
 *      Author: elchaschab
 */

#ifndef BATTLEFIELDLAYOUT_HPP_
#define BATTLEFIELDLAYOUT_HPP_

#include "2d.hpp"

namespace neurocid {

struct BattleFieldLayout {
	size_t iterations_;
	Coord width_;
	Coord height_;
};

}

#endif /* BATTLEFIELDLAYOUT_HPP_ */
