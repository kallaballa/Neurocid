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
	SpacerLayout gl_;
	PhysicsLayout phl_;
	ScannerLayout scl_;

	Scenario()  {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 1500;

		gl_.center_ = {150000,150000};
		gl_.distance_ = 10000;
		gl_.spacing_ = 100;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;

		scl_.disableClusterCenters = false;
		scl_.numClusters_ = 3;
		scl_.numFriends_ = 20;
		scl_.numEnemies_ = 20;
	}

	virtual void configureTeams(vector<Population>& teams) {};
	virtual void configurePools(vector<GeneticPool>& pools) {};
	virtual void restoreTeams(vector<Population>& teams) {};
	virtual void restorePools(vector<GeneticPool>& pools) {};
	virtual Placer* createPlacer() = 0;
};


} /* namespace neurocid */

#endif /* SCENARIO_HPP_ */
