#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>
#include <vector>
#include "scan.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

#ifdef _CHECK_BRAIN_ALLOC
#include <map>
#endif

namespace neurocid {

struct BrainLayout  {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	size_t numMetaInputs_;
	size_t numMetaLayers_;
	size_t numMetaNeuronsPerHidden_;
	size_t numInputs_;
	size_t numOutputs;
	size_t numLayers_;
	size_t numNeuronsPerHidden_;
	size_t numBrains_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    //suppress warning about unused variable version
    assert(version >= 0);
		ar & numMetaInputs_;
		ar & numMetaLayers_;
		ar & numMetaNeuronsPerHidden_;
		ar & numInputs_;
		ar & numOutputs;
		ar & numLayers_;
		ar & numNeuronsPerHidden_;
		ar & numBrains_;
	}
#endif
};

class Ship;
class Population;

template<typename Tweight> class BasicBrain {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
public:
	bool destroyed_ = false;
	bool initialized_ = false;
	BrainLayout  layout_;
	Tweight rthrust_ = 0;
	Tweight* metaInputs_ = NULL;
	Tweight* inputs_ = NULL;
  Tweight* outputs_ = NULL;

  BasicBrain() {
	}

	void initialize(BrainLayout layout, Tweight** weight = NULL) {
	  outputs_ = new Tweight[layout.numOutputs];
	  layout_ = layout;
		metaInputs_ = NULL;
		inputs_ = NULL;
		makeNN();
	    if(weight != NULL) {
	    	for(size_t b = 0; b < layout_.numBrains_ + 1; ++b) {
				for(size_t i = 0; i < size(b); ++i) {
					weights(b)[i] = weight[b][i];
				}
	    	}
	    }
	    initialized_ = true;
	}

	bool isDestroyed() {
		return destroyed_;
	}

	BasicBrain(const BasicBrain& other) : metaInputs_(other.metaInputs_), inputs_(other.inputs_) {
	};

	virtual ~BasicBrain() {
	};

	virtual void makeNN() = 0;
	virtual void applyInput(const size_t& i, const fann_type& value) = 0;
	virtual void destroy() = 0;
	virtual void randomize() = 0;
	virtual void reset() = 0;
	virtual size_t size(const size_t& bi) const = 0;
	virtual Tweight* weights(const size_t& bi) = 0;
#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    //suppress warning about unused variable version
    assert(version >= 0);
		ar & layout_;
	  if(!initialized_)
		  makeNN();

		for(size_t b = 0; b < layout_.numBrains_ + 1; ++b)
			ar & boost::serialization::make_array(weights(b), size(b));
	}
#endif
};

} /* namespace neurocid */

#endif /* BRAIN_HPP_ */
