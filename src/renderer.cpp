/*
 * renderer.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#include "renderer.hpp"
#include "gamestate.hpp"
#include "canvas.hpp"
#include "time_tracker.hpp"
#include "gui.hpp"
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
  updateMutex_.lock();
  field_ = field;
  if (field_ == NULL)
    Canvas::getInstance()->reset();
  updateMutex_.unlock();
}

void Renderer::setEnabled(bool e) {
  if (!enabled_ && e)
    Canvas::getInstance()->reset();
  enabled_ = e;
}

void Renderer::renderGui() {
  if(Gui::isInitalized()) {
    Gui& gui = *Gui::getInstance();
    gui.update(*field_);
    gui.logic();
    gui.draw();
  }
}

void Renderer::render(bool dosleep) {
  size_t sleep = (size_t) round(1000 / frameRate_);
  TimeTracker& tt = *TimeTracker::getInstance();
  Canvas& canvas = *Canvas::getInstance();
  size_t dur = tt.measure([&]() {
    updateMutex_.lock();
    if(field_ != NULL) {
      if(isEnabled()) {
        canvas.clear();
        canvas.render(*field_);
        renderGui();
        canvas.update();
        resetCanvas_ = true;
      } else if (resetCanvas_) {
        canvas.clear();
        canvas.reset();
        renderGui();
        canvas.update();
        resetCanvas_ = false;
      }
    }
    updateMutex_.unlock();
  });
  dur /= 1000;
  if(dosleep){
    if (dur < sleep)
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep - dur));
  }
}
} /* namespace neurocid */
