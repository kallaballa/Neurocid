/*
 * renderer.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#include "renderer.hpp"
#include "gamestate.hpp"
#include "canvas.hpp"
#include <thread>
#include <chrono>
#include <stddef.h>
#include <cmath>

namespace tankwar {

Renderer * Renderer::instance_ = NULL;

Renderer::Renderer(size_t frameRate) :
		frameRate_(frameRate),
		enabled_(true) {
}

void Renderer::update(BattleField* field) {
	Canvas::getInstance()->render(field);
}

} /* namespace tankwar */
