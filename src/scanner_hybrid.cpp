/*
 * scanner1.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: elchaschab
 */

#include "scanner.hpp"
#include "population.hpp"
#include "battlefield.hpp"
#include "bsp.hpp"
#include "util.hpp"
#include "error.hpp"

#ifdef _GPU_KMEANS
#include "../src/KMeans.hpp"
#include <ctime>
#endif

namespace neurocid {

void HybridScanner::teamScan(Population& friends, Population& enemies, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, ObjectBsp& bspFriends,
    ObjectBsp& bspEnemies, ObjectBsp& bspFriendFacilities, ObjectBsp& bspEnemyFacilities, ObjectBsp& bspEnemyProjectiles, BattleFieldLayout& bfl) {
  Coord numFriends = ClusterScanner::layout_.numFriends_;
  Coord numEnemies = ClusterScanner::layout_.numEnemies_;
  Coord numFriendFacilities = ClusterScanner::layout_.numFriendFacilities_;
  Coord numEnemyFacilities = ClusterScanner::layout_.numEnemyFacilities_;

  //	Coord numProjectiles = ClusterScanner::layout_.numProjectiles_;
  //size_t rangeOfSight = 10000;

  for (size_t i = 0; i < friends.size(); ++i) {
    Ship& t = friends[i];
    if (!t.dead_ && !t.layout_.isDummy_) {

      t.scan_ = Scan(&t);

      // Scan for friends.
      findNearest(bspFriends, t, FRIEND, t.scan_.objects_);
      findNearestN(bspFriends, t, FRIEND, t.scan_.objects_, numFriends - t.scan_.objects_.size());

      if (t.scan_.objects_.size() > numFriends) {
        t.scan_.objects_.resize(numFriends);
      } else if (t.scan_.objects_.empty()) {
        for (size_t i = 0; i < numFriends; ++i) {
          t.scan_.makeScanObject(FRIEND, NO_VECTOR2D, NO_COORD,NO_VECTOR2D);
        }
      }

      while (t.scan_.objects_.size() < numFriends) {
        size_t s = t.scan_.objects_.size();
        for (size_t i = 0; i < s; ++i) {
          ScanObject& so = t.scan_.objects_[i];
          t.scan_.objects_.push_back(so);
          if (t.scan_.objects_.size() == numFriends)
            break;
        }
      }

      CHECK(t.scan_.objects_.size() == numFriends);

      // Scan for enemies
      findNearest(bspEnemies, t, ENEMY, t.scan_.objects_);
      findNearestN(bspEnemies, t, ENEMY, t.scan_.objects_, numFriends + numEnemies - t.scan_.objects_.size());

      for (Vector2D ce : ctrEnemies) {
        t.scan_.makeScanObject(ENEMY, ce, t.distance(ce), t.vel_);
      }

      if (t.scan_.objects_.size() > (numFriends + numEnemies)) {
        t.scan_.objects_.resize(numFriends + numEnemies);
      }

      if (t.scan_.objects_.size() == numFriends)
        t.scan_.makeScanObject(ENEMY, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);

      while (t.scan_.objects_.size() < (numFriends + numEnemies)) {
        size_t s = t.scan_.objects_.size();
        for (size_t i = numFriends; i < s; ++i) {
          ScanObject& so = t.scan_.objects_[i];
          t.scan_.objects_.push_back(so);
          if (t.scan_.objects_.size() == (numFriends + numEnemies))
            break;
        }
      }

      CHECK(t.scan_.objects_.size() == (numFriends + numEnemies));

      size_t startNum = (numFriends + numEnemies);
      size_t targetNum = (numFriends + numEnemies + numFriendFacilities);
      findNearestN(bspFriendFacilities, t, FRIEND_FACILITY, t.scan_.objects_, numFriendFacilities);

      if (t.scan_.objects_.size() > targetNum) {
        t.scan_.objects_.resize(targetNum);
      } else if (t.scan_.objects_.size() == startNum) {
        for (size_t i = startNum; i < targetNum; ++i) {
          t.scan_.makeScanObject(FRIEND_FACILITY, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);
        }
      }

      while (t.scan_.objects_.size() < targetNum) {
        size_t s = t.scan_.objects_.size();
        for (size_t i = startNum; i < s; ++i) {
          ScanObject& so = t.scan_.objects_[i];
          CHECK(so.type_ == FRIEND_FACILITY);
          t.scan_.objects_.push_back(so);
          if (t.scan_.objects_.size() == targetNum)
            break;
        }
      }

      CHECK(t.scan_.objects_.size() == targetNum);

      startNum = (numFriends + numEnemies + numFriendFacilities);
      targetNum = (numFriends + numEnemies + numFriendFacilities + numEnemyFacilities);
      findNearestN(bspEnemyFacilities, t, ENEMY_FACILITY, t.scan_.objects_, numEnemyFacilities);

      if (t.scan_.objects_.size() > targetNum) {
        t.scan_.objects_.resize(targetNum);
      } else if (t.scan_.objects_.size() == startNum) {
        for (size_t i = startNum; i < targetNum; ++i) {
          t.scan_.makeScanObject(ENEMY_FACILITY, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);
        }
      }

      while (t.scan_.objects_.size() < targetNum) {
        size_t s = t.scan_.objects_.size();
        for (size_t i = startNum; i < s; ++i) {
          ScanObject& so = t.scan_.objects_[i];
          CHECK(so.type_ == ENEMY_FACILITY);
          t.scan_.objects_.push_back(so);
          if (t.scan_.objects_.size() == targetNum)
            break;
        }
      }

      CHECK(t.scan_.objects_.size() == targetNum);

      /*
       // Scan for projectiles
       size_t startNum = (numFriends + numEnemies);
       size_t targetNum = (numFriends + numEnemies + numProjectiles);

       findNearest(bspEnemyProjectiles, t, PROJECTILE_, t.scan_.objects_);
       findInRange(bspEnemyProjectiles, t, PROJECTILE_, t.scan_.objects_, rangeOfSight);

       if(t.scan_.objects_.size() > targetNum) {
       t.scan_.objects_.resize(targetNum);
       } else if(t.scan_.objects_.size() == startNum) {
       for(size_t i = startNum; i < targetNum; ++i) {
       t.scan_.makeScanObject(PROJECTILE_,NO_VECTOR2D,NO_COORD);
       }
       }

       while(t.scan_.objects_.size() < targetNum) {
       size_t s = t.scan_.objects_.size();
       for(size_t i = startNum; i < s; ++i) {
       ScanObject& so = t.scan_.objects_[i];
       CHECK(so.type_ == PROJECTILE_);
       t.scan_.objects_.push_back(so);
       if(t.scan_.objects_.size() == targetNum)
       break;
       }
       }

       CHECK(t.scan_.objects_.size() == targetNum);
       */
      //Scan for near misses
      t.scan_.calculate(bfl);
    }

    for (Projectile* p : t.projectiles_) {
      if (!p->dead_ && !t.layout_.disableProjectileFitness_) {
        p->scan_ = Scan(p);

        auto result = findNearest(bspEnemies, *p);
        CHECK(p->scan_.objects_.empty() || p->scan_.objects_.size() == 2);

        if (result.first != NULL && result.second != NO_COORD) {
          Vector2D enemyLoc = result.first->loc_;
          if(p->scan_.objects_.empty()) {
            p->scan_.makeScanObject(ENEMY, enemyLoc, result.second, NO_VECTOR2D);
          } else if(result.second < p->scan_.objects_[0].dist_) {
            p->scan_.objects_[0].loc_ = enemyLoc;
            p->scan_.objects_[0].dist_ = result.second;
          }
        } else {
          p->scan_.makeScanObject(ENEMY, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);
        }

        result = findNearest(bspFriends, *p);
        if (result.first != NULL && result.second != NO_COORD) {
          Vector2D friendLoc = result.first->loc_;
          if(p->scan_.objects_.size() == 1) {
            p->scan_.makeScanObject(FRIEND, friendLoc, result.second, NO_VECTOR2D);
          } else if(result.second < p->scan_.objects_[0].dist_) {
            p->scan_.objects_[1].loc_ = friendLoc;
            p->scan_.objects_[1].dist_ = result.second;
          }
        } else {
          p->scan_.makeScanObject(FRIEND, NO_VECTOR2D, NO_COORD, NO_VECTOR2D);
        }
        p->scan_.calculate(bfl);
      }

      if (p->blast_ != NULL && !p->blast_->dead_) {
        vector<Object*> result;
        findInRange(bspFriends, *p->blast_, result, p->blast_->layout_.radius_);
        findInRange(bspEnemies, *p->blast_, result, p->blast_->layout_.radius_);
        findInRange(bspFriendFacilities, *p->blast_, result, p->blast_->layout_.radius_);
        findInRange(bspEnemyFacilities, *p->blast_, result, p->blast_->layout_.radius_);

        size_t cntColateral = 0;
        for (Object* o : result) {
          if(cntColateral >= p->blast_->layout_.maxColateral_)
            break;
          if (o->type() == SHIP) {
            static_cast<Ship*>(o)->stun();
            static_cast<Ship*>(o)->impact(*p);
            ++cntColateral;
          } else if (o->type() == FACILITY) {
            /*Facility* f = static_cast<Facility*>(o);
            f->damage();

            if (p->owner_->teamID_ != f->teamID_) {
              p->owner_->defensiveHits_++;
            } else {
              p->owner_->friendlyFire_++;
            }

            if(f->dead_ && p->owner_->teamID_ != f->teamID_)
              p->owner_->killed();
            ++cntColateral;*/
          }
        }

        p->blast_->death();
      }
    }

  }
}

void HybridScanner::scan(BattleField& field) {
  CHECK(field.teams_.size() == 2);
  prepare(field);
  Population& teamA = field.teams_[0];
  Population& teamB = field.teams_[1];
  teamScan(teamA, teamB, centersA_, centersB_, bspA_, bspB_, bspFA_, bspFB_, bspPB_, field.layout_);
  teamScan(teamB, teamA, centersB_, centersA_, bspB_, bspA_, bspFB_, bspFA_, bspPA_, field.layout_);

  vector<Object*> objsA;
  vector<Object*> objsB;

  for (Facility& f : teamA.facilities_) {
    if (f.dead_)
      continue;
    findInRange(bspA_, f, objsA, f.layout_.radius_ * 2);
    findInRange(bspB_, f, objsB, f.layout_.radius_ * 2);

    if (f.teamID_ == 0) {
      for (Object* o : objsA) {
        f.recharge(*static_cast<Ship*>(o));
      }
    } else {
      for (Object* o : objsB) {
        f.recharge(*static_cast<Ship*>(o));
      }
    }
    objsA.clear();
    objsB.clear();
  }

  for (Facility& f : teamB.facilities_) {
    if (f.dead_)
      continue;
    findInRange(bspA_, f, objsA, f.layout_.radius_ * 2);
    findInRange(bspB_, f, objsB, f.layout_.radius_ * 2);

    if (f.teamID_ == 0) {
      for (Object* o : objsA) {
        f.recharge(*static_cast<Ship*>(o));
      }
    } else {
      for (Object* o : objsB) {
        f.recharge(*static_cast<Ship*>(o));
      }
    }
    objsA.clear();
    objsB.clear();
  }
}

void HybridScanner::prepare(BattleField& field) {
  BspScanner::prepare(field);
  ClusterScanner::prepare(field);
}

} /* namespace neurocid */
