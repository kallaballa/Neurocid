#include "brain.hpp"
#include "battlefield.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>

namespace tankwar {

BrainNearest::BrainNearest() : BasicBrain() {
}

BrainNearest::BrainNearest(BrainLayout layout, fann_type* weight) : BasicBrain(layout, weight) {
}

void BrainNearest::update(const BattleFieldLayout& bfl, const Scan& scan) {
	assert(nn_ != NULL);
	assert(inputs_ != NULL);
	assert(!destroyed_);

	size_t numInputs = layout_.numInputs_;
    assert(layout_.numInputs_ == (scan.objects_.size() * 2));
	assert(layout_.numInputs_ == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));

	for(size_t i = 0; i < numInputs; i++) {
		inputs_[i] = std::numeric_limits<fann_type>().max();
	}

	for (size_t i = 0; i < scan.objects_.size(); i++) {
		const ScanObject& so = scan.objects_[i];

		// if we didn't see an object we feed the last input
		if (so.loc_ != NO_VECTOR2D) {
			//if (so.type_ == ScanObjectType::ENEMY) {
				Vector2D dir = scan.dir_;
				Vector2D sub = so.loc_ - scan.loc_;
				Coord diff = (M_PI + radFromDir(dir))
						- (M_PI + radFromDir(sub));

				if (diff > M_PI)
					diff = M_PI - diff;

				else if (diff < -M_PI)
					diff = M_PI + diff;

				//		std::cerr << "diff:" << diff << std::endl;

				assert(diff > -M_PI && diff < M_PI);

				Vector2D scaledDiff = dirFromRad(diff);
				Coord scale = sub.length() / std::max(bfl.width_,bfl.height_);
				scaledDiff.x *= scale;
				scaledDiff.y *= scale;

				inputs_[i * 2] = scaledDiff.x;
				inputs_[i * 2 + 1] = scaledDiff.y;
/*			} else if (so.type_ == ScanObjectType::FRIEND) {
				Vector2D dir = scan.dir_;
				Vector2D sub = so.loc_ - scan.loc_;
				Coord diff = (M_PI + radFromDir(dir))
						- (M_PI + radFromDir(sub));

				if (diff > M_PI)
					diff = M_PI - diff;

				else if (diff < -M_PI)
					diff = M_PI + diff;

				if(diff < 0)
					diff = (M_PI/2) + diff;

				if(diff > 0)
					diff = (M_PI/2) - diff;

				//		std::cerr << "diff:" << diff << std::endl;

				assert(diff > -M_PI && diff < M_PI);
				Vector2D scaledDiff = dirFromRad(diff);
				Coord scale = sub.length() / std::max(bfl.width_,bfl.height_);
				scaledDiff.x *= scale;
				scaledDiff.y *= scale;

				inputs_[i * 2] = scaledDiff.x;
				inputs_[i * 2 + 1] = scaledDiff.y;
			}*/
		} else {
			std::cerr << "sight: " << i << " = max" << std::endl;
		}
	}

	for(size_t i = 0; i < numInputs; ++i) {
		assert(inputs_[i] != std::numeric_limits<fann_type>().max());
		assert(!std::isinf(inputs_[i]));
		assert(!std::isnan(inputs_[i]));
	}

	fann_type* outputs = fann_run(nn_, inputs_);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	shoot_ = outputs[2];
	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(shoot_));
	assert(!std::isinf(lthrust_) && !std::isinf(rthrust_) && !std::isinf(shoot_));

	//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}
} /* namespace tankwar */
