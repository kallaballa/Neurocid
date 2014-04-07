/*
 * CustomScenarios.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: elchaschab
 */
#include "declarative_scenarios.hpp"
#include <map>

namespace neurocid {

using std::map;

static map<string, Scenario*> registeredScenarios;

class SymmetricLines : public Scenario {
public:
	SymmetricLines() : Scenario() {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 1500;

		gl_.center_ = {150000,150000};
		gl_.distance_ = 10000;
		gl_.spacing_ = 400;

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

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 20;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = false;
		defenderTL.max_ammo_ = 5;
		defenderTL.max_cooldown = 20;
		teams[1].update(defenderTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}
};

class SymmetricLinesNoMove : public SymmetricLines {
public:
	SymmetricLinesNoMove() : SymmetricLines() {
		gl_.distance_ = 2000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesAttackerMove : public SymmetricLines {
public:
	SymmetricLinesAttackerMove() : SymmetricLines() {
		gl_.distance_ = 10500;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = true;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);

	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesAttackerMoveFacingInward: public SymmetricLinesAttackerMove {
public:
	SymmetricLinesAttackerMoveFacingInward() : SymmetricLinesAttackerMove() {
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, OppositeFacer, Spacer>({}, {M_PI}, {Scenario::gl_});
	}
};

class SymmetricLinesFacingInward : public SymmetricLines {
public:
	SymmetricLinesFacingInward() : SymmetricLines() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		SymmetricLines::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canRotate_ = false;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<IterRot, OppositeFacer, Spacer>({0, M_PI}, {M_PI}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesFacingOutward : public SymmetricLinesFacingInward {
public:
	SymmetricLinesFacingOutward() : SymmetricLinesFacingInward() {
	}

	virtual Placer* createPlacer() {
		return new OppositePlacer<IterRot, OppositeFacer, Spacer>({0, M_PI}, {0}, {Scenario::gl_});
	}
};

class SymmetricLinesFacingOutwardFar : public SymmetricLinesFacingOutward {
public:
	SymmetricLinesFacingOutwardFar() : SymmetricLinesFacingOutward() {
		gl_.distance_ = 20000;
	}
};

class SymmetricLinesFacingInwardFar : public SymmetricLinesFacingInward {
public:
	SymmetricLinesFacingInwardFar() : SymmetricLinesFacingInward() {
		gl_.distance_ = 20000;
	}
};

class SymmetricLinesNoMoveShort : public SymmetricLines {
public:
	SymmetricLinesNoMoveShort() : SymmetricLines() {
		bfl_.iterations_ = 300;
		gl_.spacing_ = 400;
		gl_.distance_ = 6000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		attackerTL.max_cooldown = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesNoMoveTwoRows : public SymmetricLines {
public:
	SymmetricLinesNoMoveTwoRows() : SymmetricLines() {
	}

	virtual Placer* createPlacer() {
		return new OppositePlacerTwoRows<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveTwoRows : public SymmetricLinesNoMoveTwoRows {
public:
	SymmetricLinesAttackerMoveTwoRows() : SymmetricLinesNoMoveTwoRows() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesNoMoveTwoRows::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = true;
		teams[0].update(attackerTL);
	}
};


class SymmetricLinesFar : public SymmetricLines {
public:
	SymmetricLinesFar() : SymmetricLines() {
		bfl_.iterations_ = 4000;
		gl_.distance_ = 30000;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveFar : public SymmetricLinesFar {
public:
	SymmetricLinesAttackerMoveFar() : SymmetricLinesFar() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesFar::configureTeams(teams);
		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.max_damage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class SymmetricLinesAttackerMoveFarFacingInward : public SymmetricLinesAttackerMoveFar {
public:
	SymmetricLinesAttackerMoveFarFacingInward() : SymmetricLinesAttackerMoveFar() {
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesAttackerMoveFar::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		teams[0].update(attackerTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, OppositeFacer, Spacer>({}, {M_PI}, {Scenario::gl_});
	}
};

class SymmetricLinesHuge : public SymmetricLines {
public:
	SymmetricLinesHuge() : SymmetricLines() {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 6000;
		gl_.center_ = {150000,150000};
		gl_.distance_ = 100000;
		gl_.spacing_ = 1000;
		phl_.timeStep_ = 1.0 / 30.0;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.disableProjectileFitness_ = true;
		attackerTL.max_ammo_ = 40;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.disableProjectileFitness_ = true;
		defenderTL.max_ammo_ = 40;
		teams[1].update(defenderTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};
};

class AimOnOne : public Scenario {
public:
	AimOnOne() : Scenario(){
		bfl_.iterations_ = 800;
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;

		gl_.center_ = {150000,150000};
		gl_.distance_ = 10000;
		gl_.spacing_ = 400;

		phl_.gravity_ = {0,0};
		phl_.timeStep_ = 1.0f/60.0f;
		phl_.positionIterations_ = 2;
		phl_.velocityIterations_ = 6;
		phl_.coordToMetersFactor_ = 0.05f;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = true;
		attackerTL.max_ammo_ = 5;
		attackerTL.max_cooldown = 5;

		teams[0].update(attackerTL);

		teams[1].clear();
		Ship c = teams[0][0].clone();
		c.teamID_ = 1;
		teams[1].push_back(c);

		ShipLayout dummyTL = teams[1][0].layout_;
		dummyTL.isDummy_ = true;
		dummyTL.max_damage_ = 100;
		teams[1].update(dummyTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
		assert(pools.size() == 2);
		pools[1] = GeneticPool(); //dummy pool;
	}

	virtual void restorePools(vector<GeneticPool>& pools) {
		pools[1] = pools[0];
	};


	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}

	virtual void restoreTeams(vector<Population>& teams) {
		teams[1].clear();
		//clone teamB from teamA
		for(Ship& t : teams[0]) {
			Ship c = t.clone();
			c.teamID_ = 1;
			teams[1].push_back(c);
		}
	}
};

class AimOnOneNoMove : public AimOnOne {
public:
	AimOnOneNoMove() : AimOnOne() {
		bfl_.iterations_ = 300;
		gl_.distance_ = 6000;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		AimOnOne::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		attackerTL.max_cooldown = 10;

		teams[0].update(attackerTL);
	}
};

class CrossHuge : public SymmetricLines {
public:
	CrossHuge() : SymmetricLines() {
		bfl_.width_ = 300000;
		bfl_.height_ = 300000;
		bfl_.iterations_ = 7000;
		gl_.center_ = {150000,150000};
		gl_.distance_ = 100000;
		gl_.spacing_ = 1000;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.disableProjectileFitness_ = true;
		attackerTL.max_ammo_ = 40;
		attackerTL.max_cooldown = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.disableProjectileFitness_ = true;
		defenderTL.max_ammo_ = 40;
		defenderTL.max_cooldown = 5;
		teams[1].update(defenderTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<RandomRot, RandomFacer, Spacer>({}, {}, {Scenario::gl_});
	}
};

void multiplyTeams(vector<Population>& teams, size_t n) {
	Population newA = teams[0];
	for (Ship& t : teams[0]) {
		for (size_t j = 0; j < n - 1; ++j) {
			newA.push_back(t.clone());
		}
	}
	Population newB = teams[1];
	for (Ship& t : teams[1]) {
		for (size_t j = 0; j < n - 1; ++j) {
			newB.push_back(t.clone());
		}
	}
	teams[0] = newA;
	teams[1] = newB;
}

Scenario* getScenario(const string& name) {
	auto it = registeredScenarios.find(name);
	if (it == registeredScenarios.end())
		return NULL;
	else
		return (*it).second;
}

void loadScenarios() {
	registerScenario("AimOnOneNoMove", new AimOnOneNoMove());
	registerScenario("AimOnOne", new AimOnOne());
	registerScenario("SymmetricLinesNoMoveTwoRows",
			new SymmetricLinesNoMoveTwoRows());
	registerScenario("SymmetricLinesAttackerMoveTwoRows",
			new SymmetricLinesAttackerMoveTwoRows());
	registerScenario("SymmetricLines", new SymmetricLines());
	registerScenario("SymmetricLinesAttackerMove",
			new SymmetricLinesAttackerMove());
	registerScenario("SymmetricLinesNoMove", new SymmetricLinesNoMove());
	registerScenario("SymmetricLinesFar", new SymmetricLinesFar());
	registerScenario("SymmetricLinesAttackerMoveFar",
			new SymmetricLinesAttackerMoveFar());
	registerScenario("SymmetricLinesNoMoveShort",
			new SymmetricLinesNoMoveShort());
	registerScenario("SymmetricLinesHuge", new SymmetricLinesHuge());
	registerScenario("SymmetricLinesFacingInward",
			new SymmetricLinesFacingInward());
	registerScenario("SymmetricLinesFacingInwardFar",
			new SymmetricLinesFacingInwardFar());
	registerScenario("SymmetricLinesFacingOutward",
			new SymmetricLinesFacingOutward());
	registerScenario("SymmetricLinesFacingOutwardFar",
			new SymmetricLinesFacingOutwardFar());
	registerScenario("SymmetricLinesAttackerMoveFarFacingInward",
			new SymmetricLinesAttackerMoveFarFacingInward());
	registerScenario("SymmetricLinesAttackerMoveFacingInward",
			new SymmetricLinesAttackerMoveFacingInward());
	registerScenario("CrossHuge", new CrossHuge());
}

void registerScenario(const string& name, Scenario* s) {
	auto it = registeredScenarios.find(name);
	assert(it == registeredScenarios.end());
	registeredScenarios[name] = s;
}

} /* namespace tankwar */
