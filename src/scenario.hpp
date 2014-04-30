#ifndef SCENARIO_HPP_
#define SCENARIO_HPP_

#include "population.hpp"
#include "genetic.hpp"
#include "battlefieldlayout.hpp"
#include "physics.hpp"
#include "scanner.hpp"
#include <vector>
#include "placer.hpp"

namespace neurocid {
using std::vector;

class Scenario {
public:
	BattleFieldLayout bfl_;
	PlacerLayout sl_;
	PhysicsLayout phl_;
	ScannerLayout scl_;

	Scenario()  {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 1500;

		sl_.center_ = {150000,150000};
		sl_.distance_ = 10000;
		sl_.spacing_ = 100;
		sl_.rotation_ = 0;
		sl_.placer_ = "placer/test";

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/30.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.03f;

		scl_.disableClusterCenters_ = true;
		scl_.numClusters_ = 3;
		scl_.numFriends_ = 10;
		scl_.numEnemies_ = 10;
		scl_.numFriendFacilities_ = 3;
		scl_.numEnemyFacilities_ = 3;
		scl_.numProjectiles_ = 10;
	}

	virtual ~Scenario(){
	};

	virtual void configureTeams(vector<Population>& teams) {};
	virtual void configurePools(vector<GeneticPool>& pools) {};
};

} /* namespace neurocid */

#endif /* SCENARIO_HPP_ */
