/*
 * brain.cpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#include "brain.hpp"
#include "population.hpp"
#include "tank.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>

namespace tankwar {

Brain::Brain(): nn_(NULL) {
	size_ = Params::NUM_INPUTS + Params::NUM_OUTPUTS + (((Params::NUM_LAYERS - 2) * Params::NUM_NEURONS_PER_HIDDEN));
	nn_ = fann_create_standard(Params::NUM_LAYERS, Params::NUM_INPUTS, Params::NUM_NEURONS_PER_HIDDEN, Params::NUM_NEURONS_PER_HIDDEN, Params::NUM_NEURONS_PER_HIDDEN, Params::NUM_NEURONS_PER_HIDDEN, Params::NUM_OUTPUTS);
    fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);
}

Brain::~Brain() {
}

void Brain::destroy() {
	if(nn_ != NULL)
		fann_destroy(nn_);
	nn_ = NULL;
}

void Brain::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

void Brain::update(const Tank& tank, const Population& ownTeam, const Population& otherTeam) {
	assert(nn_ != NULL);
	size_t numInputs = Params::NUM_INPUTS;
	fann_type inputs[numInputs];

	inputs[0] = (fann_type)tank.dir_.x;
	inputs[1] = (fann_type)tank.dir_.y;
	inputs[2] = (fann_type)	(1.0 / Params::MAX_PROJECTILES) * (double)tank.projectiles_;

	size_t off = 3;
	for(size_t i = 0; i < ownTeam.size(); ++i) {
	//	inputs[off + (i*2)] = (fann_type)(tank.loc_ - ownTeam[i].loc_).x;
	  //  inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - ownTeam[i].loc_).y;
		inputs[off + (i*2)] = 0;
		inputs[off + (i*2) + 1] = 0;
	}

	off += ownTeam.size();

	for(size_t i = 0; i < otherTeam.size(); ++i) {
//		inputs[off + (i*2)] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().x;
//	    inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().y;
		inputs[off + (i*2)] = 0;
		inputs[off + (i*2) + 1] = 0;
	}
	/*std::cerr << "input:\t";
	for(size_t i = 0; i < numInputs; ++i) {
		std::cerr << inputs[i] << "\t";
	}
	std::cerr << std::endl;*/
	fann_type* outputs = fann_run(nn_, inputs);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	shoot_ = outputs[2];
//	std::cerr << "shoot:\t" << inputs[2] << std::endl;
//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}

size_t Brain::size() const {
	return size_;
}

fann_type* Brain::weights() {
	return nn_->weights;
}
} /* namespace tankwar */
