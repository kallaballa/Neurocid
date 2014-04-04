#include "brain.hpp"
#include "battlefield.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>

namespace neurocid {

#ifdef _CHECK_BRAIN_ALLOC
std::map<fann*, size_t> BasicBrain::nnAllocs_;
size_t BasicBrain::nnAllocCnt_ = 0;
#endif

BasicBrain::BasicBrain(BrainLayout layout, fann_type* weight): layout_(layout), nn_(NULL), inputs_(NULL) {
	makeNN();
    if(weight != NULL) {
    	for(size_t i = 0; i < size(); ++i) {
    		weights()[i] = weight[i];
    	}
    }
}

BasicBrain::BasicBrain(const BasicBrain& other): layout_(other.layout_), nn_(other.nn_) , inputs_(other.inputs_) {
}

BasicBrain::~BasicBrain() {
}

void BasicBrain::makeNN() {
	assert(layout_.numLayers_ >= 3);
	assert(layout_.numLayers_ < 20);
	unsigned int* layerArray = new unsigned int[layout_.numLayers_];
	layerArray[0] = layout_.numInputs_;

	for(size_t i = 1; i < (layout_.numLayers_ - 1); i++) {
		layerArray[i] = layout_.neuronsPerHidden;
	}

	layerArray[layout_.numLayers_ - 1] = layout_.numOutputs;

	nn_ = fann_create_standard_array(layout_.numLayers_, layerArray);
    fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);

    inputs_ = new fann_type[layout_.numInputs_];
    reset();
    delete[] layerArray;

#ifdef _CHECK_BRAIN_ALLOC
    size_t id = ++nnAllocCnt_;
    nnAllocs_[nn_] = id;
    std::cerr << "alloc: " << id << std::endl;
#endif
}

void BasicBrain::destroy() {
	assert(!destroyed_);
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

void BasicBrain::reset() {
	assert(inputs_ != NULL);
    std::fill_n(inputs_, layout_.numInputs_, std::numeric_limits<fann_type>().max());
}

void BasicBrain::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

size_t BasicBrain::size() const {
	return nn_->total_connections;
}

fann_type* BasicBrain::weights() {
	return nn_->weights;
}

bool BasicBrain::isDestroyed() {
	return destroyed_;
}

bool BasicBrain::operator==(BasicBrain& other) {
	assert(other.size() == this->size());
	for(size_t i = 0; i < this->size(); ++i){
		if(this->weights()[i] != other.weights()[i])
			return false;
	}
	return true;
}

bool BasicBrain::operator!=(BasicBrain& other) {
	return !operator==(other);
}

} /* namespace neurocid */
