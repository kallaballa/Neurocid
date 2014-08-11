/*
 * options.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef OPTIONS_HPP_
#define OPTIONS_HPP_

#include <stddef.h>

namespace neurocid {

class Options {
	static Options* instance_;
public:
	size_t WINDOW_HEIGHT = 800;
	size_t WINDOW_WIDTH = 800;
	size_t FRAMERATE = 25;

	static Options* getInstance() {
		if(instance_ == NULL)
			instance_ = new Options();
		return instance_;
	}

  static void destroy() {
    if(instance_)
      delete instance_;

    instance_ = NULL;
  }
};

} /* namespace neurocid */

#endif /* OPTIONS_HPP_ */
