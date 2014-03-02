/*
 * brain.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>

namespace tankwar {

class Tank;
class Population;

class Brain {
	fann *nn_;
public:
	fann_type lthrust_ = 0;
	fann_type rthrust_ = 0;
	fann_type shoot_ = 0;

	Brain(size_t intputs, size_t outputs, size_t layers, size_t hiddenNeurons);
	virtual ~Brain();
	void update(const Tank& tank, const Population& ownTeam, const Population& otherTeam);
	void randomize();
};


typedef std::shared_ptr<Brain> Brain_ptr;
} /* namespace tankwar */

#endif /* BRAIN_HPP_ */
