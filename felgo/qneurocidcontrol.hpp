#ifndef QNEUROCIDCONTROL_HPP
#define QNEUROCIDCONTROL_HPP

#include <QObject>
#include "../src/canvas.hpp"


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
};

#endif // QNEUROCIDCONTROL_HPP
