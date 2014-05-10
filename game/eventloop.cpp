/*
 * eventloop.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: elchaschab
 */

#include "game.hpp"
#include "eventloop.hpp"
#include "gamestate.hpp"
#include "canvas.hpp"
#include "renderer.hpp"
#include "gui.hpp"

#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

namespace neurocid {

inline void dumpTeams() {
	GameState& gameState = *GameState::getInstance();
	gameState.pause();
#ifndef _NO_THREADS
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
	Game* game = gameState.getCurrentGame();
	if(game != NULL) {
		  time_t rawtime;
		  struct tm * timeinfo;
		  char bufferA[80];
		  char bufferB[80];

		  time (&rawtime);
		  timeinfo = localtime(&rawtime);

		  strftime(bufferA,80,"dumpA_%d-%m-%Y_%I:%M:%S.pop",timeinfo);
		  strftime(bufferB,80,"dumpB_%d-%m-%Y_%I:%M:%S.pop",timeinfo);

		  string filenameA(bufferA);
		  string filenameB(bufferB);

		  ofstream osa(filenameA);
		  write_team(game->teams_[0], osa);

		  ofstream osb(filenameB);
		  write_team(game->teams_[1], osb);
	}
	gameState.resume();
}

void EventLoop::process() {
#ifndef _NO_EVENTLOOP
	GameState& gameState = *GameState::getInstance();
	Canvas& canvas = *Canvas::getInstance();
	Renderer& renderer = *Renderer::getInstance();
	Gui& gui = *Gui::getInstance();

	SDL_Event event;

	if(SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLKey::SDLK_ESCAPE) {
				std::cerr << "Quitting" << std::endl;
				gameState.stop();
			} else if ((event.key.keysym.mod == KMOD_LALT || event.key.keysym.mod == KMOD_RALT)) {
				if (event.key.keysym.sym == SDLKey::SDLK_SPACE) {
					if (renderer.isEnabled()) {
						if (gameState.isSlow()) {
							gameState.setSlower(true);
						} else if (gameState.isSlower()) {
							renderer.setEnabled(false);
							gameState.setSlower(false);
						} else {
							gameState.setSlow(true);
						}
					} else {
						renderer.setEnabled(true);
					}
				} else if (event.key.keysym.sym == SDLKey::SDLK_p) {
					if (gameState.isPaused())
						gameState.resume();
					else
						gameState.pause();
				} else if (event.key.keysym.sym == SDLKey::SDLK_c) {
					if (canvas.isDrawCentersEnabled())
						canvas.enableDrawCenters(false);
					else
						canvas.enableDrawCenters(true);
				} else if (event.key.keysym.sym == SDLKey::SDLK_e) {
					if (canvas.isDrawEnginesEnabled())
						canvas.enableDrawEngines(false);
					else
						canvas.enableDrawEngines(true);
				} else if (event.key.keysym.sym == SDLKey::SDLK_g) {
					if (canvas.isDrawGridEnabled())
						canvas.enableDrawGrid(false);
					else
						canvas.enableDrawGrid(true);
				} else if (event.key.keysym.sym == SDLKey::SDLK_a) {
					if (canvas.isDrawProjectilesEnabled())
						canvas.enableDrawProjectiles(false);
					else
						canvas.enableDrawProjectiles(true);
				} else if (event.key.keysym.sym == SDLKey::SDLK_l) {
					if (canvas.isDrawEliteEnabled())
						canvas.enableDrawElite(false);
					else
						canvas.enableDrawElite(true);
				} else if (event.key.keysym.sym == SDLKey::SDLK_d) {
					dumpTeams();
				} else if (event.key.keysym.sym == SDLKey::SDLK_PLUS) {
					canvas.zoomIn();
				} else if (event.key.keysym.sym == SDLKey::SDLK_MINUS) {
					canvas.zoomOut();
				} else if (event.key.keysym.sym == SDLKey::SDLK_LEFT) {
					canvas.left();
				} else if (event.key.keysym.sym == SDLKey::SDLK_RIGHT) {
					canvas.right();
				} else if (event.key.keysym.sym == SDLKey::SDLK_UP) {
					canvas.up();
				} else if (event.key.keysym.sym == SDLKey::SDLK_DOWN) {
					canvas.down();
				}
			} else
				gui.pushEvent(event);
			break;

		case SDL_KEYUP:
			if (event.key.keysym.sym != SDLKey::SDLK_ESCAPE
					&& event.key.keysym.mod != KMOD_LCTRL)
				gui.pushEvent(event);
			break;

		default:
			gui.pushEvent(event);
			break;
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(40));
#endif
}

} /* namespace tankwar */
