#include "params.hpp"

namespace tankwar {
size_t Params::FRAME_PER_SECOND = 60;

//Neural network
size_t Params::Params::NUM_INPUTS = 4;
size_t Params::NUM_HIDDEN = 1;
size_t Params::NUM_NEURONS_PER_HIDDEN = 6;
size_t Params::NUM_LAYERS = 6;
size_t Params::NUM_OUTPUTS = 2;
double Params::ACTIVATION_RESPONSE = 1;
double Params::BIAS = -1;

//Tank
size_t Params::MAX_PROJECTILES=10;
size_t Params::MAX_DAMAGE=200;
double Params::MAX_TURN_RATE = 0.3;
double Params::MAX_TANK_SPEED = 4;
double Params::TANK_SCALE = 5;
size_t Params::TANK_RANGE = 5;

//Population
size_t Params::NUM_TANKS = 3;
size_t Params::NUM_TICKS = 2000;

//Projectile
double Params::PROJECTILE_SCALE = 1;
double Params::MAX_PROJECTILE_SPEED = 12;
int Params::PROJECTILE_RANGE = 3;

//Genetic
double Params::CROSSOVER_RATE = 0.7;
size_t Params::CROSSOVER_ITERATIONS = 10;
double Params::MUTATION_RATE = 0.1;
double Params::MAX_PERTUBATION = 0.3;
size_t Params::NUM_ELITE = 4;
size_t Params::NUM_ELITE_COPIES = 1;
}
