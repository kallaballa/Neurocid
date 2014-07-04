#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

#include <stddef.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <cassert>

namespace neurocid {

class Game;

class GameState {
private:
	static GameState* instance_;
	Game* currentGame_ = NULL;
	bool run_ = true;
	bool paused_ = false;
	bool slow_ = false;
	bool slower_ = true;

	bool enableAutosave_ = false;
  std::chrono::minutes autosaveInterval_;

  std::mutex dumpMutex;
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

	//FIXME implement disabling autosave
	void enableAutosave(long intervalMinutes) {
	  assert(intervalMinutes > 0);
	  autosaveInterval_ = std::chrono::minutes(intervalMinutes);
	  if(!enableAutosave_) {
	    enableAutosave_ = true;
	    std::thread t([&](){
	      while(enableAutosave_) {
	        std::this_thread::sleep_for(autosaveInterval_);
	        if(enableAutosave_)
	          dumpTeams();
	      }
	    });

	    t.detach();
	  }
	}

	void pauseBarrier(long timeoutMillis) {
	  while(isRunning()) {
      if(isPaused()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMillis));
      } else {
        break;
      }
	  }
	}

	bool isPaused() {
		return paused_;
	}

	//FIXME return after the game has actually paused
	void pause() {
		paused_ = true;
	}

	void resume() {
		paused_ = false;
	}

	void stop() {
		run_ = false;
	}

	void dumpTeams();

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
