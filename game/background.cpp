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
	Coord w = bfl.width_;
	Coord h = bfl.height_;

	for(size_t i = 0; i < 300; i++) {
		Star s;
		s.scale = 0.6;
		s.x = fRand(-w/2/s.scale,w/s.scale);
		s.y = fRand(-h/2/s.scale,h/s.scale);
		s.radius = fRand(5,10);
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
		s.scale = 0.4;

		s.x = fRand(-w/2/s.scale,w/s.scale);
		s.y = fRand(-h/2/s.scale,h/s.scale);
		s.radius = fRand(5,8);
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
		s.scale = 0.2;

		s.x = fRand(-w/2/s.scale,w/s.scale);
		s.y = fRand(-h/2/s.scale,h/s.scale);
		s.radius = fRand(5,6);
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
