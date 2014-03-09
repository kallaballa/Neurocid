#include "params.hpp"

namespace tankwar {
double Params::ACTIVATION_RESPONSE = 1;
double Params::BIAS = -1;

//Tank
size_t Params::MAX_AMMO=10;
size_t Params::MAX_DAMAGE=5;
double Params::MAX_TURN_RATE = 0.1;
double Params::MAX_TANK_SPEED = 6;
double Params::TANK_SCALE = 5;
size_t Params::TANK_RANGE = 5;
size_t Params::COOL_DOWN=20;

//Projectile
double Params::PROJECTILE_SCALE = 1;
double Params::MAX_PROJECTILE_SPEED = 6;
int Params::PROJECTILE_RANGE = 3;
}
