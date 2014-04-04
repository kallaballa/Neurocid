#ifndef TANK_HPP_
#define TANK_HPP_

#include "object.hpp"
#include "projectile.hpp"
#include "battlefieldlayout.hpp"
#include "2d.hpp"
#include <cstring>
#include <iostream>
#include "brain.hpp"
#include "scan.hpp"
#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

namespace neurocid {

struct ShipLayout {
#ifndef _NO_SERIALIZEz
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
	size_t num_perf_desc;

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
	  ar & num_perf_desc;
	}
#endif
};

using std::numeric_limits;
class Ship : public Object {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif

	bool willShoot_ = false;
public:
	typedef std::vector<Coord> PerfDesc;
	std::vector<Projectile*> projectiles_;
	PerfDesc perfDesc_;

	size_t teamID_;
	ShipLayout layout_;
	Brain* brain_;

	size_t ammonition_;
	Coord flthrust_;
	Coord frthrust_;
	Coord blthrust_;
	Coord brthrust_;

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
	  ar & boost::serialization::base_object<Object>(*this);
	  ar & teamID_;
	  ar & layout_;
	  ar & brain_;
	}
#endif
	Ship() : Object(SHIP, {0,0}, 0, 0, false, false),
			teamID_(0),
			layout_(),
			brain_(NULL),
			scan_(*this){};
	Ship(size_t teamID, ShipLayout tl, Brain* brain = NULL);
	~Ship() {
	};

	void setBrain(Brain* b);

	void think(BattleFieldLayout& bfl);
	void move(BattleFieldLayout& bfl);
	void damage();
	void death();
	void kill();
	void crash();
	void impact(Ship& t);
	void impact(Projectile& p);

	void calculateFitness();
	void resetGameState();
	void resetScore();
	void update(ShipLayout tl);
	Ship makeChild() const;
	Ship clone() const;
	bool operator<(const Ship& other) const;
	bool operator==(const Ship& other) const;
	bool operator!=(const Ship& other) const;

	Projectile* shoot();
	bool willShoot();
private:
};

} /* namespace neurocid */

#endif /* TANK_HPP_ */
