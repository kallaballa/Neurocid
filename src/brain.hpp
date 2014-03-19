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
#include "scanner.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace tankwar {

struct BrainLayout  {
	friend class boost::serialization::access;

	size_t numInputs;
	size_t numOutputs;
	size_t numLayers_;
	size_t neuronsPerHidden;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & numInputs;
	  ar & numOutputs;
	  ar & numLayers_;
	  ar & neuronsPerHidden;
	}
};

class Tank;
class Population;

class Brain {
	bool destroyed_ = false;
#ifdef _CHECK_BRAIN_ALLOC
	static std::map<fann*, size_t> nnAllocs_;
	static size_t nnAllocCnt_;
#endif
	friend class boost::serialization::access;
public:
	BrainLayout  layout_;
	fann *nn_;
	fann_type lthrust_ = 0;
	fann_type rthrust_ = 0;
	fann_type shoot_ = 0;
	fann_type* inputs_;
	Brain() :  layout_(), nn_(NULL), inputs_(NULL) {}
	Brain(BrainLayout layout, fann_type* weights = NULL);
	Brain(const Brain& other);
	virtual ~Brain();
	fann* makeNN();
	void destroy();
	void update(const Scan& scan);
	void randomize();
	size_t size() const;
	fann_type* weights();

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & layout_;
	  if(nn_ == NULL) {
		  makeNN();
	  }
	  size_t s = size();
	  ar & s;
	  ar & boost::serialization::make_array(weights(), s);
	}

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
