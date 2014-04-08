/*
 * renderer.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <mutex>
#include <stddef.h>
#include "battlefield.hpp"
#include "options.hpp"

namespace neurocid {

class Renderer {
	double frameRate_;
	bool enabled_ = true;
	bool notifiedDisable = false;
	BattleField* field_ = NULL;
	std::mutex updateMutex;
	static Renderer* instance_;
public:
	Renderer(size_t frameRate);
	void update(BattleField* field);
	void render();
	bool isEnabled() {
	  return enabled_;
	}

	void setEnabled(bool e) {
	  enabled_ = e;
	}

	void setFramerate(double frameRate) {
		frameRate_ = frameRate;
	}

	static Renderer* getInstance() {
		if(instance_ == NULL)
			instance_ = new Renderer(Options::getInstance()->FRAMERATE);
		return instance_;
	}
};
} /* namespace neurocid */

#endif /* RENDERER_HPP_ */
