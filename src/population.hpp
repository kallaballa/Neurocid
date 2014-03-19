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
	TankLayout tl_ = { true, // canShoot
			true, // canRotate
			true, // canMove

			10.0, // range_
			1, // max_speed_
			1, // max_rotation

			200,  // max_cooldown
			10,  // max_ammo_
			3    // max_damage_
			};

	BrainLayout bl_ = { 4, // inputs
			3, // outputs
			5, // layers
			6  // neurons per hidden layer
			};
};

class Population: public std::vector<Tank> {
public:
	struct Statistics {
		Statistics() {
			generationCnt_ = 0;
			reset();
		}

		//total fitness of population
		double totalFitness_;
		//best fitness this population
		double bestFitness_;
		//average fitness
		double averageFitness_;
		//worst
		double worstFitness_;
		//keeps track of the best genome
		//generation counter
		double totalFriendlyFire_;
		double totalHits_;
		double totalDamage_;
		double totalAmmonition_;
		double averageFriendlyFire_;
		double averageHits_;
		double averageDamage_;
		double averageAmmonition_;

		size_t generationCnt_;
		double score_;

		size_t fittestGenome_;

		void reset() {
			totalFitness_ = 0;
			bestFitness_ = 0;
			worstFitness_ = std::numeric_limits<double>().max();
			averageFitness_ = 0;
			averageFriendlyFire_ = 0;
			averageDamage_ = 0;
			averageAmmonition_ = 0;
			totalFitness_ = 0;
			totalFriendlyFire_ = 0;
			totalHits_ = 0;
			totalDamage_ = 0;
			totalAmmonition_ = 0;
			score_ = 0;
		}

		void print(std::ostream& os) {
			os << generationCnt_ << ":" << bestFitness_ << ":"
					<< averageFitness_ << ":" << averageHits_ << ":"
					<< averageFriendlyFire_ / 3 << ":" << averageHits_ << ":"
					<< averageDamage_ << ":" << (averageAmmonition_ / 3) << ":"
					<< (score_);
		}
	};
	PopulationLayout layout_;
	size_t score_ = 0;
	bool winner_ = false;
	Statistics stats_;

	void update(TankLayout tl) {
		for (Tank& t : *this) {
			t.update(tl);
		}
	}

	size_t countDead() {
		size_t cnt = 0;
		for (Tank& t : *this) {
			if (t.dead_)
				cnt++;
		}
		return cnt;
	}
};
}

#endif /* POPULATION_HPP_ */
