#ifndef TANK_HPP_
#define TANK_HPP_

#include "object.hpp"
#include "projectile.hpp"
#include "params.hpp"
#include "battlefield.hpp"
#include "2d.hpp"
#include <cstring>
#include <iostream>
#include "brain.hpp"

namespace tankwar {

class Tank : public Object {
	bool wantsShoot_ = false;

public:
	std::vector<Projectile> projectiles_;

	size_t ammonition_ = Params::MAX_AMMO;
	Brain brain_;
	size_t teamID_;
	Coord lthrust_ = 0;
	Coord rthrust_ = 0;
	size_t friendly_fire_ = 0;
	size_t hits_ = 0;
	size_t damage_ = 0;
	size_t cool_down = 0;
	double fitness_ = 0;

	Tank(size_t teamID, BrainLayout layout, Vector2D loc, Coord rotation);
	~Tank() {};

	bool operator==(const Tank& other) const {
		return this->loc_ == other.loc_ && this->teamID_ == other.teamID_;
	}

	bool operator!=(const Tank& other) const {
		return !this->operator ==(other);
	}

	void setDirection(Vector2D dir);
	void setRotation(double r);

	void calculateFitness();
	void think(BattleField& field);
	void move();
	Tank makeChild();

	Tank clone();
	void resetGameState();
	void resetScore();


	bool operator<(const Tank& other) const	{
		return (this->fitness_ < other.fitness_);
	}
private:
	Projectile& shoot() {
		assert(ammonition_ > 0);
		--ammonition_;
		projectiles_.push_back(Projectile(*this, loc_, rotation_));
		return *(projectiles_.end() - 1);
	}
};


} /* namespace tankwar */

#endif /* TANK_HPP_ */
