#include "brain.hpp"
#include "population.hpp"
#include "tank.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>

namespace tankwar {

#ifdef _CHECK_BRAIN_ALLOC
std::map<fann*, size_t> Brain::nnAllocs_;
size_t Brain::nnAllocCnt_ = 0;
#endif

Brain::Brain(BrainLayout layout): layout_(layout), nn_(NULL) {
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
#ifdef _CHECK_BRAIN_ALLOC
    size_t id = ++nnAllocCnt_;
    nnAllocs_[nn_] = id;
    std::cerr << "alloc: " << id << std::endl;
#endif
}

Brain::Brain(const Brain& other): layout_(other.layout_), nn_(other.nn_) {
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
	destroyed_ = true;
}

void Brain::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

void Brain::update(const vector<Vector2D>& sight) {
	assert(nn_ != NULL);
	size_t numInputs = layout_.numInputs;
	assert(layout_.numInputs == (sight.size() * 2));
	assert(layout_.numInputs == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));

	fann_type inputs[numInputs];

	for(size_t i = 0; i < sight.size(); i++) {
		inputs[(i*2)] = sight[i].x;
		inputs[(i*2)+1] = sight[i].y;
	}

	for(size_t i = 0; i < numInputs; ++i) {
		assert(!std::isinf(inputs[i]));
		assert(!std::isnan(inputs[i]));
	}

	fann_type* outputs = fann_run(nn_, inputs);
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
