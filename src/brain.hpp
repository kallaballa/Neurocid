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
	size_t numInputs_;
	size_t numOutputs;
	size_t numLayers_;
	size_t neuronsPerHidden;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & numInputs_;
	  ar & numOutputs;
	  ar & numLayers_;
	  ar & neuronsPerHidden;
	}
#endif
};

class Tank;
class Population;

class BasicBrain {
#ifdef _CHECK_BRAIN_ALLOC
	static std::map<fann*, size_t> nnAllocs_;
	static size_t nnAllocCnt_;
#endif
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
public:
	bool destroyed_ = false;
	BrainLayout  layout_;
	fann *nn_;
	fann_type lthrust_ = 0;
	fann_type rthrust_ = 0;
	fann_type fthrust_ = 0;
	fann_type bthrust_ = 0;
	fann_type shoot_ = 0;
	fann_type* inputs_;

	BasicBrain() :  layout_(), nn_(NULL), inputs_(NULL) {}
	BasicBrain(BrainLayout layout, fann_type* weights = NULL);
	BasicBrain(const BasicBrain& other);
	virtual ~BasicBrain();

	void makeNN();
	void destroy();
	void reset();
	void randomize();
	size_t size() const;
	fann_type* weights();
	bool isDestroyed();
	bool operator==(BasicBrain& other);
	bool operator!=(BasicBrain& other);

	virtual void update(const BattleFieldLayout& bfl, const Scan& scan) = 0;

#ifndef _NO_SERIALIZE
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
#endif
};

class BrainNearest : public BasicBrain {
public:
	BrainNearest();
	BrainNearest(BrainLayout layout, fann_type* weights = NULL);
	virtual void update(const BattleFieldLayout& bfl, const Scan& scan);
};

class BrainSwarm : public BasicBrain {
private:
	void applyInput(const size_t& i, const fann_type& value);
public:
	BrainSwarm();
	BrainSwarm(BrainLayout layout, fann_type* weights = NULL);
	virtual void update(const BattleFieldLayout& bfl, const Scan& scan);
};

typedef BrainSwarm Brain;
} /* namespace neurocid */

#endif /* BRAIN_HPP_ */
