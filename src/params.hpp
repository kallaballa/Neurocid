/*
 * params.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef PARAMS_HPP_
#define PARAMS_HPP_

#include <cstdlib>

namespace tankwar {
struct Params {

//Neural network
static double ACTIVATION_RESPONSE;
static double BIAS;

//Projectile
static double PROJECTILE_SCALE;
static double MAX_PROJECTILE_SPEED;
static int PROJECTILE_RANGE;
};
}
#endif /* PARAMS_HPP_ */
