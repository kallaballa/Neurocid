#include "projectile.hpp"
#include "tank.hpp"

namespace tankwar {
Projectile& Projectile::operator=(const Projectile& other) {
	owner_ = other.owner_;
	nearestEnemyDis_ = other.nearestEnemyDis_;
	loc_ = other.loc_;
	dir_ = other.dir_;
	rotation_ = other.rotation_;
	range_ = other.range_;
	speed_ = other.speed_;
	explode_ = other.explode_;
	dead_ = other.dead_;
	return *this;
}
}

