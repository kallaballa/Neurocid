/*
 * population.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef POPULATION_HPP_
#define POPULATION_HPP_

#include <vector>
#include "tank.hpp"

namespace tankwar {

class Population : public std::vector<Tank> {
	public:
		size_t score_ = 0;
		bool winner_ = false;
};

}

#endif /* POPULATION_HPP_ */
