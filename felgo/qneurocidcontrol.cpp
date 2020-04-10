/*!
    \class QNeurocidControl
    \brief A class that simply forwards ui signals to the Neurocid backend.
*/

#include "qneurocidcontrol.hpp"

#include "canvas.hpp"
#include "gamestate.hpp"
#include "options.hpp"

using namespace neurocid;

/*!
 * \brief QNeurocidControl::QNeurocidControl
 * \param parent
 */
QNeurocidControl::QNeurocidControl(QObject *parent) :QObject(parent){
}

/*!
 * \brief QNeurocidControl::zoomIn
 * Zooms into the scene displayed by the canvas
 */
void QNeurocidControl::zoomIn() {
    Canvas::getInstance()->zoomIn();
}

/*!
 * \brief QNeurocidControl::zoomOut
 * Zooms out of the scene displayed by the canvas
 */
void QNeurocidControl::zoomOut() {
    Canvas::getInstance()->zoomOut();
}

/*!
 * \brief QNeurocidControl::left
 * Moves the viewport of the canvas left
 */
void QNeurocidControl::left() {
    Canvas::getInstance()->left();
}

/*!
 * \brief QNeurocidControl::right
 * Moves the viewport of the canvas right
 */
void QNeurocidControl::right() {
    Canvas::getInstance()->right();
}

/*!
 * \brief QNeurocidControl::up
 * Moves the viewport of the canvas up
 */
void QNeurocidControl::up() {
    Canvas::getInstance()->up();
}

/*!
 * \brief QNeurocidControl::down
 * Moves the viewport of the canvas down
 */
void QNeurocidControl::down() {
    Canvas::getInstance()->down();
}

/*!
 * \brief QNeurocidControl::tiltUp
 * Tilts the battle field display in the canvas up, creating a pseudo-3D effect
 */
void QNeurocidControl::tiltUp() {
    Canvas::getInstance()->tiltUp();
}

/*!
 * \brief QNeurocidControl::tiltDown
 * Tilts the battle field display in the canvas down, creating a pseudo-3D effect
 */
void QNeurocidControl::tiltDown() {
    Canvas::getInstance()->tiltDown();
}

/*!
 * \brief QNeurocidControl::resizeCanvasBackend
 * \param width
 * \param height
 * Resize the neurocid::Canvas to the dimensions provided by the parameters
 * \a width and \a height. Usually the neurocid::Canvas is resized in
 * consequence of the QML-GfxCanvas being resized.
 */
void QNeurocidControl::resizeCanvasBackend(int width, int height) {
    if(Canvas::isInitialized())
        Canvas::getInstance()->resize(width,height);
}

/*!
 * \brief QNeurocidControl::setSpeed
 * \param s A value from 0 to 2. 0 being the slowest and and 2 the fastest.
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
 * \brief QNeurocidControl::setPaused
 * \param p If true the game will be paused
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
 * \brief QNeurocidControl::dumpTeams
 * Saves the teams to time-stamped files using the following strftime-compatible
 * (http://www.cplusplus.com/reference/ctime/strftime/) format-strings:
 *      "dumpA_%d-%m-%Y_%I:%M:%S.pop"
 *      "dumpB_%d-%m-%Y_%I:%M:%S.pop"
 * To do that the simulation is briefly paused.
 */
void QNeurocidControl::dumpTeams() {
    GameState::getInstance()->dumpTeams();
}


