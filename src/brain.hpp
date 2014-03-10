#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>
#include <assert.h>
#include <vector>
#ifdef _CHECK_BRAIN_ALLOC
#include <map>
#endif

namespace tankwar {

struct BrainLayout  {
	size_t numInputs;
	size_t numOutputs;
	size_t numLayers_;
	size_t neuronsPerHidden;
};

class Tank;
class Population;

class Brain {
	bool destroyed_ = false;
#ifdef _CHECK_BRAIN_ALLOC
	static std::map<fann*, size_t> nnAllocs_;
	static size_t nnAllocCnt_;
#endif
public:
	BrainLayout  layout_;
	fann *nn_;
	fann_type lthrust_ = 0;
	fann_type rthrust_ = 0;
	fann_type shoot_ = 0;
	fann_type* inputs_;
	Brain(BrainLayout layout);
	Brain(const Brain& other);
	virtual ~Brain();
	void destroy();
	void update(const std::vector<Vector2D>& sight);
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

} /* namespace tankwar */

#endif /* BRAIN_HPP_ */
