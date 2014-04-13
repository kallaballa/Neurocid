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
	for(size_t i = 0; i < 100; i++) {
		Star s;
		s.x = fRand(0,canvas_->width());
		s.y = fRand(0,canvas_->height());
		s.radius = fRand(2,20);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(10,255);
		s.step = s.radius / 40;
		s.discontinuity = round(s.radius / 5);
		s.scale = 0;
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 200; i++) {
		Star s;
		s.x = fRand(-600000,600000);
		s.y = fRand(-600000,600000);
		s.radius = fRand(1000,4000);
		s.r = 255 - iRand(0,10);
		s.g = 255 - iRand(0,10);
		s.b = 255 - iRand(0,10);
		s.alpha = fRand(200,255);
		s.step = 0.997;
		s.discontinuity = round(s.radius / 8);
		s.scale = 0.7;
		buffers_.push_back(s);
	}
	for(size_t i = 0; i < 200; i++) {
		Star s;
		s.x = fRand(-600000,600000);
		s.y = fRand(-600000,600000);
		s.radius = fRand(1000,4000);
		s.r = 255 - iRand(0,10);
		s.g = 255 - iRand(0,10);
		s.b = 255 - iRand(0,10);
		s.alpha = fRand(200,255);
		s.step = 0.997;
		s.discontinuity = round(s.radius / 8);
		s.scale = 0.4;
		buffers_.push_back(s);
	}
	for(size_t i = 0; i < 200; i++) {
		Star s;
		s.x = fRand(-600000,600000);
		s.y = fRand(-600000,600000);
		s.radius = fRand(1000,4000);
		s.r = 255 - iRand(0,10);
		s.g = 255 - iRand(0,10);
		s.b = 255 - iRand(0,10);
		s.alpha = fRand(200,255);
		s.step = 0.997;
		s.discontinuity = round(s.radius / 8);
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
