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
	static std::map<fann*, size_t> nnAllocs_;
	static size_t nnAllocCnt_;
#endif
	fann *nn_;
public:
	typedef fann_type value_type;
	BrainFann() : BasicBrain<fann_type>(), nn_(NULL) {
	}

	BrainFann(const BrainFann& other);
	virtual ~BrainFann();

	virtual void makeNN();
	virtual void applyInput(const size_t& i, const fann_type& value);
	virtual void destroy();
	virtual void randomize();
	virtual void reset();
	virtual size_t size() const;
	virtual fann_type* weights();

	virtual bool operator==(BrainFann& other);
	virtual bool operator!=(BrainFann& other);
	virtual void update(const BattleFieldLayout& bfl, const Scan& scan);
	void run();
};

} /* namespace neurocid */

#endif /* BRAIN_FANN */