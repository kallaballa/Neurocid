/*
 * json.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: elchaschab
 */

#ifndef _NO_JSON
#include "json_scenario.hpp"
#endif
#include "error.hpp"

namespace neurocid {

namespace json {

#ifndef _NO_JSON
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

string get_string(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name)
			  return p.value_.get_str();
	  }

	  std::cerr << "Property not found: " + name;
	  exit(1);
	  return 0;
}

Vector2D get_vector2d(js::Object& obj, const string& name) {
	  for(js::Pair& p : obj) {
		  if(p.name_ == name) {
			  js::Array& arr = p.value_.get_array();
			  CHECK_MSG(arr.size() == 2, "Invalid vector2d");
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
	Ship s(teamID, pl.sl_);
	s.loc_ = get_vector2d(obj,"loc");
	s.rotation_ = normRotation(get_double(obj, "rotation"));
	s.layout_.isDummy_ = get_bool(obj, "isDummy");
	s.layout_.canShoot_ = get_bool(obj, "canShoot");
	s.layout_.canRotate_ = get_bool(obj, "canRotate");
	s.layout_.canMove_ = get_bool(obj, "canMove");
	s.layout_.disableProjectileFitness_ = get_bool(obj, "disableProjectileFitness");

	s.layout_.radius_ = get_double(obj, "radius");
	s.layout_.maxSpeed_ = get_double(obj, "maxSpeed");
	s.layout_.maxRotation_ = get_double(obj, "maxRotation");
	s.layout_.maxFuel_ = get_double(obj, "maxFuel");
	s.layout_.startFuel_ = get_double(obj, "startFuel");
	s.layout_.fuelRate_ = get_double(obj, "fuelRate");
	s.layout_.hardness_ = get_double(obj, "hardness");

	s.layout_.maxAmmo_ = get_size_t(obj, "maxAmmo");
	s.layout_.startAmmo_ = get_size_t(obj, "startAmmo");
	s.layout_.maxCooldown_ = get_size_t(obj, "maxCooldown");
	s.layout_.maxDamage_ = get_size_t(obj, "maxDamage");
	s.layout_.crashesPerDamage_ = get_size_t(obj, "crashesPerDamage");
	s.layout_.numPerfDesc_ = get_size_t(obj, "numPerfDesc");
	s.layout_.fitnessFunction_ = get_string(obj, "fitnessFunction");

	return s;
}

Facility make_facility(js::Object& obj, size_t teamID, FacilityLayout fl) {
	Facility f(teamID, fl, Vector2D());
	f.loc_ = get_vector2d(obj,"loc");
	f.rotation_ = normRotation(get_double(obj, "rotation"));
	f.layout_.maxCooldown_ = get_size_t(obj, "maxCooldown");
	f.layout_.radius_ = get_size_t(obj, "radius");

	return f;
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
	scl.numFriendFacilities_ = get_size_t(obj, "numFriendFacilities");
	scl.numEnemyFacilities_ = get_size_t(obj, "numEnemyFacilities");
	scl.numFriends_ = get_size_t(obj, "numFriends");
	scl.numEnemies_ = get_size_t(obj, "numEnemies");
	scl.numProjectiles_ = get_size_t(obj, "numProjectiles");
}

void adjust_PlacerLayout(js::Object& obj, PlacerLayout& pl) {
	pl.center_ = get_vector2d(obj, "center");
	pl.distance_ = get_double(obj, "distance");
	pl.placer_ = get_string(obj, "placer");
	pl.rotation_ = get_double(obj, "rotation");
	pl.spacing_ = get_double(obj, "spacing");
	pl.fuzz_ = get_double(obj, "fuzz");
}

JsonScenario::JsonScenario(const string& filename) :
		Scenario(), teams_(2) {
	ifstream is(filename);
	js::Value rootValue;
	js::read(is, rootValue);

	js::Object bflObj = get_object(rootValue.get_obj(), "BattleFieldLayout");
	js::Object phlObj = get_object(rootValue.get_obj(), "PhysicsLayout");
	js::Object sclObj = get_object(rootValue.get_obj(), "ScannerLayout");
	js::Object plObj = get_object(rootValue.get_obj(), "PlacerLayout");

	js::Array teamAObj = get_array(rootValue.get_obj(), "TeamA");
	js::Array teamBObj = get_array(rootValue.get_obj(), "TeamB");
	js::Array facAObj = get_array(rootValue.get_obj(), "FacilitiesA");
	js::Array facBObj = get_array(rootValue.get_obj(), "FacilitiesB");

	PopulationLayout pl = make_default_population_layout();
	GeneticLayout gl = make_default_genetic_layout();
	adjust_BattleFieldLayout(bflObj, bfl_);
	adjust_PhysicsLayout(phlObj, phl_);
	adjust_ScannerLayout(sclObj, scl_);
	adjust_PlacerLayout(plObj, pl_);

	for (js::Value& v : teamAObj) {
		teams_[0].push_back(make_ship(v.get_obj(), 0, pl));
	}

	for (js::Value& v : teamBObj) {
		teams_[1].push_back(make_ship(v.get_obj(), 1, pl));
	}

	for (js::Value& v : facAObj) {
		teams_[0].facilities_.push_back(make_facility(v.get_obj(), 0, pl.fl_));
		std::cerr << "facilityA" << std::endl;
	}

	for (js::Value& v : facBObj) {
		teams_[1].facilities_.push_back(make_facility(v.get_obj(), 1, pl.fl_));
		std::cerr << "facilityB" << std::endl;
	}

	pools_ = make_pools(2, gl);
}

JsonScenario::~JsonScenario() {
}

void JsonScenario::configureTeams(vector<Population>& teams) {
	scale_population(teams[0], teams_[0].size());
	scale_population(teams[1], teams_[1].size());

	teams[0].facilities_ = teams_[0].facilities_;
	teams[1].facilities_ = teams_[1].facilities_;

	for(size_t i = 0; i < teams_[0].size(); ++i) {
		teams[0][i].layout_ = teams_[0][i].layout_;
	}

	for(size_t i = 0; i < teams_[1].size(); ++i) {
		teams[1][i].layout_ = teams_[1][i].layout_;
	}

	JsonPlacer(teams_).place(teams);
}
#endif
}
}
