#include "qneurocidcontrol.hpp"

void QNeurocidControl::zoomIn() {
    neurocid::Canvas::getInstance()->zoomIn();
}

void QNeurocidControl::zoomOut() {
    neurocid::Canvas::getInstance()->zoomOut();
}

void QNeurocidControl::left() {
    neurocid::Canvas::getInstance()->left();
}

void QNeurocidControl::right() {
    neurocid::Canvas::getInstance()->right();
}

void QNeurocidControl::up() {
    neurocid::Canvas::getInstance()->up();
}

void QNeurocidControl::down() {
    neurocid::Canvas::getInstance()->down();
}

void QNeurocidControl::tiltUp() {
    neurocid::Canvas::getInstance()->tiltUp();
}

void QNeurocidControl::tiltDown() {
    neurocid::Canvas::getInstance()->tiltDown();
}
