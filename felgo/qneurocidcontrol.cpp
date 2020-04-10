#include "qneurocidcontrol.hpp"

#include "canvas.hpp"
#include "gamestate.hpp"
#include "options.hpp"

using namespace neurocid;

QNeurocidControl::QNeurocidControl(QObject *parent) :QObject(parent){
}

/*!
 * \fn void zoomIn()
 * Zooms into the scene displayed by the canvas
 */
void QNeurocidControl::zoomIn() {
    Canvas::getInstance()->zoomIn();
}

/*!
 * \fn void zoomOut()
 * Zooms out of the scene displayed by the canvas
 */
void QNeurocidControl::zoomOut() {
    Canvas::getInstance()->zoomOut();
}

/*!
 * \fn void left()
 * Moves the viewport of the canvas left
 */
void QNeurocidControl::left() {
    Canvas::getInstance()->left();
}

/*!
 * \fn void right()
 * Moves the viewport of the canvas right
 */
void QNeurocidControl::right() {
    Canvas::getInstance()->right();
}

/*!
 * \fn void up()
 * Moves the viewport of the canvas up
 */
void QNeurocidControl::up() {
    Canvas::getInstance()->up();
}

/*!
 * \fn void down()
 * Moves the viewport of the canvas down
 */
void QNeurocidControl::down() {
    Canvas::getInstance()->down();
}

/*!
 * \fn void tiltUp()
 * Tilts the battle field display in the canvas up, creating a pseudo-3D effect
 */
void QNeurocidControl::tiltUp() {
    Canvas::getInstance()->tiltUp();
}

/*!
 * \fn void tiltDown()
 * Tilts the battle field display in the canvas down, creating a pseudo-3D effect
 */
void QNeurocidControl::tiltDown() {
    Canvas::getInstance()->tiltDown();
}

/*!
 * \fn void resizeCanvasBackend(int width, int height)
 * Resize the neurocid::Canvas to the dimensions provided by the parameters
 * \a width and \a height. Usually the neurocid::Canvas is resized in
 * consequence of the QML-GfxCanvas being resized.
 */
void QNeurocidControl::resizeCanvasBackend(int width, int height) {
    if(Canvas::isInitialized())
        Canvas::getInstance()->resize(width,height);
}

/*!
 * \fn void setSpeed(int s)
 * Sets the speed of the neurocid simulation. if \a s is set to 2 the
 * simulation relinquishes control over the simulation speed which in effect
 * results in variable speed (depending on the resources available to the
 * program). In most scenarios that means that the simulation will run faster
 * at the start of the simulation then towards the end, when there are less
 * game objects remaining.
 * This does not effect the framerate.
 */
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

/*!
 * \fn void setPaused(bool p)
 * If \a p is true the game will be paused. If changed to false again the game
 * will resume.
 * This does not effect the framerate.
 */
void QNeurocidControl::setPaused(bool p) {
    if(!p)
        GameState::getInstance()->resume();
    else
        GameState::getInstance()->pause();
}

/*!
 * \fn void dumpTeams()
 * Saves the teams to time-stamped files using the following strftime-compatible
 * (http://www.cplusplus.com/reference/ctime/strftime/) format-strings:
 *      "dumpA_%d-%m-%Y_%I:%M:%S.pop"
 *      "dumpB_%d-%m-%Y_%I:%M:%S.pop"
 * To do that the simulation is briefly paused.
 */
void QNeurocidControl::dumpTeams() {
    GameState::getInstance()->dumpTeams();
}


