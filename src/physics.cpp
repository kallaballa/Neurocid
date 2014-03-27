/*
 * physics.cpp
 *
 *  Created on: Mar 15, 2014
 *      Author: elchaschab
 */

#include "physics.hpp"
#include "battlefield.hpp"
#include "tank.hpp"
#include "projectile.hpp"
#include "population.hpp"

namespace tankwar {
using std::vector;

void Physics::wallHit(Tank& t) {
	t.kill();
}

void Physics::wallHit(Projectile& p) {
	p.death();
}

void Physics::collide(Projectile& p1, Projectile& p2) {
	p1.death();
	p2.death();
}

void Physics::collide(Projectile& p, Tank& t) {
	if (t != (*p.owner_)) {
		t.impact(p);
	}
}

void Physics::collide(Tank& t1, Tank& t2) {
	t1.impact(t2);
}

void Physics::BeginContact(b2Contact* contact) {
  Object* oA = static_cast<Object*>(contact->GetFixtureA()->GetBody()->GetUserData());
  Object* oB = static_cast<Object*>(contact->GetFixtureB()->GetBody()->GetUserData());

  if(oA != NULL || oB != NULL) {
	  if(oA == NULL && oB != NULL && oB->type() == PROJECTILE && !oB->dead_) {
		  wallHit(*static_cast<Projectile*>(oB));
		  deadBodies_.push_back(contact->GetFixtureB()->GetBody());
	  } else if(oB == NULL && oA != NULL && oA->type() == PROJECTILE && !oA->dead_) {
		  wallHit(*static_cast<Projectile*>(oA));
		  deadBodies_.push_back(contact->GetFixtureA()->GetBody());
	  } else if(oA == NULL && oB != NULL && oB->type() == TANK && !oB->dead_) {
		  wallHit(*static_cast<Tank*>(oB));
		  deadBodies_.push_back(contact->GetFixtureB()->GetBody());
	  } else if(oB == NULL && oA != NULL && oA->type() == TANK && !oA->dead_) {
		  wallHit(*static_cast<Tank*>(oA));
		  deadBodies_.push_back(contact->GetFixtureA()->GetBody());
	  } else if(oA != NULL && oB != NULL && !oA->dead_ && !oB->dead_) {
		  if(oA->type() == PROJECTILE && oB->type() == PROJECTILE) {
			  collide(*static_cast<Projectile*>(oA), *static_cast<Projectile*>(oB));
		  } else if(oA->type() == PROJECTILE && oB->type() == TANK) {
			  collide(*static_cast<Projectile*>(oA), *static_cast<Tank*>(oB));
		  } else if(oA->type() == TANK && oB->type() == PROJECTILE) {
			  collide(*static_cast<Projectile*>(oB), *static_cast<Tank*>(oA));
		  } else if(oA->type() == TANK && oB->type() == TANK) {
			  collide(*static_cast<Tank*>(oB), *static_cast<Tank*>(oA));
		  }

		  if(oA->dead_)
			  deadBodies_.push_back(contact->GetFixtureA()->GetBody());

		  if(oB->dead_)
			  deadBodies_.push_back(contact->GetFixtureB()->GetBody());
	  }

  }
}

void Physics::EndContact(b2Contact* contact) {

}

float32 Physics::toMeters(Coord c) {
	return c * layout_.coordToMetersFactor_;
}

Coord Physics::toCoord(float32 m) {
	return m / layout_.coordToMetersFactor_;
}

b2Body* Physics::makeWorldBox(BattleFieldLayout& bfl) {
	b2Body* body;
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
    bodyDef.position.Set(0, 0);
    bodyDef.userData = NULL;
    bodyDef.allowSleep = true;
    bodyDef.awake = false;
    //shape definition
    b2EdgeShape topShape;
    b2EdgeShape leftShape;
    b2EdgeShape bottomShape;
    b2EdgeShape rightShape;
    float32 w = toMeters(bfl.width_);
    float32 h = toMeters(bfl.height_);

    topShape.Set(	b2Vec2(0,0),  b2Vec2(w,0));
    leftShape.Set(	b2Vec2(0,0),  b2Vec2(0,h));
    bottomShape.Set(b2Vec2(0,h),  b2Vec2(w,h));
    rightShape.Set(	b2Vec2(w,0),  b2Vec2(w,h));

	//fixture definition
	b2FixtureDef topFixdef;
	topFixdef.shape = &topShape;
	topFixdef.density = 1;
	topFixdef.filter.maskBits = 3;
	topFixdef.filter.categoryBits = 3;

	b2FixtureDef leftFixdef;
	leftFixdef.shape = &leftShape;
	leftFixdef.density = 1;
	leftFixdef.filter.maskBits = 3;
	leftFixdef.filter.categoryBits = 3;

	b2FixtureDef bottomFixdef;
	bottomFixdef.shape = &bottomShape;
	bottomFixdef.density = 1;
	bottomFixdef.filter.maskBits = 3;
	bottomFixdef.filter.categoryBits = 3;

	b2FixtureDef rightFixdef;
	rightFixdef.shape = &rightShape;
	rightFixdef.density = 1;
	rightFixdef.filter.maskBits = 3;
	rightFixdef.filter.categoryBits = 3;

	body = world_.CreateBody(&bodyDef);
	body->CreateFixture(&topFixdef);
	body->CreateFixture(&leftFixdef);
	body->CreateFixture(&bottomFixdef);
	body->CreateFixture(&rightFixdef);
	return body;
}

b2Body* Physics::makeTankBody(Tank& t) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(toMeters(t.loc_.x_), toMeters(t.loc_.y_));

    assert(t.rotation_ <= M_PI);
    bodyDef.angle = t.rotation_;
    bodyDef.userData = static_cast<Object*>(&t);
    bodyDef.allowSleep = true;
    bodyDef.awake = true;
    b2Body* body = world_.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape dynamicCircle;
    dynamicCircle.m_p.Set(0, 0);
    dynamicCircle.m_radius = toMeters(t.range_);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 10.0f;

    fixtureDef.restitution = 0.3f;

    // Override the default friction.
    fixtureDef.friction = 0.3f;
    fixtureDef.filter.maskBits = 1;
    fixtureDef.filter.categoryBits = 3;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0.3);
    body->SetAngularDamping(1);
    return body;
}

b2Body* Physics::makeProjectileBody(Projectile& p) {
	b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(toMeters(p.loc_.x_), toMeters(p.loc_.y_));
    assert(p.rotation_ <= M_PI);
    bodyDef.angle = p.rotation_;
    bodyDef.allowSleep = true;
    bodyDef.awake = true;
    bodyDef.userData = static_cast<Object*>(&p);
    b2Body* body = world_.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape dynamicCircle;
    dynamicCircle.m_p.Set(0, 0);
    dynamicCircle.m_radius = toMeters(p.range_);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 0.01;

    // Override the default friction.
    fixtureDef.friction = 0.3f;

//Deativate projectile collisions
//    fixtureDef.filter.maskBits = 2;
//    fixtureDef.filter.categoryBits = 1;
    fixtureDef.filter.maskBits = 1;
    fixtureDef.filter.categoryBits = 3;
    body->SetFixedRotation(true);

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0.1);
    body->SetAngularDamping(0);
    return body;
}

Physics::Physics(PhysicsLayout pl) :
	world_(b2Vec2(layout_.gravity_.x, layout_.gravity_.y)),
	layout_(pl) {
	//FIXME why do we have to set the gravity after construction?
	b2Vec2 g1 = b2Vec2(layout_.gravity_.x, layout_.gravity_.y);
	world_.SetGravity(g1);
}

Physics::~Physics() {
	world_.ClearForces();
}

void Physics::create(BattleField& field) {
	makeWorldBox(field.layout_);
	for(Population& team : field.teams_) {
		for(Tank& t: team) {
			makeTankBody(t);
			for(Projectile* p : t.projectiles_) {
				makeProjectileBody(*p);
			}
		}
	}

	world_.SetContactListener(this);
}

void Physics::update(vector<Projectile*> spawned) {
	for(Projectile* p : spawned) {
		makeProjectileBody(*p);
	}
}

void Physics::update(vector<Tank*> spawned) {
	for(Tank* t : spawned) {
		makeTankBody(*t);
	}
}

void Physics::step() {
	for (b2Body* body = world_.GetBodyList(); body; body = body->GetNext()) {
		if(body->GetUserData() != NULL) {
			Object* o = (Object*)body->GetUserData();
			if(o->type() == TANK) {
				Tank* t = static_cast<Tank*>(o);
				Vector2D force = o->getDirection() * (o->speed_ * 120);
				if(t->layout_.canMove_ && !t->layout_.isDummy_)
					body->ApplyForce(b2Vec2(force.x_, force.y_), body->GetWorldCenter());
				else
					body->SetLinearVelocity(b2Vec2(0,0));

				if(t->layout_.canRotate_ && !t->layout_.isDummy_)
					body->ApplyTorque(o->rotForce_ * 3);
				else
					body->SetAngularVelocity(0);

				assert(o->rotation_ <= M_PI);
			} else if(o->type() == PROJECTILE) {
				Projectile* p = static_cast<Projectile*>(o);
				if(hypot(p->loc_.x_ - p->startLoc_.x_, p->loc_.y_ - p->startLoc_.y_) > p->layout_.max_travel_) {
					p->dead_ = true;
					deadBodies_.push_back(body);
				} else {
					Vector2D force = o->getDirection() * o->speed_ * 120;
					body->SetAwake(true);
					body->ApplyLinearImpulse(b2Vec2(force.x_, force.y_), body->GetWorldCenter());
				}
			}
		}
	}

	world_.Step(layout_.timeStep_, layout_.velocityIterations_, layout_.positionIterations_);
	//cleanup the dead
	for(b2Body* body : deadBodies_) {
		world_.DestroyBody(body);
	}
	deadBodies_.clear();

	//FIXME don't copy coordinates
	for (b2Body* body = world_.GetBodyList(); body; body = body->GetNext()) {
		if(body->GetUserData() != NULL) {
			b2Vec2 pos = body->GetPosition();
			Object* obj = (Object*)body->GetUserData();
			b2Vec2 vel = body->GetLinearVelocity();
			Coord angVel = body->GetAngularVelocity();
			obj->vel_.x_ = vel.x;
			obj->vel_.y_ = vel.y;
			assert(!std::isnan(angVel));
			obj->angVel_ = angVel;
			obj->loc_.x_ = toCoord(pos.x);
			obj->loc_.y_ = toCoord(pos.y);
			obj->rotation_ = normRotation(body->GetAngle());
			assert(obj->rotation_ <= M_PI);
		}
	}
}

} /* namespace tankwar */
