#ifndef GUI_HPP_
#define GUI_HPP_

#include <cassert>
#include <SDL_events.h>

namespace neurocid {

struct BattleField;

class Gui {
	static Gui* instance_;
public:
	static void init(Gui* gui) {
		assert(instance_ == NULL);
		instance_ = gui;
	}

	static bool isInitalized() {
		return instance_ != NULL;
	}

	static Gui* getInstance() {
		assert(instance_ != NULL);
		return instance_;
	}

	~Gui() {
	}

	virtual void pushEvent(SDL_Event event) = 0;
	virtual void update(BattleField& field) = 0;
	virtual void logic() = 0;
	virtual void draw() = 0;
};
}
#endif /* GUI_HPP_ */
