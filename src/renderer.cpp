/*
 * renderer.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef _NO_VIDEOENC
#include "video_encoder.hpp"
#endif
#include "renderer.hpp"
#include "gamestate.hpp"
#include "canvas.hpp"
#include "time_tracker.hpp"
#include "gui/gui.hpp"
#include "gui/osd.hpp"
#include <thread>
#include <chrono>
#include <stddef.h>
#include <cmath>

namespace neurocid {

Renderer * Renderer::instance_ = NULL;

Renderer::Renderer(size_t frameRate) :
		frameRate_(frameRate) {
}

void Renderer::update(BattleField* field) {
	updateMutex.lock();
	field_ = field;
	updateMutex.unlock();
}

void Renderer::render() {
	size_t sleep = (size_t) round(1000 / frameRate_);
	TimeTracker& tt = *TimeTracker::getInstance();
	Canvas& canvas = *Canvas::getInstance();
	size_t dur = tt.measure([&]() {
					updateMutex.lock();
					if(field_ != NULL) {
						if(isEnabled()) {
							canvas.clear();
							canvas.render(*field_);
							Gui& gui = *Gui::getInstance();
							OsdScreenWidget& osd = *OsdScreenWidget::getInstance();
							osd.update(*field_);
							gui.logic();
							gui.draw();
							canvas.update();
#ifndef _NO_VIDEOENC
							VideoEncoder::getInstance()->encode(Canvas::getInstance()->getSurface());
#endif

							notifiedDisable = false;
						} else if (!notifiedDisable) {
							canvas.clear();
							Gui& gui = *Gui::getInstance();
							OsdScreenWidget& osd = *OsdScreenWidget::getInstance();
							osd.update(*field_);
							gui.logic();
							gui.draw();
							canvas.update();
							notifiedDisable = true;
						}
					}
					updateMutex.unlock();
	});
	dur/=1000;
	if(dur < sleep)
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep - dur));
}
} /* namespace neurocid */
