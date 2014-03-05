#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>
#include <assert.h>

namespace tankwar {

class Tank;
class Population;

class Brain {
	bool destroyed_ = false;

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

	bool isDestroyed() {
		return destroyed_;
	}

	bool operator==(Brain& other) {
		assert(other.size() == this->size());
		for(size_t i = 0; i < this->size(); ++i){
			if(this->weights()[i] != other.weights()[i])
				return false;
		}
		return true;
	}

	bool operator!=(Brain& other) {
		return !operator==(other);
	}
};


typedef Brain* Brain_ptr;
} /* namespace tankwar */

#endif /* BRAIN_HPP_ */
