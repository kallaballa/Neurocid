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
#include "scanner.hpp"

namespace tankwar {

struct TankLayout {
	bool canShoot_;
	bool canRotate_;
	bool canMove_;

	Coord range_;
	Coord max_speed_;
	Coord max_rotation_;

	size_t max_cooldown;
	size_t max_ammo_;
	size_t max_damage_;
};

using std::numeric_limits;
class Tank : public Object {
	bool willShoot_ = false;
public:
	std::vector<Projectile*> projectiles_;

	size_t teamID_;
	TankLayout tl_;
	Brain* brain_;

	size_t ammonition_;
	Coord lthrust_;
	Coord rthrust_;
	size_t friendly_fire_;
	size_t hits_;
	size_t damage_;
	size_t cool_down;
	double fitness_ = 0;
	Scan scan_;

	Tank(size_t teamID, TankLayout tl);
	~Tank() {
	};

	void setBrain(Brain* b);
	void think(BattleFieldLayout& bfl);
	void move(BattleFieldLayout& bfl);

	void calculateFitness();
	void resetGameState();
	void resetScore();
	void update(TankLayout tl);
	Tank makeChild();
	Tank clone();
	bool operator<(const Tank& other) const;
	bool operator==(const Tank& other) const;
	bool operator!=(const Tank& other) const;

	Projectile* shoot();
	bool willShoot();
private:
};


} /* namespace tankwar */

#endif /* TANK_HPP_ */
