/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "ogre_app.hpp"
#include "canvas.hpp"
#include <mutex>

namespace tankwar {
class BattleField;

class OgreCanvas : public Canvas, public BaseApplication
{
	std::mutex fieldMutex_;
	BattleField* field_;
	bool initialized_;
public:
    OgreCanvas(void);
    virtual ~OgreCanvas(void);

protected:
    string generateName(Projectile& p, size_t tid, size_t pid);
    void updateProjectile(Projectile& p, const string& name);
    void updateProjectile(Projectile& p, size_t tid, size_t pid);
    void createProjectile(Projectile& p, size_t tid, size_t pid);
    void createProjectile(Projectile& p, const string& name);
    string generateName(Tank& t, size_t id);
    void updateTank(Tank& t, const string& name);
    void updateTank(Tank& t, size_t id);
    void createTank(Tank& t, size_t id);
    void createTank(Tank& t, const string& name);

    bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual void updateScene(BattleField& field);
    virtual void createScene(BattleField& field);
    virtual void createScene();
    virtual void render(BattleField* field);
};
}

#endif // #ifndef __TutorialApplication_h_
