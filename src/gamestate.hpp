#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

#include <stddef.h>
#include <thread>
#include <chrono>
#include <iostream>



namespace tankwar {

class Game;

class GameState {
private:
	static GameState* instance_;
	Game* currentGame_ = NULL;
	bool run_ = true;
	bool paused_ = false;
	bool slow_ = false;
	bool slower_ = true;
public:
	bool isSlow() {
		return slow_;
	}

	bool isSlower() {
		return slower_;
	}

	void setSlow(bool s) {
		slow_ = s;
		if(slow_)
			slower_ = false;
	}

	void setSlower(bool s) {
		slower_ = s;
		if(slower_)
			slow_ = false;
	}

	bool isRunning() {
		return run_;
	}

	bool tryPause() {
		if(paused_) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		return paused_ && run_;
	}

	bool isPaused() {
		return paused_;
	}

	void pause() {
		paused_ = true;
	}

	void resume() {
		paused_ = false;
	}

	void stop() {
		run_ = false;
	}

	static GameState* getInstance() {
		if(instance_ == NULL)
			instance_ = new GameState();
		return instance_;
	}

	void setCurrentGame(Game* g);
	Game* getCurrentGame();
};
}

#endif /* GAMESTATE_HPP_ */
