#ifndef GUI_HPP_
#define GUI_HPP_

#include "error.hpp"
#include <stddef.h>

namespace neurocid {

class BattleField;

class Gui {
	static Gui* instance_;
public:
	static void init(Gui* gui) {
		CHECK(instance_ == NULL);
		instance_ = gui;
	}

	static bool isInitalized() {
		return instance_ != NULL;
	}

	static Gui* getInstance() {
	  CHECK(instance_ != NULL);
		return instance_;
	}

  static void destroy() {
    if(instance_)
      delete instance_;

    instance_ = NULL;
  }

	virtual ~Gui() {
	}

	virtual void update(BattleField& field) = 0;
	virtual void logic() = 0;
	virtual void draw() = 0;
};
}
#endif /* GUI_HPP_ */
