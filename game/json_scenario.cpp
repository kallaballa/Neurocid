/*
 * json.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: elchaschab
 */

#include "json_scenario.hpp"

namespace neurocid {

namespace json {

size_t get_size_t(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name)
			  return p.value_.get_uint64();
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return 0;
}

bool get_bool(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name)
			  return p.value_.get_bool();
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return 0;
}

double get_double(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name)
			  return p.value_.get_real();
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return 0;
}

Vector2D get_vector2d(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name) {
			  js::Array& arr = p.value_.get_array();
			  assert(arr.size() == 2);
			  return Vector2D(arr[0].get_real(), arr[1].get_real());
		  }
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return 0;
}

js::Object get_object(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name)
			  return p.value_.get_obj();
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return js::Object();
}

js::Array get_array(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name)
			  return p.value_.get_array();
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return js::Array();
}

Ship make_ship(js::Object& obj, size_t teamID, PopulationLayout pl) {
	Ship s(teamID, pl.tl_);
	s.loc_ = get_vector2d(obj,"loc");
	s.rotation_ = normRotation(get_double(obj, "rotation"));
	s.layout_.isDummy_ = get_bool(obj, "ShipIsDummy");
	s.layout_.canShoot_ = get_bool(obj, "ShipCanShoot");
	s.layout_.canRotate_ = get_bool(obj, "ShipCanRotate");
	s.layout_.canMove_ = get_bool(obj, "ShipCanMove");
	s.layout_.disableProjectileFitness_ = get_bool(obj,
			"ShipDisableProjectileFitness");
	s.layout_.radius_ = get_double(obj, "ShipRange");
	s.layout_.maxSpeed_ = get_double(obj, "ShipMaxSpeed");
	s.layout_.maxRotation_ = get_double(obj, "ShipMaxRotation");
	s.layout_.maxFuel_ = get_double(obj, "ShipMaxFuel");
	s.layout_.fuelRate_ = get_double(obj, "ShipFuelRate");
	s.layout_.maxCooldown_ = get_size_t(obj, "ShipMaxCooldown");
	s.layout_.maxAmmo_ = get_size_t(obj, "ShipMaxAmmo");
	s.layout_.maxDamage_ = get_size_t(obj, "ShipMaxDamage");
	s.layout_.crashesPerDamage_ = get_size_t(obj, "ShipCrashesPerDamage");
	s.layout_.numPerfDesc_ = get_size_t(obj, "ShipNumPerfDesc");
	return s;
}

void adjust_PhysicsLayout(js::Object& obj, PhysicsLayout& phl) {
	Vector2D gv = get_vector2d(obj, "gravity");
	phl.gravity_ = b2Vec2(gv.x_, gv.y_);
	phl.timeStep_ = get_double(obj, "timeStep");
	phl.positionIterations_ = get_size_t(obj, "positionIterations");
	phl.velocityIterations_ = get_size_t(obj, "velocityIterations");
	phl.coordToMetersFactor_ = get_double(obj, "coordToMetersFactor");
}

void adjust_BattleFieldLayout(js::Object& obj, BattleFieldLayout& bfl) {
	bfl.width_ = get_size_t(obj, "width");
	bfl.height_ = get_size_t(obj, "height");
	bfl.iterations_ = get_size_t(obj, "iterations");
}

void adjust_ScannerLayout(js::Object& obj, ScannerLayout& scl) {
	scl.disableClusterCenters_ = get_bool(obj, "disableClusterCenters");
	scl.numClusters_ = get_size_t(obj, "numClusters");
	scl.numFriends_ = get_size_t(obj, "numFriends");
	scl.numEnemies_ = get_size_t(obj, "numEnemies");
	scl.numProjectiles_ = get_size_t(obj, "numProjectiles");
}

JsonScenario::JsonScenario(const string& filename) :
		Scenario(), teams_(2) {
	ifstream is(filename);
	js::Value rootValue;
	js::read(is, rootValue);
	js::Object bflObj = get_object(rootValue.get_obj(), "BattleFieldLayout");
	js::Object phlObj = get_object(rootValue.get_obj(), "PhysicsLayout");
	js::Object sclObj = get_object(rootValue.get_obj(), "ScannerLayout");
	js::Array teamAObj = get_array(rootValue.get_obj(), "TeamA");
	js::Array teamBObj = get_array(rootValue.get_obj(), "TeamB");
	PopulationLayout pl = make_default_population_layout();
	GeneticLayout gl = make_default_genetic_layout();
	adjust_BattleFieldLayout(bflObj, bfl_);
	adjust_PhysicsLayout(phlObj, phl_);
	adjust_ScannerLayout(sclObj, scl_);

	for (js::Value& v : teamAObj) {
		teams_[0].push_back(make_ship(v.get_obj(), 0, pl));
	}
	for (js::Value& v : teamBObj) {
		teams_[1].push_back(make_ship(v.get_obj(), 1, pl));
	}

	pools_ = make_pools(2, gl);
}

JsonScenario::~JsonScenario() {
}

void JsonScenario::configureTeams(vector<Population>& teams) {
	scale_population(teams[0], teams_[0].size());
	scale_population(teams[1], teams_[1].size());
	assert(teams_[0].size() == teams[0].size());
	assert(teams_[1].size() == teams[1].size());
	for(size_t i = 0; i < teams_[0].size(); ++i) {
		teams[0][i].layout_ = teams_[0][i].layout_;
	}

	for(size_t i = 0; i < teams_[1].size(); ++i) {
		teams[1][i].layout_ = teams_[1][i].layout_;
	}

	JsonPlacer(teams_).place(teams);
}

}
}
