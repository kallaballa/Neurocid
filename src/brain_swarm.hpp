#ifndef BRAIN_SWARM_HPP_
#define BRAIN_SWARM_HPP_

#include "brain_fann.hpp"
#include <cstddef>

namespace neurocid {

typedef BrainFann parentBrain_t;

class BrainSwarm : public parentBrain_t {
public:
	BrainSwarm() :
		parentBrain_t() {
	}

	virtual ~BrainSwarm() {
	}

	BrainSwarm(const BrainSwarm& other): parentBrain_t(other) {
	}

	virtual void update(const BattleFieldLayout& bfl, const Scan& scan);
};

} /* namespace neurocid */

#endif /* BRAIN_SWARM */
