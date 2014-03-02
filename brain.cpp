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

namespace tankwar {

Brain::Brain(size_t inputs, size_t outputs, size_t layers, size_t hiddenNeurons): nn_(NULL) {
	nn_ = fann_create_standard(layers, inputs, hiddenNeurons, outputs);
    fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);
}

Brain::~Brain() {
	if(nn_ != NULL)
		fann_destroy(nn_);
}

void Brain::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

void Brain::update(const Tank& tank, const Population& ownTeam, const Population& otherTeam) {
	size_t len = 2 + (ownTeam.size() * 2) + (otherTeam.size() * 2);
	fann_type inputs[len];
	for(int i = 2; i < len; ++i) {
		inputs[i] = 0;
	}

	inputs[0] = (fann_type)tank.dir_.x;
	inputs[1] = (fann_type)tank.dir_.y;

	/*size_t off = 2;
	for(int i = 0; i < ownTeam.size(); ++i) {
		inputs[off + (i*2)] = (fann_type)(tank.loc_ - ownTeam[i].loc_).normalize().x;
	    inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - ownTeam[i].loc_).normalize().y;
	}

	off += ownTeam.size();

	for(int i = 0; i < otherTeam.size(); ++i) {
		inputs[off + (i*2)] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().x;
	    inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().y;
	}*/

	fann_type* outputs = fann_run(nn_, inputs);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	shoot_ = outputs[2];
}
} /* namespace tankwar */
