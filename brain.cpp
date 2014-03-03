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
	nn_ = fann_create_standard(Params::NUM_LAYERS, Params::NUM_INPUTS, Params::NUM_NEURONS_PER_HIDDEN, Params::NUM_OUTPUTS);
    fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);
}

Brain::Brain(const Brain& other): nn_(other.nn_) {
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
	assert(Params::NUM_INPUTS == fann_get_num_input(nn_));
	assert(Params::NUM_OUTPUTS == fann_get_num_output(nn_));
	fann_type inputs[numInputs];

	inputs[0] = (fann_type)tank.dir_.x;
	inputs[1] = (fann_type)tank.dir_.y;
//	inputs[2] = (fann_type)	(1.0 / Params::MAX_PROJECTILES) * (double)tank.projectiles_;
	inputs[2] = (fann_type)(tank.loc_ - otherTeam[0].loc_).normalize().x;
	inputs[3] = (fann_type)(tank.loc_ - otherTeam[0].loc_).normalize().y;

	assert(!isnan(inputs[0]) && !isnan(inputs[1]) && !isnan(inputs[2]));

/*	size_t off = 3;
	for(size_t i = 0; i < ownTeam.size(); ++i) {
		inputs[off + (i*2)] = (fann_type)(tank.loc_ - ownTeam[i].loc_).normalize().x;
		inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - ownTeam[i].loc_).normalize().y;
//		inputs[off + (i*2)] = 1;
//		inputs[off + (i*2) + 1] = 1;
	}

	off += (ownTeam.size() * 2);

	for(size_t i = 0; i < otherTeam.size(); ++i) {
		inputs[off + (i*2)] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().x;
	    inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().y;
//		inputs[off + (i*2)] = 1;
//		inputs[off + (i*2) + 1] = 1;
	}

	off += (otherTeam.size() * 2);
*/
//	std::cerr << "input:";
	for(size_t i = 0; i < numInputs; ++i) {
		assert(!isnan(inputs[i]));
//		std::cerr << "\t" << inputs[i];
	}
//	std::cerr << std::endl;
	fann_type* outputs = fann_run(nn_, inputs);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	shoot_ = outputs[2];
//	std::cerr << "shoot:\t" << inputs[2] << std::endl;
//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}

size_t Brain::size() const {
	return nn_->total_connections;
}

fann_type* Brain::weights() {
	return nn_->weights;
}
} /* namespace tankwar */
