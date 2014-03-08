#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

#include <stddef.h>

namespace tankwar {
class GameState {
private:
	static GameState* instance_;
	bool run_ = true;
	bool slow_ = false;
public:
	bool isSlow() {
		return slow_;
	}

	void setSlow(bool s) {
		slow_ = s;
	}

	bool isRunning() {
		return run_;
	}

	void stop() {
		run_ = false;
	}

	static GameState* getInstance() {
		if(instance_ == NULL)
			instance_ = new GameState();
		return instance_;
	}
};
}

#endif /* GAMESTATE_HPP_ */
