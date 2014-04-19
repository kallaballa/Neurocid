#ifndef BRAIN_FANN_HPP_
#define BRAIN_FANN_HPP_

#include <fann.h>
#include "2d.hpp"
#include <memory>
#include <assert.h>
#include <vector>
#include "scan.hpp"
#include "brain.hpp"

namespace neurocid {

class Ship;
class Population;

class BrainFann : public BasicBrain<fann_type> {
#ifdef _CHECK_BRAIN_ALLOC
	static std::map<fann**, size_t> nnAllocs_;
	static size_t nnAllocCnt_;
#endif
	fann **nn_;
	size_t lastBrain_;
	size_t brainSwitches_;
public:
	typedef fann_type value_type;
	BrainFann() : BasicBrain<fann_type>(), nn_(NULL), lastBrain_(0), brainSwitches_(0) {
	}

	BrainFann(const BrainFann& other);
	virtual ~BrainFann();

	virtual void makeNN();
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
	size_t brainSwitches() {
		return brainSwitches_;
	}
};

} /* namespace neurocid */

#endif /* BRAIN_FANN */
