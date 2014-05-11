/*
 * background.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: elchaschab
 */

#include "background.hpp"
#include "battlefieldlayout.hpp"
#include "util.hpp"
#include "sdlcanvas.hpp"
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL.h>

namespace neurocid {


Background::Background(SDLCanvas* canvas) :
		canvas_(canvas) {
}

void Background::init(BattleFieldLayout& bfl) {
	Coord w = bfl.width_  * 4;
	Coord h = bfl.height_ * 4;

	for(size_t i = 0; i < 300; i++) {
		Star s;
		s.z_ = 4;
		s.x = fRand(-w,w);
		s.y = fRand(-h,h);
		s.radius = fRand(1,3);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(100,255);
		s.step = 0.7;
		s.discontinuity = std::max(1.0, round(s.radius / 8));
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 600; i++) {
		Star s;
		s.z_ = 5;
		s.x = fRand(-w,w);
		s.y = fRand(-h,h);
		s.radius = fRand(1,3);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(100,255);
		s.step = 0.7;
		s.discontinuity = std::max(1.0, round(s.radius / 8));
		buffers_.push_back(s);
	}

	for(size_t i = 0; i < 1200; i++) {
		Star s;
		s.z_ = 6;
		s.x = fRand(-w,w);
		s.y = fRand(-h,h);
		s.radius = fRand(1,2);
		s.r = 255 - iRand(0,20);
		s.g = 255 - iRand(0,20);
		s.b = 255 - iRand(0,20);
		s.alpha = fRand(100,255);
		s.step = 0.7;
		s.discontinuity = std::max(1.0, round(s.radius / 8));
		buffers_.push_back(s);
	}
}

void Background::draw(SDL_Surface* surface) {
	for(Star& s : buffers_) {
		canvas_->drawStar(s);
	}
}
} /* namespace tankwar */
