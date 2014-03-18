#include "brain.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>

namespace tankwar {

#ifdef _CHECK_BRAIN_ALLOC
std::map<fann*, size_t> Brain::nnAllocs_;
size_t Brain::nnAllocCnt_ = 0;
#endif

Brain::Brain(BrainLayout layout): layout_(layout), nn_(NULL), inputs_(NULL) {
	assert(layout_.numLayers_ >= 3);
	assert(layout_.numLayers_ < 20);
	unsigned int layerArray[layout_.numLayers_];
	layerArray[0] = layout_.numInputs;
	layerArray[layout_.numLayers_ - 1] = layout_.numOutputs;

	for(size_t i = 1; i < (layout_.numLayers_ - 1); i++) {
		layerArray[i] = layout_.neuronsPerHidden;
	}

	nn_ = fann_create_standard_array(layout_.numLayers_, layerArray);
    fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);

    inputs_ = new fann_type[layout_.numInputs];
    std::fill_n(inputs_, layout_.numInputs, std::numeric_limits<fann_type>().max());

#ifdef _CHECK_BRAIN_ALLOC
    size_t id = ++nnAllocCnt_;
    nnAllocs_[nn_] = id;
    std::cerr << "alloc: " << id << std::endl;
#endif
}

Brain::Brain(const Brain& other): layout_(other.layout_), nn_(other.nn_) , inputs_(other.inputs_) {
}

Brain::~Brain() {
}

void Brain::destroy() {
    assert(nn_ != NULL);
#ifdef _CHECK_BRAIN_ALLOC
	auto it = nnAllocs_.find(nn_);
	assert(it != nnAllocs_.end());
	std::cerr << "free: " << (*it).second << std::endl;
	nnAllocs_.erase(it);
#endif
	fann_destroy(nn_);
	nn_ = NULL;
	delete[] inputs_;
	inputs_ = NULL;

	destroyed_ = true;
}

void Brain::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

void Brain::update(const Scan& scan) {
	assert(nn_ != NULL);
	assert(inputs_ != NULL);
	assert(!destroyed_);

	size_t numInputs = layout_.numInputs;
    assert(layout_.numInputs == (scan.objects_.size() * 2));
	assert(layout_.numInputs == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));

	for(size_t i = 0; i < numInputs; i++) {
		inputs_[i] = std::numeric_limits<fann_type>().max();
	}

	for (size_t i = 0; i < scan.objects_.size(); i++) {
		const ScanObject& so = scan.objects_[i];

		// if we didn't see an object we feed the last input
		if (so.loc_ != NO_VECTOR2D) {
			if (so.type_ == ScanObjectType::ENEMY) {
				Vector2D dir = scan.dir_;

				double diff = (M_PI + radFromDir(dir))
						- (M_PI + radFromDir(so.loc_ - scan.loc_));

				if (diff > M_PI)
					diff = M_PI - diff;

				else if (diff < -M_PI)
					diff = M_PI + diff;

				//		std::cerr << "diff:" << diff << std::endl;

				assert(diff > -M_PI && diff < M_PI);

				Vector2D vDiff = dirFromRad(diff);

				inputs_[i * 2] = dirFromRad(diff).x;
				inputs_[i * 2 + 1] = dirFromRad(diff).y;
			} else if (so.type_ == ScanObjectType::FRIEND) {
				Vector2D dir = scan.dir_;

				double diff = (M_PI + radFromDir(dir))
						- (M_PI + radFromDir(so.loc_ - scan.loc_));

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
				inputs_[i * 2] = dirFromRad(diff).x;
				inputs_[i * 2 + 1] = dirFromRad(diff).y;
			}
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

size_t Brain::size() const {
	return nn_->total_connections;
}

fann_type* Brain::weights() {
	return nn_->weights;
}
} /* namespace tankwar */
