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
    void zoomIn();
    void zoomOut();
    void left();
    void right();
    void up();
    void down();
    void tiltUp();
    void tiltDown();

    void setSpeed(int s);
    void setPaused(bool p);
    void dumpTeams();

};

#endif // QNEUROCIDCONTROL_HPP
