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
using std::numeric_limits;
class Tank : public Object {
	bool wantsShoot_ = false;

public:
	struct Scanner {
		Vector2D nearestLoc_ = {numeric_limits<Coord>().max(), numeric_limits<Coord>().max()};
		Coord nearestDis_ = numeric_limits<Coord>().max();
		Vector2D nearestEnemyLoc_ = {numeric_limits<Coord>().max(), numeric_limits<Coord>().max()};
		Vector2D nearestFriendLoc_ = {numeric_limits<Coord>().max(), numeric_limits<Coord>().max()};
		Vector2D nearestFriend2Loc_ = {numeric_limits<Coord>().max(), numeric_limits<Coord>().max()};
	};

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
	Vector2D lastLoc_;
	Scanner scanner_;

	Tank(size_t teamID, BrainLayout layout, Vector2D loc, Coord rotation);
	~Tank() {};

	bool operator==(const Tank& other) const {
		return this->loc_ == other.loc_ && this->teamID_ == other.teamID_;
	}

	bool operator!=(const Tank& other) const {
		return !this->operator ==(other);
	}

	vector<Vector2D> scan() const;

	void setDirection(Vector2D dir);
	void setRotation(double r);

	void calculateFitness();
	void think(BattleField& field);
	void move();
	void stepBack();
	Tank makeChild();

	Tank clone();
	void resetGameState();
	void resetScore();


	bool operator<(const Tank& other) const	{
		return (this->fitness_ < other.fitness_);
	}

	ObjectType type() {
		return ObjectType::TANK;
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
