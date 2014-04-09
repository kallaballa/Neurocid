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
	for(size_t i = 0; i < 200; i++) {
		Star s;
		s.x = fRand(0,canvas_->width());
		s.y = fRand(0,canvas_->height());
		s.radius = fRand(0,3);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(10,128);
		s.step = fRand(0,255/s.radius);
		s.scale = 0;
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 500; i++) {
		Star s;
		s.x = fRand(0,300000);
		s.y = fRand(0,300000);
		s.radius = fRand(0,60);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(10,64);
		s.step = fRand(0,255/s.radius);
		s.scale = 1.5;
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 500; i++) {
		Star s;
		s.x = fRand(0,300000);
		s.y = fRand(0,300000);
		s.radius = fRand(0,60);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(10,64);
		s.step = fRand(0,255/s.radius);
		s.scale = 3;
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 500; i++) {
		Star s;
		s.x = fRand(0,300000);
		s.y = fRand(0,300000);
		s.radius = fRand(0,60);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(10,64);
		s.step = fRand(0,255/s.radius);
		s.scale = 6;
		buffers_.push_back(s);
	}
}

void Background::draw(SDL_Surface* surface) {
	for(Star& s : buffers_) {
		canvas_->drawStar(s);
	}
}
} /* namespace tankwar */
