/*
 * time_tracker.hpp
 *
 *  Created on: Mar 22, 2014
 *      Author: elchaschab
 */

#ifndef TIME_TRACKER_HPP_
#define TIME_TRACKER_HPP_

#include "gamestate.hpp"
#include <chrono>
#include <map>
#include <string>
#include <sstream>
#include <ostream>
#include <limits>

namespace neurocid {

using std::ostream;
using std::stringstream;
using std::string;
using std::map;
using std::chrono::microseconds;

struct TimeInfo {
	long totalCnt_ = 0;
	long totalTime_ = 0;
	long gameCnt_ = 0;
	long gameTime_ = 0;
	long last_ = 0;
	map<string, TimeInfo> children_;

	void add(size_t t) {
		last_ = t;
		totalTime_ += t;
    gameTime_ += t;
		++totalCnt_;
    ++gameCnt_;

		if(totalCnt_ == std::numeric_limits<long>::max() || totalTime_ == std::numeric_limits<long>::max()) {
			totalCnt_ = 0;
			totalTime_ = 0;
		}

    if(gameCnt_ == std::numeric_limits<long>::max() || gameTime_ == std::numeric_limits<long>::max()) {
      gameCnt_ = 0;
      gameTime_ = 0;
    }
	}

	void newGame() {
	  gameCnt_ = 0;
	  gameTime_ = 0;
	}

	string str() const {
		stringstream ss;
		ss << (double)totalTime_/(double)totalCnt_/1000.0 << " = (" << (double)totalTime_/(double)1000.0 << '\\' << totalCnt_ << ')';
		return ss.str();
	}
};

inline std::ostream& operator<<(ostream& os, TimeInfo& ti) {
	os << (double)ti.totalTime_/(double)ti.totalCnt_/1000 << " = (" << (double)ti.totalTime_/(double)1000 << '\\' << ti.totalCnt_ << ')';
	return os;
}

class TimeTracker {
private:
	static TimeTracker* instance_;

	 map<string, TimeInfo> tiMap_;
	 bool enabled_;
	TimeTracker();
public:
	virtual ~TimeTracker();

	template<typename F> void execute(const string& name, F const &func)
	{
		if(!GameState::getInstance()->isRunning())
			return;
		auto start = std::chrono::system_clock::now();
		func();
		auto duration = std::chrono::duration_cast<microseconds>(
			std::chrono::system_clock::now() - start);
		tiMap_[name].add(duration.count());
	}

	template<typename F> void execute(const string& parentName, const string& name, F const &func)
	{
		if(!GameState::getInstance()->isRunning())
			return;
		auto start = std::chrono::system_clock::now();
		func();
		auto duration = std::chrono::duration_cast<microseconds>(
			std::chrono::system_clock::now() - start);
		tiMap_[parentName].children_[name].add(duration.count());
	}

	template<typename F> size_t measure(F const &func)
	{
		if(!GameState::getInstance()->isRunning())
			return 0;
		auto start = std::chrono::system_clock::now();
		func();
		auto duration = std::chrono::duration_cast<microseconds>(
			std::chrono::system_clock::now() - start);
		return duration.count();
	}

	bool isEnabled() {
		return enabled_;
	}

	void setEnabled(bool e) {
		enabled_ = e;
	}

	void print(ostream& os) {
		stringstream ss;
		ss << "Time tracking info: " << std::endl;
		for(auto it : tiMap_) {
			ss << "\t" << it.first << ": " << it.second << std::endl;
			for(auto itc : it.second.children_) {
				ss << "\t\t" << itc.first << ": " << itc.second << std::endl;
			}
		}
		ss << std::endl;

		os << ss.str();
	}

	const map<string, TimeInfo>& getMap() {
		return tiMap_;
	}

	void reset() {
		tiMap_.clear();
	}

	static TimeTracker* getInstance() {
		if(instance_ == NULL)
			instance_ = new TimeTracker();

		return instance_;
	}

  static void destroy() {
    if(instance_)
      delete instance_;

    instance_ = NULL;
  }

  static void newGame() {
    for(auto pair : instance_->tiMap_) {
      pair.second.newGame();
      for(auto pairc : pair.second.children_) {
        pairc.second.newGame();
      }
    }
  }
};

} /* namespace neurocid */

#endif /* TIME_TRACKER_HPP_ */
