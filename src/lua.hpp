/*
 * lua.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: elchaschab
 */

#ifndef LUA_HPP_
#define LUA_HPP_

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "ship.hpp"

namespace neurocid {

using std::string;

namespace lua {

inline void make_vector_2d(lua_State* L, const string& name, const Vector2D& v);
inline void make_projectile_layout(lua_State* L, const ProjectileLayout& pl);
inline void make_projectile(lua_State *L, const Projectile& p, const size_t i);
inline void make_ship_layout(lua_State *L, const ShipLayout& sl);
inline void make_scan_object(lua_State *L, const ScanObject& so, const size_t& i);
inline void make_scan(lua_State *L, const Scan& scan);
inline void make_ship(lua_State *L, const Ship& ship);

double run_fitness_function(const string& script, const Ship& ship);
}
} /* namespace neurocid */

#endif /* LUA_HPP_ */
