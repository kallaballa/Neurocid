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
struct PopulationLayout {
	TankLayout tl_ = {
			true, // canShoot
			true, // canRotate
			true, // canMove

			10.0, // range_
			1, // max_speed_
			1, // max_rotation

			200,  // max_cooldown
			10,  // max_ammo_
			3    // max_damage_
	};

	BrainLayout bl_ = {
			4, // inputs
			3, // outputs
			5, // layers
			6  // neurons per hidden layer
	};
};

class Population : public std::vector<Tank> {
	public:
		PopulationLayout layout_;
		size_t score_ = 0;
		bool winner_ = false;

		void update(TankLayout tl) {
			for(Tank& t : *this) {
				t.update(tl);
			}
		}

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
