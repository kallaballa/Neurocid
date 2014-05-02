#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>
#include <assert.h>
#include <vector>
#include "scan.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
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

	size_t numEyes_;
	size_t numVectorsPerEye_;
	size_t numEyeLayers_;
	size_t numEyeNeuronsPerHidden_;

	size_t numOutputs;
	size_t numLayers_;
	size_t numNeuronsPerHidden_;
	size_t numBrains_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & numMetaInputs_;
		ar & numMetaLayers_;
		ar & numMetaNeuronsPerHidden_;

		ar & numEyes_;
		ar & numVectorsPerEye_;
		ar & numEyeLayers_;
		ar & numEyeNeuronsPerHidden_;

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
	Tweight lthrust_ = 0;
	Tweight rthrust_ = 0;
	Tweight fthrust_ = 0;
	Tweight bthrust_ = 0;
	Tweight shoot_ = 0;
	Tweight* metaInputs_ = NULL;
	Tweight* eyeInputs_ = NULL;

	BasicBrain() {
	}

	void initialize(BrainLayout layout, Tweight** weight = NULL) {
		layout_ = layout;
		metaInputs_ = NULL;
		eyeInputs_ = NULL;
		makeNN();
	    if(weight != NULL) {
	    	for(size_t b = 0; b < numNetworks(); ++b) {
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

	BasicBrain(const BasicBrain& other) : metaInputs_(other.metaInputs_), eyeInputs_(other.eyeInputs_) {
	};

	virtual ~BasicBrain() {
	};

	size_t numNetworks() {
		return layout_.numEyes_ + layout_.numBrains_ + 1;
	}
	virtual void makeNN() = 0;
	virtual void feedEye(const size_t eyeIndex, const size_t vIndex, Vector2D& v) = 0;
	virtual void destroy() = 0;
	virtual void randomize() = 0;
	virtual void reset() = 0;
	virtual size_t size(const size_t& bi) const = 0;
	virtual Tweight* weights(const size_t& bi) = 0;
#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & layout_;
	  if(!initialized_)
		  makeNN();

		for(size_t b = 0; b < numNetworks(); ++b)
			ar & boost::serialization::make_array(weights(b), size(b));
	}
#endif
};

} /* namespace neurocid */

#endif /* BRAIN_HPP_ */
