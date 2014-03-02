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
static size_t FRAME_PER_SECOND;

//Neural network
static size_t NUM_INPUTS;
static size_t NUM_HIDDEN;
static size_t NUM_NEURONS_PER_HIDDEN;
static size_t NUM_OUTPUTS;
static double ACTIVATION_RESPONSE;
static double BIAS;

//Tank
static size_t MAX_PROJECTILES;
static size_t MAX_DAMAGE;
static double MAX_TURN_RATE;
static double MAX_TANK_SPEED;
static double TANK_SCALE;
static size_t TANK_RANGE;

//Population
static size_t NUM_TANKS;
static size_t NUM_TICKS;

//Projectile
static double PROJECTILE_SCALE;
static double MAX_PROJECTILE_SPEED;
static int PROJECTILE_RANGE;
//Genetic
static double CROSSOVER_RATE;
static double MUTATION_RATE;
static double MAX_PERTUBATION;
static size_t NUM_ELITE;
static size_t NUM_ELITE_COPIES;
};
}
#endif /* PARAMS_HPP_ */
