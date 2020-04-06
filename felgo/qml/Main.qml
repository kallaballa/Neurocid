import Felgo 3.0
import QtQuick 2.0

App {
    id: neurocidApp
    objectName: "neurocidApp"

    signal left()
    signal right()
    signal up()
    signal down()

    signal zoomIn()
    signal zoomOut()
    signal tiltUp()
    signal tiltDown()

    signal setSpeed(int s)
    signal setPaused(bool p)
    signal dumpTeams()

    signal resize(int width, int height)

    NavigationStack {
        Page {
            title: "A Felgo Frontend for Neurocid based on an SDLgfx-like Canvas"

            GfxCanvas {
                id: gfxCanvas
                objectName: "gfxCanvas"
                anchors.fill: parent;
                //make sure to send a initial resize signal when canvas becomes available
                onAvailableChanged: neurocidApp.resize(width, height)
                onCanvasSizeChanged: neurocidApp.resize(width, height)

                Item {
                    anchors.fill: parent
                    Row {
                        id: panRow
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter

                        AppButton {
                            id: leftBtn
                            text: "Left"
                            onClicked: neurocidApp.left()
                        }

                        AppButton {
                            id: rightBtn
                            text: "Right"
                            onClicked: neurocidApp.right()
                        }

                        AppButton {
                            id: upBtn
                            text: "Up"
                            onClicked: neurocidApp.up()
                        }

                        AppButton {
                            id: downBtn
                            text: "Down"
                            onClicked: neurocidApp.down()
                        }
                    }
                    Row {
                        id: zoomAndTiltRow
                        anchors.top: panRow.bottom
                        anchors.horizontalCenter: parent.horizontalCenter

                        AppButton {
                            id: zoomInBtn
                            text: "Zoom In"
                            onClicked: neurocidApp.zoomIn()
                        }

                        AppButton {
                            id: zoomOutBtn
                            text: "Zoom Out"
                            onClicked: neurocidApp.zoomOut()
                        }

                        AppButton {
                            id: tiltUpBtn
                            text: "Tilt Up"
                            onClicked: neurocidApp.tiltUp()
                        }

                        AppButton {
                            id: tiltDownBtn
                            text: "Tilt Down"
                            onClicked: neurocidApp.tiltDown()
                        }
                    }

                    Row {
                        id: gameStateRow
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom

                        AppButton {
                            id: changeSpeedTgl
                            property var gameSpeed: 0
                            backgroundColor: gameSpeed === 0 ? "red" : (gameSpeed === 1 ? "orange" : "yellow")
                            backgroundColorPressed: Qt.darker(gameSpeed === 0 ? "red" : (gameSpeed === 1 ? "orange" : "yellow"))
                            text: gameSpeed === 0 ? "Fast" : (gameSpeed === 1 ? "Slow" : "Slowest")
                            onClicked: {
                                if(gameSpeed > 1)
                                    gameSpeed = 0;
                                else
                                    gameSpeed++;
                                neurocidApp.setSpeed(gameSpeed)
                            }
                        }

                        // ToggleButton component is ugly and bloated: https://doc.qt.io/qt-5/qml-qtquick-extras-togglebutton.htm
                        AppButton {
                            id: pauseGameTgl
                            property bool paused: false
                            backgroundColor: !paused ? "green" : "red"
                            text: !paused ? "Pause" : "Resume"
                            onClicked: {
                                paused = !paused
                                neurocidApp.setPaused(paused)
                            }
                        }

                        AppButton {
                            id: dumpTeamsBtn
                            text: "Dump Teams"
                            onClicked: neurocidApp.dumpTeams()
                        }
                    }
                }
            }
        }
    }
}

