#ifndef TANK_HPP_
#define TANK_HPP_

#include "object.hpp"
#include "projectile.hpp"
#include "battlefield.hpp"
#include "2d.hpp"
#include <cstring>
#include <iostream>
#include "brain.hpp"
#include "scanner.hpp"
#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

namespace tankwar {

struct TankLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	ProjectileLayout pl_;
	bool isDummy_;
	bool canShoot_;
	bool canRotate_;
	bool canMove_;
	bool disableProjectileFitness_;

	Coord range_;
	Coord max_speed_;
	Coord max_rotation_;

	size_t max_cooldown;
	size_t max_ammo_;
	size_t max_damage_;
	size_t crashes_per_damage_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & pl_;

	  ar & isDummy_;
	  ar & canShoot_;
	  ar & canRotate_;
	  ar & canMove_;
	  ar & disableProjectileFitness_;

	  ar & range_;
	  ar & max_speed_;
	  ar & max_rotation_;

	  ar & max_cooldown;
	  ar & max_ammo_;
	  ar & max_damage_;
	  ar & crashes_per_damage_;
	}
#endif
};

using std::numeric_limits;
class Tank : public Object {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif

	bool willShoot_ = false;
public:
	std::vector<Projectile*> projectiles_;

	size_t teamID_;
	TankLayout layout_;
	Brain* brain_;

	size_t ammonition_;
	Coord lthrust_;
	Coord rthrust_;
	size_t friendlyFire_;
	size_t crash_;
	size_t crashDamage_;
	size_t hits_;
	size_t damage_;
	size_t cool_down;
	Coord fitness_ = 0;
	Scan scan_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & teamID_;
	  ar & layout_;
	  ar & brain_;
	}
#endif
	Tank() : Object(TANK, {0,0}, 0, 0, false, false),
			teamID_(0),
			layout_(),
			brain_(NULL),
			scan_(*this){};
	Tank(size_t teamID, TankLayout tl, Brain* brain = NULL);
	~Tank() {
	};

	void setBrain(Brain* b);

	void think(BattleFieldLayout& bfl);
	void move(BattleFieldLayout& bfl);
	void damage();
	void death();
	void kill();
	void crash();
	void impact(Tank& t);
	void impact(Projectile& p);

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
