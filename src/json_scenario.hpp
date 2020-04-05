/*
 * json.hpp
 *
 *  Created on: Apr 15, 2014
 *      Author: elchaschab
 */

#ifndef JSON_HPP_
#define JSON_HPP_

#include "population.hpp"
#include "battlefieldlayout.hpp"
#include "physics.hpp"
#include "scanner.hpp"
#include "placer.hpp"
#include "scenario.hpp"
#include "json_spirit/json_spirit.h"

namespace neurocid {

namespace js = json_spirit;

namespace json {

size_t get_size_t(js::Object& obj, const string& name);
bool get_bool(js::Object& obj, const string& name);
double get_double(js::Object& obj, const string& name);
string get_string(js::Object& obj, const string& name);
Vector2D get_vector2d(js::Object& obj, const string& name);
js::Object get_object(js::Object& obj, const string& name);
void adjust_PhysicsLayout(js::Object& obj, PhysicsLayout& phl);
void adjust_BattleFieldLayout(js::Object& obj, BattleFieldLayout& bfl);
void adjust_ScannerLayout(js::Object& obj, ScannerLayout& scl);
Ship make_ship(js::Object& obj, size_t teamID, PopulationLayout pl);

class JsonPlacer {
	std::vector<Population>& teams_;
public:
	JsonPlacer(std::vector<Population>& teams) : teams_(teams) {
	}

	void place(vector<Population>& teams) {
		assert(teams_[0].size() == teams[0].size());
		assert(teams_[1].size() == teams[1].size());
		for(size_t i = 0; i < teams_[0].size(); ++i) {
			teams[0][i].loc_ = teams_[0][i].loc_;
			teams[0][i].rotation_ = teams_[0][i].rotation_;
		}

		for(size_t i = 0; i < teams_[1].size(); ++i) {
			teams[1][i].loc_ = teams_[1][i].loc_;
			teams[1][i].rotation_ = teams_[1][i].rotation_;
		}
	}
};

class JsonScenario : public Scenario {
public:
	std::vector<Population> teams_;
	std::vector<GeneticPool> pools_;

	JsonScenario(const string& filename);
	virtual ~JsonScenario();
	virtual void configureTeams(vector<Population>& teams) override;
};

}
} /* namespace neurocid */

#endif /* JSON_HPP_ */
