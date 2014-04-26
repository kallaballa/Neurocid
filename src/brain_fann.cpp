#include "brain_fann.hpp"
#include "battlefield.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>

namespace neurocid {

#ifdef _CHECK_BRAIN_ALLOC
std::map<fann**, size_t> BrainFann::nnAllocs_;
size_t BrainFann::nnAllocCnt_ = 0;
#endif

BrainFann::BrainFann(const BrainFann& other): BasicBrain<fann_type>(other), nn_(other.nn_), lastBrain_(other.lastBrain_), brainSwitches_(other.brainSwitches_)  {
}

BrainFann::~BrainFann() {
}

void BrainFann::applyMeta(const size_t& i, const fann_type& value) {
	assert(i < layout_.numMetaInputs_);
	metaInputs_[i] = value;
	assert(!std::isnan(metaInputs_[i]) && !std::isinf(metaInputs_[i]) && metaInputs_[i] >= -1 && metaInputs_[i] <= 1);
}

void BrainFann::applyInput(const size_t& i, const fann_type& value) {
	assert(i < layout_.numInputs_);
	inputs_[i] = value;
	assert(!std::isnan(inputs_[i]) && !std::isinf(inputs_[i]) && inputs_[i] >= -1 && inputs_[i] <= 1);
}

void BrainFann::makeNN() {
	assert(layout_.numLayers_ >= 3);
	assert(layout_.numLayers_ < 20);
	assert(layout_.numBrains_ >= 1);
	unsigned int* layerArray = new unsigned int[std::max(layout_.numLayers_, layout_.numMetaLayers_)];

	// create meta brain
	layerArray[0] = layout_.numMetaInputs_;

	for(size_t i = 1; i < (layout_.numMetaLayers_ - 1); i++) {
		layerArray[i] = layout_.numMetaNeuronsPerHidden_;
	}

	layerArray[layout_.numMetaLayers_ - 1] = layout_.numBrains_;

	nn_ = new fann*[layout_.numBrains_ + 1];
	nn_[0] = fann_create_standard_array(layout_.numMetaLayers_, layerArray);
    fann_set_activation_function_hidden(nn_[0], FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_[0], FANN_SIGMOID_SYMMETRIC);

    // create sub brains
    layerArray[0] = layout_.numInputs_;
	for(size_t i = 1; i < (layout_.numLayers_ - 1); i++) {
		layerArray[i] = layout_.numNeuronsPerHidden_;
	}
    layerArray[layout_.numLayers_ - 1] = layout_.numOutputs;

	for(size_t i = 1; i < layout_.numBrains_ + 1; ++i) {
		nn_[i] = fann_create_standard_array(layout_.numLayers_, layerArray);
		fann_set_activation_function_hidden(nn_[i], FANN_SIGMOID_SYMMETRIC);
		fann_set_activation_function_output(nn_[i], FANN_SIGMOID_SYMMETRIC);
	}

    metaInputs_ = new fann_type[layout_.numMetaInputs_];
	inputs_ = new fann_type[layout_.numInputs_];
    delete[] layerArray;

	reset();

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

	for(size_t i = 0; i < layout_.numBrains_ + 1; ++i)
		fann_destroy(nn_[i]);

	delete[] nn_;
	nn_ = NULL;

	delete[] inputs_;
	inputs_ = NULL;
	destroyed_ = true;
}

void BrainFann::reset() {
	lastBrain_ = 0;
	brainSwitches_ = 0;
	assert(metaInputs_ != NULL);
	assert(inputs_ != NULL);

	std::fill_n(metaInputs_, layout_.numMetaInputs_, std::numeric_limits<fann_type>().max());
	std::fill_n(inputs_, layout_.numInputs_, std::numeric_limits<fann_type>().max());
}

void BrainFann::randomize() {
	for(size_t i = 0; i < layout_.numBrains_ + 1; ++i)
		fann_randomize_weights(nn_[i], -1, 1);
}

size_t BrainFann::size(const size_t& bi) const {
	return nn_[bi]->total_connections;
}

fann_type* BrainFann::weights(const size_t& bi) {
	return nn_[bi]->weights;
}

bool BrainFann::operator==(BrainFann& other) {
	assert(other.layout_.numBrains_ == this->layout_.numBrains_);

	for(size_t i = 0; i < layout_.numBrains_ + 1; ++i) {
		assert(other.size(i) == this->size(i));
		for(size_t j = 0; j < this->size(i); ++j){
			if(this->weights(i)[j] != other.weights(i)[j])
				return false;
		}
	}
	return true;
}

bool BrainFann::operator!=(BrainFann& other) {
	return !operator==(other);
}

void BrainFann::update(const BattleFieldLayout& bfl, const Scan& scan) {
	assert(nn_ != NULL);
	assert(metaInputs_ != NULL);
	assert(inputs_ != NULL);
	assert(!destroyed_);
	assert(layout_.numInputs_ == (scan.objects_.size() * 4) + 5);

	assert(layout_.numMetaInputs_ == fann_get_num_input(nn_[0]));
	assert(layout_.numBrains_ == fann_get_num_output(nn_[0]));

	for(size_t i = 1; i < layout_.numBrains_ + 1; ++i) {
		assert(layout_.numInputs_ == fann_get_num_input(nn_[i]));
		assert(layout_.numOutputs == fann_get_num_output(nn_[i]));
	}
}

void BrainFann::run() {
	fann_type* outputs = fann_run(nn_[0], metaInputs_);
	fann_type val = -1;
	size_t selected = 1;
	for(size_t i = 0; i < layout_.numBrains_; ++i) {
		if(outputs[i] > val) {
			val = outputs[i];
			selected = i + 1;
		}
	}

	if(selected != lastBrain_)
		++brainSwitches_;

	lastBrain_ = selected;
	outputs = fann_run(nn_[selected], inputs_);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	fthrust_ = outputs[2];
	bthrust_ = outputs[3];
	shoot_ = outputs[4];

	assert(!std::isnan(bthrust_) && !std::isnan(fthrust_) && !std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(shoot_));
	assert(!std::isinf(bthrust_) && !std::isinf(fthrust_) && !std::isinf(lthrust_) && !std::isinf(rthrust_) && !std::isinf(shoot_));
}
} /* namespace neurocid */
