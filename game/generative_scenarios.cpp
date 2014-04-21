#include "generative_scenarios.hpp"
#include <map>

namespace neurocid {

using std::map;

static map<string, Scenario*> registeredScenarios;

class SymmetricLines : public Scenario {
public:
	SymmetricLines() : Scenario() {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 750;

		gl_.center_ = {300000,300000};
		gl_.distance_ = 10000;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.isDummy_ = false;
		attackerTL.maxCooldown_ = 5;
		attackerTL.startFuel_ = 500;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = false;
		defenderTL.maxCooldown_ = 5;
		defenderTL.startFuel_ = 500;
		teams[1].update(defenderTL);

		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<IterRot, OppositeFacer, Spacer>({0,0}, {0}, {Scenario::gl_});
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
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMove : public SymmetricLines {
public:
	SymmetricLinesAttackerMove() : SymmetricLines() {
		gl_.distance_ = 15000;
		bfl_.iterations_ = 4000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 450;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveLong : public SymmetricLinesAttackerMove {
public:
	SymmetricLinesAttackerMoveLong() : SymmetricLinesAttackerMove() {
		bfl_.iterations_ = 15000;
		gl_.distance_ = 120000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 800;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesAttackerMoveFacingInward: public SymmetricLinesAttackerMove {
public:
	SymmetricLinesAttackerMoveFacingInward() : SymmetricLinesAttackerMove() {
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<IterRot, OppositeFacer, Spacer>({0,0}, {M_PI}, {Scenario::gl_});
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
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<IterRot, OppositeFacer, Spacer>({0, M_PI}, {M_PI}, {Scenario::gl_});
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
		attackerTL.maxCooldown_ = 5;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
		teams[1].update(defenderTL);
	}
};

class SymmetricLinesNoMoveTwoRows : public SymmetricLines {
public:
	SymmetricLinesNoMoveTwoRows() : SymmetricLines() {
	}

	virtual Placer* createPlacer() {
		return new OppositePlacerTwoRows<IterRot, OppositeFacer, Spacer>({0,0}, {0}, {Scenario::gl_});
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.isDummy_ = true;
		defenderTL.maxDamage_ = 100;
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
		bfl_.iterations_ = 2500;
		gl_.distance_ = 80000;
		gl_.spacing_ = 400;
	}
};

class SymmetricLinesFarLong : public SymmetricLinesFar {
public:
	SymmetricLinesFarLong() : SymmetricLinesFar() {
		bfl_.iterations_ = 15000;
		gl_.distance_ = 120000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLinesFar::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 800;
		teams[0].update(attackerTL);
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
		return new FuzzyOppositePlacer<IterRot, OppositeFacer, Spacer>({0,0}, {M_PI}, {Scenario::gl_});
	}
};

class SymmetricLinesHuge : public SymmetricLines {
public:
	SymmetricLinesHuge() : SymmetricLines() {
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;
		bfl_.iterations_ = 6000;
		gl_.center_ = {300000,300000};
		gl_.distance_ = 200000;
		gl_.spacing_ = 1000;
	}

	virtual void configureTeams(vector<Population>& teams) {
		SymmetricLines::configureTeams(teams);

		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.startFuel_ = 2000;
		attackerTL.maxFuel_ = 30000;
		attackerTL.maxAmmo_ = 40;
		teams[0].update(attackerTL);

		ShipLayout defenderTL = teams[1][0].layout_;
		defenderTL.startFuel_ = 2000;
		defenderTL.maxFuel_ = 30000;
		defenderTL.maxAmmo_ = 40;
		teams[1].update(defenderTL);
		multiplyTeams(teams,5);
		teams[0].facilities_.push_back(Facility(0, teams[0].layout_.fl_, {0,0}));
		teams[1].facilities_.push_back(Facility(1, teams[1].layout_.fl_, {0,0}));
	}

	virtual void configurePools(vector<GeneticPool>& pools) {
	};
};

class AimOnOne : public Scenario {
public:
	AimOnOne() : Scenario(){
		bfl_.iterations_ = 400;
		bfl_.width_ = 600000;
		bfl_.height_ = 600000;

		gl_.center_ = {300000,300000};
		gl_.distance_ = 10000;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

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
		assert(pools.size() == 2);
		pools[1] = GeneticPool(); //dummy pool;
	}

	virtual Placer* createPlacer() {
		return new FuzzyOppositePlacer<IterRot, RandomFacer, Spacer>({0,0}, {}, {Scenario::gl_});
	}
};

class AimOnOneNoMove : public AimOnOne {
public:
	AimOnOneNoMove() : AimOnOne() {
		bfl_.iterations_ = 150;
		gl_.distance_ = 6000;
		gl_.spacing_ = 400;
	}

	virtual void configureTeams(vector<Population>& teams) {
		AimOnOne::configureTeams(teams);
		ShipLayout attackerTL = teams[0][0].layout_;
		attackerTL.canMove_ = false;
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
		gl_.center_ = {300000,300000};
		gl_.distance_ = 100000;
		gl_.spacing_ = 1000;
	}

	void configureTeams(vector<Population>& teams) {
		assert(teams.size() == 2);

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

	Placer* createPlacer() {
		return new CrossPlacerTwoRows<IterRot, OppositeFacer, Spacer>({0,0}, {0}, {Scenario::gl_});
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

Scenario* get_declarative_scenario(const string& name) {
	auto it = registeredScenarios.find(name);
	if (it == registeredScenarios.end())
		return NULL;
	else
		return (*it).second;
}

void load_delarative_scenarios() {
	registerScenario("AimOnOneNoMove", new AimOnOneNoMove());
	registerScenario("AimOnOne", new AimOnOne());
	registerScenario("SymmetricLinesNoMoveTwoRows",
			new SymmetricLinesNoMoveTwoRows());
	registerScenario("SymmetricLinesAttackerMoveTwoRows",
			new SymmetricLinesAttackerMoveTwoRows());
	registerScenario("SymmetricLines", new SymmetricLines());
	registerScenario("SymmetricLinesAttackerMove",
			new SymmetricLinesAttackerMove());
	registerScenario("SymmetricLinesAttackerMoveLong",
			new SymmetricLinesAttackerMoveLong());
	registerScenario("SymmetricLinesNoMove", new SymmetricLinesNoMove());
	registerScenario("SymmetricLinesFar", new SymmetricLinesFar());
	registerScenario("SymmetricLinesFarLong", new SymmetricLinesFarLong());
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
