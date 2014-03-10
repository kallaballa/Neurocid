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

		size_t countDead() {
			size_t cnt = 0;
			for(Tank& t : *this) {
				if(t.dead_)
					cnt++;
			}
			return cnt;
		}
};

}

#endif /* POPULATION_HPP_ */
