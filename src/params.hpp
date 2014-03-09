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

//Tank
static size_t MAX_AMMO;
static size_t MAX_DAMAGE;
static double MAX_TURN_RATE;
static double MAX_TANK_SPEED;
static double TANK_SCALE;
static size_t TANK_RANGE;
static size_t COOL_DOWN;

//Projectile
static double PROJECTILE_SCALE;
static double MAX_PROJECTILE_SPEED;
static int PROJECTILE_RANGE;
};
}
#endif /* PARAMS_HPP_ */
