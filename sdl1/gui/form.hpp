/*
 * form.hpp
 *
 *  Created on: Apr 11, 2014
 *      Author: elchaschab
 */

#ifndef FORM_HPP_
#define FORM_HPP_

#include "screen.hpp"
#include <map>
#include <string>
#include <mutex>
#include <chrono>
#include <cassert>
#include <thread>

namespace neurocid {

using std::string;
using std::map;
using std::mutex;
class Form : public Screen {
public:
	enum Type {
		STRING,
		INT,
		FLOAT,
		BOOL
	};

	class Value {
		Type type_;
		string vs_;
		unsigned int vi_ = 0;
		float vf_ = 0;
		bool vb_ = 0;
	public:
		Value(string v) : type_(STRING), vs_(v) {}
		Value(unsigned int v) : type_(INT), vi_(v) {}
		Value(float v) : type_(FLOAT), vf_(v) {}
		Value(bool v) : type_(BOOL), vb_(v) {}

		string str() {
			assert(type_ == STRING);
			return vs_;
		}

		unsigned int integer() {
			assert(type_ == INT);
			return vi_;
		}

		float floatp() {
			assert(type_ == FLOAT);
			return vf_;
		}

		bool boolean() {
			assert(type_ == BOOL);
			return vb_;
		}

	};

	virtual ~Form() {};

	virtual void open() = 0;

	map<string, Value> query() {
		values_.clear();
		open_ = true;
		open();

		while(open_) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		return values_;
	}

	void submit() {
		open_ = false;
	}

	bool isOpen() {
		return open_;
	}
private:
	map<string, Value> values_;
	mutex queryMutex_;
	bool open_ = false;
};

} /* namespace neurocid */

#endif /* FORM_HPP_ */
