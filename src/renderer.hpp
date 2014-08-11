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
	bool resetCanvas_ = false;
	BattleField* field_ = NULL;
	std::mutex updateMutex_;
	static Renderer* instance_;
public:
	Renderer(size_t frameRate);
	void update(BattleField* field);
	void renderGui();
	void render();

	bool isEnabled() {
	  return enabled_;
	}

	void setEnabled(bool e);
	void setFramerate(double frameRate) {
		frameRate_ = frameRate;
	}

	static Renderer* getInstance() {
		if(instance_ == NULL)
			instance_ = new Renderer(Options::getInstance()->FRAMERATE);
		return instance_;
	}

  static void destroy() {
    if(instance_)
      delete instance_;

    instance_ = NULL;
  }
};
} /* namespace neurocid */

#endif /* RENDERER_HPP_ */
