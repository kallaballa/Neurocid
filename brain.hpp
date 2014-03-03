#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>

namespace tankwar {

class Tank;
class Population;

class Brain {

public:
	fann *nn_;
	fann_type lthrust_ = 0;
	fann_type rthrust_ = 0;
	fann_type shoot_ = 0;

	Brain();
	Brain(const Brain& other);
	virtual ~Brain();
	void destroy();
	void update(const Tank& tank, const Population& ownTeam, const Population& otherTeam);
	void randomize();
	size_t size() const;
	fann_type* weights();
};


typedef Brain* Brain_ptr;
} /* namespace tankwar */

#endif /* BRAIN_HPP_ */
