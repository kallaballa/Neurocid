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
	PlacerLayout pl_;
	PhysicsLayout phl_;
	ScannerLayout scl_;

	Scenario()  {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 1500;

		pl_.center_ = {150000,150000};
		pl_.distance_ = 10000;
		pl_.spacing_ = 100;
		pl_.rotation_ = 0;
		pl_.fuzz_ = 4;
		pl_.placer_ = "placer/opposite";

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/10.0f;
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

	virtual void configureTeams(vector<Population>& teams) {
		unused(teams);
	};
	virtual void configurePools(vector<GeneticPool>& pools) {
		unused(pools);
	};
};

} /* namespace neurocid */

#endif /* SCENARIO_HPP_ */
