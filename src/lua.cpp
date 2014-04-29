/*
 * lua.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: elchaschab
 */

#include "lua.hpp"
#include "util.hpp"
#include <map>

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
		int status;
		int result;
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

	static ScriptLoader* getInstance() {
		if(instance_ == NULL)
			instance_ = new ScriptLoader();

		return instance_;
	}
};

ScriptLoader* ScriptLoader::instance_ = NULL;

void make_vector_2d(lua_State* L, const string& name, const Vector2D& v) {
    lua_newtable(L);
	lua_pushnumber(L, v.x_);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, v.y_);
	lua_setfield(L, -2, "y");
	lua_setfield(L, -2, name.c_str());
}

void make_projectile_layout(lua_State* L, const ProjectileLayout& pl) {
	lua_newtable(L);
	lua_pushnumber(L, pl.maxSpeed_);
	lua_setfield(L, -2, "maxSpeed");
	lua_pushnumber(L, pl.maxTravel_);
	lua_setfield(L, -2, "maxTravel");
	lua_pushnumber(L, pl.radius_);
	lua_setfield(L, -2, "radius");
	lua_setfield(L, -2, "layout");
}

inline void make_projectile(lua_State *L, const Projectile& p, const size_t i) {
	lua_newtable(L);
    make_vector_2d(L, "loc", p.loc_);
    make_vector_2d(L, "startLoc", p.startLoc_);

    make_projectile_layout(L, p.layout_);
    make_scan(L, p.scan_);

    lua_pushinteger(L, i);
	lua_insert(L, -2);
	lua_settable(L, -3);
}

void make_ship_layout(lua_State *L, const ShipLayout& sl) {
    lua_newtable(L);

    lua_pushboolean(L, sl.canMove_);
	lua_setfield(L, -2, "canMove");

    lua_pushboolean(L, sl.canRotate_);
	lua_setfield(L, -2, "canRotate");

    lua_pushboolean(L, sl.canShoot_);
	lua_setfield(L, -2, "canShoot");

    lua_pushboolean(L, sl.isDummy_);
	lua_setfield(L, -2, "isDummy");

	lua_pushboolean(L, sl.disableProjectileFitness_);
	lua_setfield(L, -2, "disableProjectileFitness");

    lua_pushnumber(L, sl.fuelRate_);
	lua_setfield(L, -2, "fuelRate");

    lua_pushnumber(L, sl.hardness_);
	lua_setfield(L, -2, "hardness");

    lua_pushinteger(L, sl.maxAmmo_);
	lua_setfield(L, -2, "maxAmmo");

    lua_pushinteger(L, sl.maxCooldown_);
	lua_setfield(L, -2, "maxCooldown");

	lua_pushinteger(L, sl.maxDamage_);
	lua_setfield(L, -2, "maxDamage");

	lua_pushnumber(L, sl.maxFuel_);
	lua_setfield(L, -2, "maxFuel");

    lua_pushnumber(L, sl.maxRotation_);
	lua_setfield(L, -2, "maxRotation");

    lua_pushnumber(L, sl.maxSpeed_);
	lua_setfield(L, -2, "maxSpeed");

    lua_pushinteger(L, sl.numPerfDesc);
	lua_setfield(L, -2, "numPerfDesc");

	lua_pushnumber(L, sl.radius_);
	lua_setfield(L, -2, "radius");

    lua_pushinteger(L, sl.startAmmo_);
	lua_setfield(L, -2, "startAmmo");

    lua_pushnumber(L, sl.startFuel_);
	lua_setfield(L, -2, "startFuel");

    lua_setfield(L, -2, "layout");
}

void make_scan_object(lua_State *L, const ScanObject& so, const size_t& i) {
    lua_newtable(L);
	make_vector_2d(L, "dir", so.dir_);
	make_vector_2d(L, "loc", so.loc_);
	make_vector_2d(L, "vel", so.vel_);

	lua_pushinteger(L, so.angDist_);
	lua_setfield(L, -2, "angDist");

	lua_pushinteger(L, so.dist_);
	lua_setfield(L, -2, "dist");

	lua_pushinteger(L, so.type_);
	lua_setfield(L, -2, "type");

	lua_pushinteger(L, i);
	lua_insert(L, -2);
	lua_settable(L, -3);
}

void make_scan(lua_State *L, const Scan& scan) {
    lua_newtable(L);
    make_vector_2d(L, "normCenter", scan.normCenter_);
    make_vector_2d(L, "normDir", scan.normDir_);
    make_vector_2d(L, "normVel", scan.normVel_);

    lua_newtable(L);
    size_t soi = 0;
    for(const ScanObject& so : scan.objects_) {
		make_scan_object(L, so, ++soi);
    }

    lua_setfield(L, -2, "objects");
    lua_setfield(L, -2, "scan");
}

void make_ship(lua_State *L, const Ship& ship) {
    lua_newtable(L);

    make_vector_2d(L, "loc", ship.loc_);

    lua_pushnumber(L, ship.failedShots_);
	lua_setfield(L, -2, "failedShots");

    lua_pushnumber(L, ship.hits_);
	lua_setfield(L, -2, "hits");

    lua_pushnumber(L, ship.recharged_);
	lua_setfield(L, -2, "recharged");

    lua_pushnumber(L, ship.fuel_);
	lua_setfield(L, -2, "fuel");

	lua_pushnumber(L, ship.friendlyFire_);
	lua_setfield(L, -2, "friendlyFire");

    lua_pushnumber(L, ship.damage_);
	lua_setfield(L, -2, "damage");

    lua_pushboolean(L, ship.dead_);
	lua_setfield(L, -2, "dead");

    make_ship_layout(L, ship.layout_);
	make_scan(L, ship.scan_);

   lua_newtable(L);
    size_t pi = 0;
    for(const Projectile* p : ship.projectiles_) {
		make_projectile(L, *p, ++pi);
    }
    lua_setfield(L, -2, "projectiles");

    lua_setfield(L, -2, "ship");
}

double run_fitness_function(const string& name, const Ship& ship) {

	lua_State* L = ScriptLoader::getInstance()->load(name);

	/*
	 * Ok, now here we go: We pass data to the lua script on the stack.
	 * That is, we first have to prepare Lua's virtual stack the way we
	 * want the script to receive it, then ask Lua to run it.
	 */
	lua_newtable(L); /* neurocid table */
	make_ship(L, ship);
	lua_setglobal(L, "neurocid"); /* set bottom table as global variable t */

	/* Ask Lua to run our little script */
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		exit(1);
	}

	/* Get the returned value at the top of the stack (index -1) */
	double fitness = lua_tonumber(L, -1);
	lua_pop(L, 1); /* Take the returned value out of the stack */

	return fitness;
}

}

} /* namespace neurocid */
