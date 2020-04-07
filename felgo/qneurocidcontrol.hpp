#ifndef QNEUROCIDCONTROL_HPP
#define QNEUROCIDCONTROL_HPP

#include <QObject>

class QNeurocidControl : public QObject
{
    Q_OBJECT
public:
    explicit QNeurocidControl(QObject *parent = nullptr) :QObject(parent){}
    ~QNeurocidControl() {}
signals:

public slots:
    // ------ neurocid::Canvas manipulation slots ------

    /*!
     * \brief zoomIn
     * Zooms into the scene displayed by the canvas
     */
    void zoomIn();

    /*!
     * \brief zoomOut
     * Zooms out of the scene displayed by the canvas
     */
    void zoomOut();

    /*!
     * \brief left
     * Moves the viewport of the canvas left
     */
    void left();

    /*!
     * \brief right
     * Moves the viewport of the canvas right
     */
    void right();

    /*!
     * \brief up
     * Moves the viewport of the canvas up
     */
    void up();

    /*!
     * \brief down
     * Moves the viewport of the canvas down
     */
    void down();

    /*!
     * \brief tiltUp
     * Tilts the battle field display in the canvas up, creating a pseudo-3D effect
     */
    void tiltUp();

    /*!
     * \brief tiltDown
     * Tilts the battle field display in the canvas down, creating a pseudo-3D effect
     */
    void tiltDown();

    /*!
     * \brief resizeCanvasBackend
     * \param width
     * \param height
     * Resize the neurocid::Canvas to the dimensions provided by the parameters
     * \a width and \a height. Usually the neurocid::Canvas is resized in
     * consequence of the QML-GfxCanvas being resized.
     */
    void resizeCanvasBackend(int width, int height);

    // ------ neurocid::GameState manipulation slots ------

    /*!
     * \brief setSpeed
     * \param s A value from 0 to 2. 0 being the slowest and and 2 the fastest.
     * Sets the speed of the neurocid simulation. if \a s is set to 2 the
     * simulation relinquishes control over the simulation speed which in effect
     * results in variable speed (depending on the resources available to the
     * program). In most scenarios that means that the simulation will run faster
     * at the start of the simulation then towards the end, when there are less
     * game objects remaining.
     * This does not effect the framerate.
     */
    void setSpeed(int s);

    /*!
     * \brief setPaused
     * \param p If true the game will be paused
     * If \a p is true the game will be paused. If changed to false again the game
     * will resume.
     * This does not effect the framerate.
     */
    void setPaused(bool p);

    /*!
     * \brief dumpTeams
     * Saves the teams to time-stamped files using the following strftime-compatible
     * (http://www.cplusplus.com/reference/ctime/strftime/) format-strings:
     *      "dumpA_%d-%m-%Y_%I:%M:%S.pop"
     *      "dumpB_%d-%m-%Y_%I:%M:%S.pop"
     * To do that the simulation is briefly paused.
     */
    void dumpTeams();
};

#endif // QNEUROCIDCONTROL_HPP
