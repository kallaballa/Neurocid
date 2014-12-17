#ifndef TANK_HPP_
#define TANK_HPP_

#include "object.hpp"
#include "projectile.hpp"
#include "facility.hpp"
#include "battlefieldlayout.hpp"
#include "2d.hpp"
#include <cstring>
#include <iostream>
#include "brain_swarm.hpp"
#include "scan.hpp"
#include "history.hpp"
#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace neurocid {

typedef BrainSwarm Brain;
struct BattleFieldLayout;

struct ShipLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	ProjectileLayout pl_;

	bool isDummy_;
	bool canShoot_;
	bool canRotate_;
	bool canMove_;
  bool canJump_;
	bool disableProjectileFitness_;

	Coord radius_;
	Coord maxSpeed_;
	Coord maxRotation_;
	Coord maxFuel_;
	Coord startFuel_;
	Coord jumpRate_;
	Coord fuelRate_;
	Coord hardness_;

	size_t maxAmmo_;
	size_t startAmmo_;
	size_t maxCooldown_;
	size_t maxDamage_;
	size_t crashesPerDamage_;
	size_t numPerfDesc_;

	string fitnessFunction_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & pl_;

	  ar & isDummy_;
	  ar & canShoot_;
	  ar & canRotate_;
	  ar & canMove_;
	  ar & canJump_;
	  ar & disableProjectileFitness_;

	  ar & radius_;
	  ar & maxSpeed_;
	  ar & maxRotation_;
	  ar & maxFuel_;
	  ar & startFuel_;
	  ar & jumpRate_;
	  ar & fuelRate_;
	  ar & hardness_;

	  ar & maxAmmo_;
	  ar & maxCooldown_;
	  ar & maxDamage_;
	  ar & crashesPerDamage_;
	  ar & numPerfDesc_;
	  ar & fitnessFunction_;
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
	Scan scan_;
	History history_;

	Coord flthrust_ = 0;
	Coord frthrust_ = 0;
	Coord blthrust_ = 0;
	Coord brthrust_ = 0;
	size_t ammo_ = 0;
	size_t cool_down = 0;

	size_t friendlyFire_ = 0;
	size_t crash_ = 0;
	size_t crashDamage_= 0;
	size_t defensiveHits_= 0;
	size_t offensiveHits_ = 0;
	size_t damage_= 0;
	size_t failedShots_ = 0;
	size_t kills_ = 0;

	Coord fuel_ = 0;
	Coord recharged_ = 0 ;

	Coord fitness_ = 0;
	bool isElite = false;
  bool isJumping_ = false;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & boost::serialization::base_object<Object>(*this);
	  ar & teamID_;
	  ar & layout_;
	  ar & brain_;
	}
#endif
	Ship() : Object(SHIP, {0,0}, 0, 0,false, true, false),
			teamID_(0),
			layout_(),
			brain_(NULL),
			scan_(this)
	{};

	Ship(ObjectType type, size_t teamID, ShipLayout tl, Brain* brain = NULL);
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
	void impact(Facility& f);
	void impact(Ship& t);
	void impact(Projectile& p);
	void killed();

	void log();
	void calculateFitness(const BattleFieldLayout& bfl, Facility& f, bool winner);
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
