#include "brain_fann.hpp"
#include "battlefield.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>

namespace neurocid {

#ifdef _CHECK_BRAIN_ALLOC
std::map<fann*, size_t> BrainFann::nnAllocs_;
size_t BrainFann::nnAllocCnt_ = 0;
#endif

BrainFann::BrainFann(const BrainFann& other): BasicBrain<fann_type>(other), nn_(other.nn_) {
}

BrainFann::~BrainFann() {
}

void BrainFann::applyInput(const size_t& i, const fann_type& value) {
	assert(i < layout_.numInputs_);
	inputs_[i] = value;
	assert(!std::isnan(inputs_[i]) && !std::isinf(inputs_[i]) && inputs_[i] >= -1 && inputs_[i] <= 1);
}
void BrainFann::makeNN() {
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

void BrainFann::destroy() {
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

void BrainFann::reset() {
	assert(inputs_ != NULL);
    std::fill_n(inputs_, layout_.numInputs_, std::numeric_limits<fann_type>().max());
}

void BrainFann::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

size_t BrainFann::size() const {
	return nn_->total_connections;
}

bool BrainFann::operator==(BrainFann& other) {
	assert(other.size() == this->size());
	for(size_t i = 0; i < this->size(); ++i){
		if(this->weights()[i] != other.weights()[i])
			return false;
	}
	return true;
}

bool BrainFann::operator!=(BrainFann& other) {
	return !operator==(other);
}

void BrainFann::update(const BattleFieldLayout& bfl, const Scan& scan) {
	assert(nn_ != NULL);
	assert(inputs_ != NULL);
	assert(!destroyed_);
	assert(layout_.numInputs_ == (scan.objects_.size() * 2) + 4);
	assert(layout_.numInputs_ == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));
}

fann_type* BrainFann::weights() {
	return nn_->weights;
}

void BrainFann::run() {
	fann_type* outputs = fann_run(nn_, inputs_);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	fthrust_ = outputs[2];
	bthrust_ = outputs[3];
	shoot_ = outputs[4];

	assert(!std::isnan(bthrust_) && !std::isnan(fthrust_) && !std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(shoot_));
	assert(!std::isinf(bthrust_) && !std::isinf(fthrust_) && !std::isinf(lthrust_) && !std::isinf(rthrust_) && !std::isinf(shoot_));
}
} /* namespace neurocid */
