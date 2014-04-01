/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "canvas_ogre.hpp"
#include "battlefield.hpp"
#include "gamestate.hpp"
#include <thread>

namespace tankwar {

using namespace Ogre;

OgreCanvas::OgreCanvas() :
		field_(NULL), initialized_(false) {
}

OgreCanvas::~OgreCanvas() {
}

Coord scale(Coord c) {
	return c/10;
}

string OgreCanvas::generateName(Tank& t, size_t id) {
	return "Tank" + std::to_string(t.teamID_) + "_" + std::to_string(id);
}

string OgreCanvas::generateName(Projectile& p, size_t tid, size_t pid) {
	return "P" + std::to_string(p.owner_->teamID_) + "_" + std::to_string(tid) + "_" + std::to_string(pid);
}

void OgreCanvas::updateTank(Tank& t, const string& name) {
    Coord width = field_->layout_.width_;
    Coord height = field_->layout_.height_;


	Node* tankNode = mSceneMgr->getRootSceneNode()->getChild(name);
	if(!t.dead_) {
		tankNode->setPosition(scale(t.loc_.x_ - (width/2)), scale(t.loc_.y_ - (height/2)), 0);
		tankNode->yaw(Radian(t.rotation_));
    	static_cast<SceneNode*>(tankNode)->setVisible(true);
	} else
		static_cast<SceneNode*>(tankNode)->setVisible(false);
}

void OgreCanvas::updateProjectile(Projectile& p, const string& name) {
    Coord width = field_->layout_.width_;
    Coord height = field_->layout_.height_;

	Node* projectileNode = mSceneMgr->getRootSceneNode()->getChild(name);
    if(!p.dead_) {
    	projectileNode->setPosition(scale(p.loc_.x_ - (width/2)), scale(p.loc_.y_ - (height/2)), 0);
    	projectileNode->yaw(Radian(p.rotation_));
    	static_cast<SceneNode*>(projectileNode)->setVisible(true);
    } else
    	static_cast<SceneNode*>(projectileNode)->setVisible(false);
}

void OgreCanvas::updateTank(Tank& t, size_t id) {
	updateTank(t, generateName(t, id));
}

void OgreCanvas::updateProjectile(Projectile& p, size_t tid, size_t pid) {
	updateProjectile(p, generateName(p, tid, pid));
}

void OgreCanvas::createProjectile(Projectile& p, size_t tid, size_t pid) {
	createProjectile(p, generateName(p, tid, pid));
}

void OgreCanvas::createProjectile(Projectile& p, const string& name) {
    SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);

	RibbonTrail* trail;
	NameValuePairList params;
    params["numberOfChains"] = "2";
    params["maxElements"] = "80";
    trail = (RibbonTrail*)mSceneMgr->createMovableObject("RibbonTrail", &params);
    trail->setMaterialName("Examples/LightRibbonTrail");
    trail->setTrailLength(400);
    trail->setInitialColour(0, 1.0, 0.8, 0);
    trail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
    trail->setInitialWidth(0, 5);

    Entity* ogreHead = mSceneMgr->createEntity(name + ".ent", "ogrehead.mesh");
    headNode->attachObject(ogreHead);
    headNode->scale(scale(1),scale(1),scale(1));

    trail->addNode(headNode);

	Light* light = mSceneMgr->createLight();
    light->setDiffuseColour(ColourValue(255,0,0,255));
	headNode->attachObject(light);

	BillboardSet* bbs = mSceneMgr->createBillboardSet(1);
    bbs->createBillboard(Vector3::ZERO, ColourValue(255,0,0,255));
    bbs->setMaterialName("Examples/Flare");
    headNode->attachObject(bbs);

    updateProjectile(p, name);
}

void OgreCanvas::createTank(Tank& t, size_t id) {
	createTank(t, generateName(t, id));
}

void OgreCanvas::createTank(Tank& t, const string& name) {
    Entity* ogreHead = mSceneMgr->createEntity(name + ".ent", "ogrehead.mesh");
    SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
    headNode->attachObject(ogreHead);
    headNode->scale(scale(5),scale(5),scale(5));
    updateTank(t, name);
}

bool OgreCanvas::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	bool ret = BaseApplication::frameRenderingQueued(evt);
	fieldMutex_.lock();
	if(field_ != NULL)
		updateScene(*field_);
	field_ = NULL;
	fieldMutex_.unlock();
	return ret;
}

void OgreCanvas::updateScene(BattleField& field) {
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];
    size_t tid = 0;
    size_t pid = 0;
    string name;
    for(Tank& t : teamA) {
		string name = generateName(t, tid);
		if(mSceneMgr->hasSceneNode(name))
			updateTank(t, name);
		else
			createTank(t, name);

    	pid = 0;
    	for(Projectile* p : t.projectiles_) {
    		string name = generateName(*p, tid, pid);
    		if(mSceneMgr->hasSceneNode(name))
        		updateProjectile(*p, name);
    		else
    			createProjectile(*p, name);
    		++pid;
    	}
        ++tid;
    }

    tid = 0;
    pid = 0;
    for(Tank& t : teamB) {
		string name = generateName(t, tid);
		if(mSceneMgr->hasSceneNode(name))
			updateTank(t, name);
		else
			createTank(t, name);

    	pid = 0;
    	for(Projectile* p : t.projectiles_) {
    		string name = generateName(*p, tid, pid);
    		if(mSceneMgr->hasSceneNode(name))
        		updateProjectile(*p, name);
    		else
    			createProjectile(*p, name);
    		++pid;
    	}
        ++tid;
    }
}

void OgreCanvas::createScene(BattleField& field) {
	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];

	mSceneMgr->clearScene();
    mSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
    mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 5000, false);

    size_t tid = 0;
    size_t pid = 0;
    for(Tank& t : teamA) {
    	createTank(t, tid);
    	pid = 0;
    	for(Projectile* p : t.projectiles_) {
    		createProjectile(*p, tid, pid);
    		++pid;
    	}
        ++tid;
    }

    tid = 0;
    pid = 0;
    for(Tank& t : teamB) {
    	createTank(t, tid);
    	pid = 0;
    	for(Projectile* p : t.projectiles_) {
    		createProjectile(*p, tid, pid);
    		++pid;
    	}
        ++tid;
    }

    // Create a Light and set its position
    Light* light = mSceneMgr->createLight("MainLight");
    light->setPosition(0.0f, 0.0f, 0.0f);
}

void OgreCanvas::createScene() {
	fieldMutex_.lock();
	assert(field_ != NULL);
	createScene(*field_);
	fieldMutex_.unlock();
}

void OgreCanvas::render(BattleField* field) {
	fieldMutex_.lock();
	if(GameState::getInstance()->isRunning()) {
		field_ = field;
		if(!initialized_) {
			initialized_ = true;
			std::thread t([&]() {
				this->go();
				GameState::getInstance()->stop();
			});

			t.detach();
		}
	}
	fieldMutex_.unlock();
}
}
