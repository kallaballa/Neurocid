/*
 * physics.cpp
 *
 *  Created on: Mar 15, 2014
 *      Author: elchaschab
 */

#include "physics.hpp"
#include "battlefield.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "population.hpp"
#include "error.hpp"

namespace neurocid {
using std::vector;

void Physics::wallHit(Ship& t) {
	t.kill();
}

void Physics::wallHit(Projectile& p) {
	p.death();
}

void Physics::wallHit(Blast& ex) {
}

void Physics::collide(Projectile& p1, Projectile& p2) {
  p1.death();
  p2.death();
  if(p1.blast())
    blastingProjectiles_.push_back(&p1);

  if(p2.blast())
    blastingProjectiles_.push_back(&p2);
}

void Physics::collide(Projectile& p, Ship& t) {
	if (t != (*p.owner_)) {
		t.impact(p);
	}
	p.death();
	if(p.blast())
	  blastingProjectiles_.push_back(&p);
}

void Physics::collide(Ship& t1, Ship& t2) {
	t1.impact(t2);
}

void Physics::collide(Blast& ex1, Blast& ex2) {
}

void Physics::collide(Blast& ex, Ship& t) {
  if (t != (*ex.owner_->owner_)) {
    t.impact(*ex.owner_);
  }
}

void Physics::collide(Blast& ex1, Projectile& p) {
  p.death();
  if(p.blast())
    blastingProjectiles_.push_back(&p);
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
	  } else if(oA == NULL && oB != NULL && oB->type() == BLAST && !oB->dead_) {
      wallHit(*static_cast<Blast*>(oB));
      deadBodies_.push_back(contact->GetFixtureB()->GetBody());
    } else if(oB == NULL && oA != NULL && oA->type() == BLAST && !oA->dead_) {
      wallHit(*static_cast<Blast*>(oA));
      deadBodies_.push_back(contact->GetFixtureA()->GetBody());
    } else if(oA == NULL && oB != NULL && oB->type() == SHIP && !oB->dead_) {
		  wallHit(*static_cast<Ship*>(oB));
		  deadBodies_.push_back(contact->GetFixtureB()->GetBody());
	  } else if(oB == NULL && oA != NULL && oA->type() == SHIP && !oA->dead_) {
		  wallHit(*static_cast<Ship*>(oA));
		  deadBodies_.push_back(contact->GetFixtureA()->GetBody());
	  } else if(oA != NULL && oB != NULL && !oA->dead_ && !oB->dead_) {
		  if(oA->type() == PROJECTILE && oB->type() == PROJECTILE) {
			  collide(*static_cast<Projectile*>(oA), *static_cast<Projectile*>(oB));
		  } else if(oA->type() == PROJECTILE && oB->type() == SHIP) {
			  collide(*static_cast<Projectile*>(oA), *static_cast<Ship*>(oB));
		  } else if(oA->type() == SHIP && oB->type() == PROJECTILE) {
			  collide(*static_cast<Projectile*>(oB), *static_cast<Ship*>(oA));
		  } else if(oA->type() == BLAST && oB->type() == BLAST) {
        collide(*static_cast<Blast*>(oA), *static_cast<Blast*>(oB));
      } else if(oA->type() == BLAST && oB->type() == SHIP) {
        collide(*static_cast<Blast*>(oA), *static_cast<Ship*>(oB));
      } else if(oA->type() == BLAST && oB->type() == PROJECTILE) {
        collide(*static_cast<Blast*>(oA), *static_cast<Projectile*>(oB));
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

void Physics::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
 {
     b2Fixture* fixtureA = contact->GetFixtureA();
     b2Fixture* fixtureB = contact->GetFixtureB();

     //fixture with user data is a target, other fixture is an arrow
     Object* oA = (Object*)fixtureA->GetBody()->GetUserData();
     Object* oB = (Object*)fixtureB->GetBody()->GetUserData();

     if(oA != NULL && oB != NULL && oA->type() == SHIP && oB->type() == SHIP) {
    	 Ship* sA = static_cast<Ship*>(oA);
    	 Ship* sB = static_cast<Ship*>(oB);
		 if ( impulse->normalImpulses[0] > 300 * sA->layout_.hardness_ ) {
			 collide(*sA,*sB);
		 } else if (impulse->normalImpulses[0] > 300 * sB->layout_.hardness_) {
			 collide(*sB,*sA);
		 }
     }
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

b2Body* Physics::makeShipBody(Ship& t) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(toMeters(t.loc_.x_), toMeters(t.loc_.y_));

    CHECK(t.rotation_ <= M_PI);
    bodyDef.angle = t.rotation_;
    bodyDef.userData = static_cast<Object*>(&t);
    bodyDef.allowSleep = true;
    bodyDef.awake = true;
    b2Body* body = world_.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape dynamicCircle;
    dynamicCircle.m_p.Set(0, 0);
    dynamicCircle.m_radius = toMeters(t.radius_);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 0.2f;

    fixtureDef.restitution = 0.5f;

    // Override the default friction.
    fixtureDef.friction = 0.0f;
    fixtureDef.filter.maskBits = 1;
    fixtureDef.filter.categoryBits = 3;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0);
    body->SetAngularDamping(0);
    return body;
}

b2Body* Physics::makeProjectileBody(Projectile& p) {
	b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(toMeters(p.loc_.x_), toMeters(p.loc_.y_));
    CHECK(p.rotation_ <= M_PI);
    bodyDef.angle = p.rotation_;
    bodyDef.allowSleep = true;
    bodyDef.awake = true;
    bodyDef.userData = static_cast<Object*>(&p);
    b2Body* body = world_.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape dynamicCircle;
    dynamicCircle.m_p.Set(0, 0);
    dynamicCircle.m_radius = toMeters(p.radius_);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 0.001;

    // Override the default friction.
    fixtureDef.friction = 0.3f;

    //Deativate projectile collisions
    fixtureDef.filter.maskBits = 2;
    fixtureDef.filter.categoryBits = 1;
//    fixtureDef.filter.maskBits = 1;
//    fixtureDef.filter.categoryBits = 3;
    body->SetFixedRotation(true);
    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0);
    body->SetAngularDamping(0);
    return body;
}

b2Body* Physics::makeBlastBody(Projectile& p) {
  CHECK(p.blast_ != NULL);
  b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(toMeters(p.loc_.x_), toMeters(p.loc_.y_));
    CHECK(p.rotation_ <= M_PI);
    bodyDef.angle = p.rotation_;
    bodyDef.allowSleep = true;
    bodyDef.awake = true;
    bodyDef.userData = p.blast_;
    b2Body* body = world_.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape dynamicCircle;
    dynamicCircle.m_p.Set(0, 0);
    dynamicCircle.m_radius = toMeters(p.blast_->radius_);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1;

    // Override the default friction.
    fixtureDef.friction = 0.3f;

    //Deativate projectile collisions
    fixtureDef.filter.maskBits = 1;
    fixtureDef.filter.categoryBits = 3;
//    fixtureDef.filter.maskBits = 1;
//    fixtureDef.filter.categoryBits = 3;
    body->SetFixedRotation(true);
    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0);
    body->SetAngularDamping(0);
    return body;
}

Physics::Physics(PhysicsLayout pl) :
	world_(b2Vec2(pl.gravity_.x, pl.gravity_.y)),
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
		for(Ship& t: team) {
			makeShipBody(t);
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

void Physics::update(vector<Ship*> spawned) {
	for(Ship* t : spawned) {
		makeShipBody(*t);
	}
}

void Physics::step() {
	for (b2Body* body = world_.GetBodyList(); body; body = body->GetNext()) {
		if(body->GetUserData() != NULL) {
			Object* o = (Object*)body->GetUserData();
			if(o->type() == SHIP) {
				Ship* t = static_cast<Ship*>(o);
				Vector2D dir = o->getDirection();
				Vector2D across1 = dir;
				Vector2D across2 = dir;
				across1.rotate(-45);
				across2.rotate(45);

				if(t->fuel_ > 0) {
					Coord fLeft = t->fuel_;
					Coord fRate = t->layout_.fuelRate_;
					Coord fRequired = std::abs(t->flthrust_ * fRate) +
							std::abs(t->frthrust_ * fRate) +
							std::abs(t->blthrust_ * fRate) +
							std::abs(t->brthrust_ * fRate);

					if(fLeft < fRequired) {
						Coord diff = (fRequired - fLeft) / 4;
						t->flthrust_ -= diff;
						t->frthrust_ -= diff;
						t->blthrust_ -= diff;
						t->brthrust_ -= diff;
						fRequired = fLeft;
					}
					t->fuel_ -= fRequired;

					Vector2D flforce = across2 * (t->flthrust_ * t->layout_.maxSpeed_ * 8);
					Vector2D frforce = across1 * -(t->frthrust_ * t->layout_.maxSpeed_ * 8);
					Vector2D blforce = across1 * -(t->blthrust_ * t->layout_.maxSpeed_ * 8);
					Vector2D brforce = across2 * (t->brthrust_ * t->layout_.maxSpeed_ * 8);

					b2Vec2 wc = body->GetWorldCenter();
					Vector2D flengine(wc.x, wc.y);
					Vector2D frengine(wc.x, wc.y);
					Vector2D blengine(wc.x, wc.y);
					Vector2D brengine(wc.x, wc.y);

					flengine += (across1 * (t->radius_));
					frengine += (across2 * (t->radius_));
					blengine += (across2 * -(t->radius_));
					brengine += (across1 * -(t->radius_));
					Vector2D totalForce;
					totalForce += flforce;
					totalForce += frforce;
					totalForce += blforce;
					totalForce += brforce;

					if(t->isJumping_) {
					  b2Vec2 tf(totalForce.x_ * 30, totalForce.y_ * 30);
					  b2Vec2 newpos = body->GetPosition() + tf;
					  body->SetTransform(newpos, body->GetAngle());
					} else {
            body->ApplyLinearImpulse(b2Vec2(flforce.x_, flforce.y_), b2Vec2(flengine.x_, flengine.y_),false);
            body->ApplyLinearImpulse(b2Vec2(frforce.x_, frforce.y_), b2Vec2(frengine.x_, frengine.y_),false);
            body->ApplyLinearImpulse(b2Vec2(blforce.x_, blforce.y_), b2Vec2(blengine.x_, blengine.y_),false);
            body->ApplyLinearImpulse(b2Vec2(brforce.x_, brforce.y_), b2Vec2(brengine.x_, brengine.y_),false);
					}
				} else {
					t->flthrust_ = 0;
					t->frthrust_ = 0;
					t->blthrust_ = 0;
					t->brthrust_ = 0;
					if(!t->layout_.isDummy_) {
						t->death();
						deadBodies_.push_back(body);
					}
				}
				if (!t->layout_.canMove_ || t->layout_.isDummy_) {
					body->SetLinearVelocity({0,0});
				}

				if(!t->layout_.canRotate_ || t->layout_.isDummy_)
					body->SetAngularVelocity(0);
				else {
					float32 angVel = body->GetAngularVelocity();
					if(angVel > 5)
						body->SetAngularVelocity(5);
					else if(angVel < -5)
						body->SetAngularVelocity(-5);
				}

				b2Vec2 vel = body->GetLinearVelocity();
				float speed = vel.Normalize();
				float maxSpeed = 150.0 * t->layout_.maxSpeed_;
				if ( speed > maxSpeed ) {
				    body->SetLinearVelocity( maxSpeed * vel);
				}

				CHECK(o->rotation_ <= M_PI);
			} else if(o->type() == PROJECTILE) {
				Projectile* p = static_cast<Projectile*>(o);
				if((p->loc_ - p->startLoc_).length() > p->layout_.maxTravel_) {
				  p->death();
				  /*if(p->blast())
				    makeBlastBody(*p);
				  deadBodies_.push_back(body);*/
				} else {
					Vector2D force = p->getDirection() * (p->layout_.maxSpeed_ * 10000);
					body->SetAwake(true);
					body->SetLinearVelocity(b2Vec2(force.x_, force.y_));
				}
      } else if(o->type() == BLAST) {
        Blast* ex = static_cast<Blast*>(o);
        ex->dead_ = true;
        deadBodies_.push_back(body);
      }
		}
	}

	world_.Step(layout_.timeStep_, layout_.velocityIterations_, layout_.positionIterations_);
	//cleanup the dead
	for(b2Body* body : deadBodies_) {
		world_.DestroyBody(body);
	}
	deadBodies_.clear();

  //create blast bodies
  for(Projectile* p : blastingProjectiles_) {
    makeBlastBody(*p);
  }

  blastingProjectiles_.clear();

	//FIXME don't copy coordinates
	for (b2Body* body = world_.GetBodyList(); body; body = body->GetNext()) {
		if(body->GetUserData() != NULL) {
			b2Vec2 pos = body->GetPosition();
			Object* obj = (Object*)body->GetUserData();
			b2Vec2 vel = body->GetLinearVelocity();
			Coord angVel = body->GetAngularVelocity();
			obj->vel_.x_ = vel.x;
			obj->vel_.y_ = vel.y;
			CHECK(!std::isnan(angVel));
			obj->angVel_ = angVel;
			obj->loc_.x_ = toCoord(pos.x);
			obj->loc_.y_ = toCoord(pos.y);
			obj->rotation_ = normRotation(body->GetAngle());
			CHECK(obj->rotation_ <= M_PI);
		}
	}
}

} /* namespace neurocid */
