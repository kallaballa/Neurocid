#ifndef BRAIN_FANN_HPP_
#define BRAIN_FANN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>
#include <assert.h>
#include <vector>
#include <deque>
#include "scan.hpp"
#include "brain.hpp"
#include "battlefieldlayout.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/deque.hpp>
#endif

namespace neurocid {

class Ship;
class Population;

using std::deque;

struct BrainStatistics {
	deque<size_t> switches_;
	size_t numGameSwitches_ = 0;

	void recordBrainSwitch(size_t index) {
		numGameSwitches_ = 0;
		switches_.push_back(index);
		if(switches_.size() > 10000)
			switches_.pop_front();
	}
#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & switches_;
	}
#endif
};

class BrainFann : public BasicBrain<fann_type> {
#ifdef _CHECK_BRAIN_ALLOC
	static std::map<fann**, size_t> nnAllocs_;
	static size_t nnAllocCnt_;
#endif
	fann **nn_;
	size_t lastBrain_;
public:
	BrainStatistics brainStats_;

	typedef fann_type value_type;
	BrainFann() : BasicBrain<fann_type>(), nn_(NULL), lastBrain_(0), brainStats_() {
	}

	BrainFann(const BrainFann& other);
	virtual ~BrainFann();

	virtual void makeNN();
	virtual void applyMeta(const size_t& i, const fann_type& value);
	virtual void applyInput(const size_t& i, const fann_type& value);
	virtual void destroy();
	virtual void randomize();
	virtual void reset();
	virtual size_t size(const size_t& bi) const override;
	virtual fann_type* weights(const size_t& bi) override;

	virtual bool operator==(BrainFann& other);
	virtual bool operator!=(BrainFann& other);
	virtual void update(const BattleFieldLayout& bfl, const Scan& scan);
	void run();
};

} /* namespace neurocid */

#endif /* BRAIN_FANN */
