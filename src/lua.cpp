/*
 * lua.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: elchaschab
 */

#include "lua.hpp"
#include "util.hpp"
#include "placer.hpp"
#include "population.hpp"
#include "history.hpp"
#include "battlefieldlayout.hpp"
#include "error.hpp"
#include <map>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace neurocid {

namespace lua {

using std::map;

class ScriptLoader {
	static ScriptLoader* instance_;
	lua_State *L_;
	string path_;
	map<string, int> refMap_;
public:
	ScriptLoader() {
		L_ = luaL_newstate();
		luaL_openlibs(L_);

		std::string neurocidPath = get_env("NEUROCID_PATH");

		if(neurocidPath.empty()) {
			std::cerr << "Environment variable $NEUROCID_PATH not set" << std::endl;
			exit(1);
		}

		path_ = neurocidPath + "/lua/";
	}

	~ScriptLoader() {
		for(auto pair : refMap_) {
			luaL_unref(L_, LUA_REGISTRYINDEX, pair.second);
		}
		lua_close(L_);
	}

	lua_State* load(const string& name) {
		string p = path_ + name + ".lua";
		auto it = refMap_.find(p);
		if(it == refMap_.end()) {
			int status = luaL_loadfile(L_, p.c_str());
			if (status) {
				//FIXME exception
				fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L_, -1));
				exit(1);
			}

			//make a copy and get a reference to the registry
			lua_pushvalue(L_,-1);
			int ref = luaL_ref(L_,LUA_REGISTRYINDEX);
			refMap_[p] = ref;
		} else {
			lua_rawgeti(L_,LUA_REGISTRYINDEX, (*it).second);
		}

		return L_;
	}

	void make_field(const string& name, double d) {
		lua_pushnumber(L_, d);
		lua_setfield(L_, -2, name.c_str());
	}

	void make_field(const string& name, size_t s) {
		lua_pushinteger(L_, s);
		lua_setfield(L_, -2, name.c_str());
	}

	void make_field(const string& name, int b) {
		lua_pushinteger(L_, b);
		lua_setfield(L_, -2, name.c_str());
	}

	void make_field(const string& name, bool b) {
		lua_pushboolean(L_, b);
		lua_setfield(L_, -2, name.c_str());
	}

	void make_vector_2d(const string& name, const Vector2D& v) {
	    lua_newtable(L_);
	    make_field("x", v.x_);
	    make_field("y", v.y_);
		lua_setfield(L_, -2, name.c_str());
	}

	void make_projectile_layout(const ProjectileLayout& pl) {
		lua_newtable(L_);
	    make_field("maxSpeed", pl.maxSpeed_);
		make_field("maxTravel", pl.maxTravel_);
		make_field("radius", pl.radius_);
		lua_setfield(L_, -2, "layout");
	}

	void make_projectile(const Projectile& p, const size_t i) {
		lua_newtable(L_);
	    make_vector_2d("loc", p.loc_);
	    make_vector_2d("startLoc", p.startLoc_);

	    make_projectile_layout(p.layout_);
	    make_scan(p.scan_);

	    lua_pushinteger(L_, i);
		lua_insert(L_, -2);
		lua_settable(L_, -3);
	}

	void make_ship_layout(const ShipLayout& sl) {
	    lua_newtable(L_);
	    make_field("canMove", sl.canMove_);
	    make_field("canRotate", sl.canRotate_);
	    make_field("canShoot", sl.canShoot_);
	    make_field("isDummy", sl.isDummy_);
		make_field("disableProjectileFitness", sl.disableProjectileFitness_);
	    make_field("fuelRate", sl.fuelRate_);
	    make_field("hardness", sl.hardness_);
	    make_field("maxAmmo", sl.maxAmmo_);
	    make_field("maxCooldown", sl.maxCooldown_);
		make_field("maxDamage", sl.maxDamage_);
		make_field("maxFuel", sl.maxFuel_);
	    make_field("maxRotation", sl.maxRotation_);
	    make_field("maxSpeed", sl.maxSpeed_);
	    make_field("numPerfDesc", sl.numPerfDesc_);
		make_field("radius", sl.radius_);
	    make_field("startAmmo", sl.startAmmo_);
	    make_field("startFuel", sl.startFuel_);
	    lua_setfield(L_, -2, "layout");
	}

	void make_history(const History& h) {
	    lua_newtable(L_);
	    make_vector_2d("avgLoc", h.avgLoc_);
	    make_vector_2d("avgVel", h.avgVel_);
	    lua_setfield(L_, -2, "history");
	}

	void make_scan_object(const ScanObject& so, const size_t& i) {
	    lua_newtable(L_);
		make_vector_2d("dir", so.dir_);
		make_vector_2d("loc", so.loc_);
		make_vector_2d("vel", so.vel_);

		make_field("angDist", so.angDist_);
		make_field("dist", so.dist_);
		make_field("type", so.type_);

		lua_pushinteger(L_, i);
		lua_insert(L_, -2);
		lua_settable(L_, -3);
	}

	void make_scan(const Scan& scan) {
	    lua_newtable(L_);
	    make_vector_2d("normCenter", scan.normCenter_);
	    make_vector_2d("normDir", scan.normDir_);
	    make_vector_2d("normVel", scan.normVel_);

	    lua_newtable(L_);
	    size_t soi = 0;
	    for(const ScanObject& so : scan.objects_) {
			make_scan_object(so, ++soi);
	    }

	    lua_setfield(L_, -2, "objects");
	    lua_setfield(L_, -2, "scan");
	}

	void make_ship_fields(const Ship& ship) {
	    lua_newtable(L_);

	    make_vector_2d("loc", ship.loc_);
	    make_vector_2d("vel", ship.vel_);
	    make_field("failedShots", ship.failedShots_);
	    make_field("hits", ship.hits_);
	    make_field("recharged", ship.recharged_);
	    make_field("fuel", ship.fuel_);
		make_field("friendlyFire", ship.friendlyFire_);
	    make_field("damage", ship.damage_);
	    make_field("dead", ship.dead_);
	    make_field("captured", ship.captured_);
	    make_field("kills", ship.kills_);

	    make_ship_layout(ship.layout_);
	    make_history(ship.history_);
	    make_scan(ship.scan_);

		lua_newtable(L_);
	    size_t pi = 0;
	    for(const Projectile* p : ship.projectiles_) {
			make_projectile(*p, ++pi);
	    }
	}

	void make_ship(const Ship& ship) {
		make_ship_fields(ship);
	    lua_setfield(L_, -2, "projectiles");
	    lua_setfield(L_, -2, "ship");
	}

	void make_ship(const Ship& ship, const size_t& i) {
		make_ship_fields(ship);
	    lua_setfield(L_, -2, "projectiles");
		lua_pushinteger(L_, i);
		lua_insert(L_, -2);
		lua_settable(L_, -3);
	}


	void make_facility_layout(const FacilityLayout& fl) {
		lua_newtable(L_);
	    make_field("maxCooldown", fl.maxCooldown_);
		make_field("radius", fl.radius_);
		lua_setfield(L_, -2, "layout");
	}

	void make_facility(const Facility& f, const size_t i) {
		lua_newtable(L_);

		make_facility_layout(f.layout_);
		make_vector_2d("loc", f.loc_);

	    lua_pushinteger(L_, i);
		lua_insert(L_, -2);
		lua_settable(L_, -3);
	}

	void make_population(const string& name, const Population& team) {
		lua_newtable(L_);

		lua_newtable(L_);
		size_t si = 0;
	    for(const Ship& ship : team) {
			make_ship(ship, ++si);
	    }
	    lua_setfield(L_, -2, "ships");

	    lua_newtable(L_);
		size_t fi = 0;
		for(const Facility& f : team.facilities_) {
			make_facility(f, ++fi);
		}
	    lua_setfield(L_, -2, "facilities");

	    lua_setfield(L_, -2, name.c_str());
	}

	void make_placer_layout(const PlacerLayout& pl) {
		lua_newtable(L_);
	    make_vector_2d("center", pl.center_);
		make_field("distance", pl.distance_);
		make_field("spacing", pl.spacing_);
		make_field("rotation", pl.rotation_);
		make_field("fuzz", pl.fuzz_);

		lua_setfield(L_, -2, "pl");
	}

	void make_battlefield_layout(const BattleFieldLayout& bfl) {
		lua_newtable(L_);
		make_field("width", bfl.width_);
		make_field("height", bfl.height_);
		make_field("iterations", bfl.iterations_);
		lua_setfield(L_, -2, "bfl");
	}


	void copy_locations(const string& name, Population& team) {
		luaL_checktype(L_, -1, LUA_TTABLE);
		lua_pushstring(L_, name.c_str());
		lua_gettable(L_, -2);
		luaL_checktype(L_, -1, LUA_TTABLE);

		lua_pushstring(L_, "ships");
		lua_gettable(L_, -2);
		luaL_checktype(L_, -1, LUA_TTABLE);

		size_t s = lua_objlen(L_, -1);

		CHECK_MSG(s == team.size(), "Only loc and rotation of ships and facilities may be changed");

		for(size_t i = 1; i <= s; ++i) {
			lua_rawgeti(L_, -1, i);
			luaL_checktype(L_, -1, LUA_TTABLE);

			lua_getfield(L_, -1, "rotation");
			team[i-1].rotation_ = lua_tonumber(L_, -1);
			CHECK_MSG(team[i-1].rotation_ <= M_PI && team[i-1].rotation_ >= -M_PI, "You need to normalize rotations to -pi and pi");
			lua_pop(L_, 1);

			lua_pushstring(L_, "loc");
			lua_gettable(L_, -2);

			luaL_checktype(L_, -1, LUA_TTABLE);

			lua_getfield(L_, -1, "x");
			team[i-1].loc_.x_ = lua_tonumber(L_, -1);
			lua_pop(L_, 1);

			lua_getfield(L_, -1, "y");
			team[i-1].loc_.y_ = lua_tonumber(L_, -1);
			lua_pop(L_, 3);
		}

		lua_pop(L_, 1);

		lua_pushstring(L_, "facilities");
		lua_gettable(L_, -2);
		luaL_checktype(L_, -1, LUA_TTABLE);

		s = lua_objlen(L_, -1);

		CHECK_MSG(s == team.facilities_.size(), "Only loc and rotation of ships and facilities may be changed");

		for(size_t i = 1; i <= s; ++i) {
			lua_rawgeti(L_, -1, i);
			luaL_checktype(L_, -1, LUA_TTABLE);

			lua_pushstring(L_, "loc");
			lua_gettable(L_, -2);

			luaL_checktype(L_, -1, LUA_TTABLE);

			lua_getfield(L_, -1, "x");
			team.facilities_[i-1].loc_.x_ = lua_tonumber(L_, -1);
			lua_pop(L_, 1);

			lua_getfield(L_, -1, "y");
			team.facilities_[i-1].loc_.y_ = lua_tonumber(L_, -1);
			lua_pop(L_, 3);
		}
		lua_pop(L_, 2);
	}

	static ScriptLoader* getInstance() {
		if(instance_ == NULL)
			instance_ = new ScriptLoader();

		return instance_;
	}
};

ScriptLoader* ScriptLoader::instance_ = NULL;

double run_fitness_function(const string& name, const Ship& ship, const BattleFieldLayout& bfl) {
	ScriptLoader* loader = ScriptLoader::getInstance();
	lua_State* L = loader->load(name);

	lua_newtable(L); /* neurocid table */
	loader->make_battlefield_layout(bfl);
	loader->make_ship(ship);
	lua_setglobal(L, "nc");

	/* Ask Lua to run our little script */
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "Failed to run script (%s): %s\n", name.c_str(), lua_tostring(L, -1));
		exit(1);
	}

	/* Get the returned value at the top of the stack (index -1) */
	double fitness = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return fitness;
}

void run_placer(const string& name, vector<Population>& teams, const PlacerLayout& pl, const size_t& tick) {
	ScriptLoader* loader = ScriptLoader::getInstance();
	lua_State* L = loader->load(name);

	lua_newtable(L); /* neurocid table */
	loader->make_field("tick", tick);
	loader->make_placer_layout(pl);
	loader->make_population("teamA", teams[0]);
	loader->make_population("teamB", teams[1]);
 	lua_setglobal(L, "nc");

	/* Ask Lua to run our little script */
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "Failed to run script (%s): %s\n", name.c_str(), lua_tostring(L, -1));
		exit(1);
	}

	loader->copy_locations("teamA", teams[0]);
	loader->copy_locations("teamB", teams[1]);
	lua_pop(L, 1);
}
}

} /* namespace neurocid */
