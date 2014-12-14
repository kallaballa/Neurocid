/*
 * physics.hpp
 *
 *  Created on: Mar 15, 2014
 *      Author: elchaschab
 */

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include "projectile.hpp"
#include "2d.hpp"
#include <Box2D/Box2D.h>
#include <vector>



namespace neurocid {
using std::vector;
class Ship;
class BattleField;
struct BattleFieldLayout;

struct PhysicsLayout {
	b2Vec2 gravity_ ;
	float32	 coordToMetersFactor_;
    float32	 timeStep_;
    int   velocityIterations_;
    int   positionIterations_;
};

class Physics  : public b2ContactListener {
private:
	b2World world_;
	PhysicsLayout layout_;
	std::vector<b2Body*> deadBodies_;
  std::vector<Projectile*> blastingProjectiles_;

	float32 toMeters(Coord c);
	Coord toCoord(float32 m);

	b2Body* makeWorldBox(BattleFieldLayout& bfl);
	b2Body* makeShipBody(Ship& t);
	b2Body* makeProjectileBody(Projectile& p);
	b2Body* makeBlastBody(Projectile& p);

	void wallHit(Ship& t);
	void wallHit(Projectile& p);
	void wallHit(Blast& ex);
	void collide(Projectile& p1, Projectile& p2);
	void collide(Projectile& p, Ship& t);
	void collide(Ship& t1, Ship& t2);
  void collide(Blast& ex1, Blast& ex2);
  void collide(Blast& ex, Ship& t);
  void collide(Blast& ex1, Projectile& p);

  void BeginContact(b2Contact* contact)  override;
	void EndContact(b2Contact* contact) override;
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
public:
	Physics(PhysicsLayout pl);
	virtual ~Physics();

	void create(BattleField& field);
	void update(vector<Projectile*> spawned);
	void update(vector<Ship*> spawned);
	void step();
};

} /* namespace neurocid */

#endif /* PHYSICS_HPP_ */
