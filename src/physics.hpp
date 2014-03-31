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



namespace tankwar {
using std::vector;
class Tank;
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

	float32 toMeters(Coord c);
	Coord toCoord(float32 m);

	b2Body* makeWorldBox(BattleFieldLayout& bfl);
	b2Body* makeTankBody(Tank& t);
	b2Body* makeProjectileBody(Projectile& p);
	void wallHit(Tank& t);
	void wallHit(Projectile& p);
	void collide(Projectile& p1, Projectile& p2);
	void collide(Projectile& p, Tank& t);
	void collide(Tank& t1, Tank& t2);
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
public:
	Physics(PhysicsLayout pl);
	virtual ~Physics();

	void create(BattleField& field);
	void update(vector<Projectile*> spawned);
	void update(vector<Tank*> spawned);
	void step();
};

} /* namespace tankwar */

#endif /* PHYSICS_HPP_ */
