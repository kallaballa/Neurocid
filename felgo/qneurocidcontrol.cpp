#include "qneurocidcontrol.hpp"

#include "../src/canvas.hpp"
#include "../src/gamestate.hpp"
#include "../src/options.hpp"

using namespace neurocid;

void QNeurocidControl::zoomIn() {
    Canvas::getInstance()->zoomIn();
}

void QNeurocidControl::zoomOut() {
    Canvas::getInstance()->zoomOut();
}

void QNeurocidControl::left() {
    Canvas::getInstance()->left();
}

void QNeurocidControl::right() {
    Canvas::getInstance()->right();
}

void QNeurocidControl::up() {
    Canvas::getInstance()->up();
}

void QNeurocidControl::down() {
    Canvas::getInstance()->down();
}

void QNeurocidControl::tiltUp() {
    Canvas::getInstance()->tiltUp();
}

void QNeurocidControl::tiltDown() {
    Canvas::getInstance()->tiltDown();
}

void QNeurocidControl::setSpeed(int s) {
    auto* gameState = GameState::getInstance();
    if(s == 0) {
        gameState->setSlow(false);
        gameState->setSlower(false);
    } else if(s == 1) {
        gameState->setSlow(true);
    } else if(s == 1) {
        gameState->setSlower(true);
    }
}

void QNeurocidControl::setPaused(bool p) {
    if(!p)
        neurocid::GameState::getInstance()->resume();
    else
        neurocid::GameState::getInstance()->pause();
}

void QNeurocidControl::dumpTeams() {
    neurocid::GameState::getInstance()->dumpTeams();
}

void QNeurocidControl::resize(int width, int height) {
    if(Canvas::isInitialized())
        Canvas::getInstance()->resize(width,height);
}

