/*
 * background.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: elchaschab
 */

#include "background.hpp"
#include "util.hpp"
#include "canvas.hpp"
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL.h>

namespace neurocid {


Background::Background(Canvas* canvas) :
		canvas_(canvas) {
}

void Background::init() {
	for(size_t i = 0; i < 1200; i++) {
		Star s;
		s.x = fRand(-2400000,2400000);
		s.y = fRand(-2400000,2400000);
		s.radius = fRand(5,10);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(100,255);
		s.step = 0.7;
		s.discontinuity = std::max(1.0, round(s.radius / 8));
		s.scale = 0.6;
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 600; i++) {
		Star s;
		s.x = fRand(-1200000,1200000);
		s.y = fRand(-1200000,1200000);
		s.radius = fRand(5,8);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(100,255);
		s.step = 0.7;
		s.discontinuity = std::max(1.0, round(s.radius / 8));
		s.scale = 0.4;
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 300; i++) {
		Star s;
		s.x = fRand(-600000,600000);
		s.y = fRand(-600000,600000);
		s.radius = fRand(5,6);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(100,255);
		s.step = 0.7;
		s.discontinuity = std::max(1.0, round(s.radius / 8));
		s.scale = 0.2;
		buffers_.push_back(s);
	}
}

void Background::draw(SDL_Surface* surface) {
	for(Star& s : buffers_) {
		canvas_->drawStar(s);
	}
}
} /* namespace tankwar */
