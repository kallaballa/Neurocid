#include "procedural_scenarios.hpp"
#include "error.hpp"
#include <map>

namespace neurocid {

using std::map;

static map<string, Scenario*> registeredScenarios;

class SymmetricLines : public Scenario {
public:
	SymmetricLines() : Scenario() {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 5000;

		pl_.center_ = {300000,300000};
		pl_.distance_ = 20000;
		pl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = true;
		attackerTL.maxCooldown_ = 5;
		attackerTL.startFuel_ = 1000;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = false;
		defenderTL.canMove_ = true;
		defenderTL.maxCooldown_ = 5;
		defenderTL.startFuel_ = 1000;
		teams[1].update(defenderTL);

		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
//		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
//		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));

//		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
//		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
	}
};

class SymmetricLinesNoMove : public SymmetricLines {
public:
	SymmetricLinesNoMove() : SymmetricLines() {
		pl_.distance_ = 2000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMove : public SymmetricLines {
public:
	SymmetricLinesAttackerMove() : SymmetricLines() {
		pl_.distance_ = 15000;
		bfl_.iterations_ = 4000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 1000;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		defenderTL.radius_ = 200;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveLong : public SymmetricLinesAttackerMove {
public:
	SymmetricLinesAttackerMoveLong() : SymmetricLinesAttackerMove() {
		bfl_.iterations_ = 15000;
		pl_.distance_ = 120000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesAttackerMove::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 2000;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};


class SymmetricLinesNoMoveShort : public SymmetricLines {
public:
	SymmetricLinesNoMoveShort() : SymmetricLines() {
		bfl_.iterations_ = 300;
		pl_.spacing_ = 400;
		pl_.distance_ = 6000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		attackerTL.maxCooldown_ = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesFar : public SymmetricLines {
public:
	SymmetricLinesFar() : SymmetricLines() {
		bfl_.iterations_ = 2500;
		pl_.distance_ = 80000;
		pl_.spacing_ = 400;
	}
};

class SymmetricLinesFarLong : public SymmetricLinesFar {
public:
	SymmetricLinesFarLong() : SymmetricLinesFar() {
		bfl_.iterations_ = 15000;
		pl_.distance_ = 120000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesFar::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 2000;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.startFuel_ = 2000;
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
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesHuge : public SymmetricLines {
public:
	SymmetricLinesHuge() : SymmetricLines() {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 100000;
		pl_.center_ = {300000,300000};
		pl_.distance_ = 200000;
		pl_.spacing_ = 1000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);
		teams[0].facilities_[0].layout_.maxDamage_ = 2000;
		teams[1].facilities_[0].layout_.maxDamage_ = 2000;
    //teams[0].facilities_[0].layout_.maxRecharge_ = 10000000;
    //teams[1].facilities_[0].layout_.maxRecharge_ = 10000000;


		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 3000;
		attackerTL.maxFuel_ = 20000;
		attackerTL.maxAmmo_ = 20;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.startFuel_ = 3000;
		defenderTL.maxFuel_ = 20000;
		defenderTL.maxAmmo_ = 20;
		teams[1].update(defenderTL);
		multiplyTeams(teams,3);
/*		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
*/	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};
};

class AimOnOne : public Scenario {
public:
	AimOnOne() : Scenario(){
		bfl_.iterations_ = 400;
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;

		pl_.center_ = {300000,300000};
		pl_.distance_ = 10000;
		pl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.canMove_ = true;
		attackerTL.maxCooldown_ = 5;

		teams[0].update(attackerTL);

		teams[1].clear();
		Ship c = teams[0][0].clone();
		c.teamID_ = 1;
		teams[1].push_back(c);

		ShipLayout dummyTL = teams[1][0].layout_;
		dummyTL.isDummy_ = true;
		dummyTL.radius_ = 500;
		dummyTL.maxDamage_ = 100;
		teams[1].update(dummyTL);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
		pools[1] = GeneticPool(); //dummy pool;
	}
};

class AimOnOneNoMove : public AimOnOne {
public:
	AimOnOneNoMove() : AimOnOne() {
		bfl_.iterations_ = 150;
		pl_.distance_ = 6000;
		pl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		AimOnOne::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		attackerTL.startAmmo_ = 5;
		attackerTL.maxCooldown_ = 10;

		teams[0].update(attackerTL);
	}
};

class CrossHuge : public SymmetricLines {
public:
	CrossHuge() : SymmetricLines() {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 3500;
		pl_.center_ = {300000,300000};
		pl_.distance_ = 100000;
		pl_.spacing_ = 1000;
	}

	void configureTeams(vector<Population>& teams) {
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.maxCooldown_ = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.maxCooldown_ = 5;
		teams[1].update(defenderTL);
		multiplyTeams(teams,5);
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};
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

Scenario* get_procedural_scenario(const string& name) {
	auto it = registeredScenarios.find(name);
	if (it == registeredScenarios.end())
		return NULL;
	else
		return (*it).second;
}

void load_procedural_scenarios() {
	registerScenario("AimOnOneNoMove", new AimOnOneNoMove());
	registerScenario("AimOnOne", new AimOnOne());
	registerScenario("SymmetricLines", new SymmetricLines());
	registerScenario("SymmetricLinesAttackerMove", new SymmetricLinesAttackerMove());
	registerScenario("SymmetricLinesAttackerMoveLong",new SymmetricLinesAttackerMoveLong());
	registerScenario("SymmetricLinesNoMove", new SymmetricLinesNoMove());
	registerScenario("SymmetricLinesFar", new SymmetricLinesFar());
	registerScenario("SymmetricLinesFarLong", new SymmetricLinesFarLong());
	registerScenario("SymmetricLinesAttackerMoveFar", new SymmetricLinesAttackerMoveFar());
	registerScenario("SymmetricLinesNoMoveShort", new SymmetricLinesNoMoveShort());
	registerScenario("SymmetricLinesHuge", new SymmetricLinesHuge());
	registerScenario("CrossHuge", new CrossHuge());
}

void registerScenario(const string& name, Scenario* s) {
	auto it = registeredScenarios.find(name);
	if(it != registeredScenarios.end()) {
		error("Attempt to register an existing scenario.");
	}
	registeredScenarios[name] = s;
}

} /* namespace tankwar */
