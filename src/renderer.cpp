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
		enabled_(true),
		field_(NULL),
		updateMutex()  {
}

void Renderer::update(BattleField* field) {
	updateMutex.lock();
	field_ = field;
	updateMutex.unlock();
}

void Renderer::render() {
		if(isEnabled()) {
			updateMutex.lock();
			if(field_ != NULL)
				Canvas::getInstance()->render(*field_);
			updateMutex.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds((size_t)round(1000 / frameRate_)));
}
} /* namespace tankwar */
