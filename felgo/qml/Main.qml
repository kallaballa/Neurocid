import Felgo 3.0
import QtQuick 2.9
import QtQuick.Extras 1.4

App {
    id: app

    NavigationStack {
        Page {
            title: "A Felgo Frontend for Neurocid"
            Canvas {
                id: nc_canvas
                objectName: "nc_canvas"
                anchors.fill: parent;
                renderTarget: Canvas.Image;
                renderStrategy: Canvas.Immediate;

                signal zoomIn()
                signal zoomOut()
                signal left()
                signal right()
                signal up()
                signal down()
                signal tiltUp()
                signal tiltDown()
                signal setSpeed(int s)
                signal togglePauseGame()
                signal dumpTeams()

                function makePadded8BitHexString(v) {
                    if(v < 0 || v > 255) {
                        console.error("makePadded8BitHexString: value out of bounds");
                        return "00";
                    } else {
                        var vHex = v.toString(16);
                        if(vHex.length === 1) {
                            return "0" + vHex;
                        } else {
                            return vHex;
                        }
                    }
                }

                function makeHtmlColorString(r, g, b) {
                    return "#" + makePadded8BitHexString(r) +
                            makePadded8BitHexString(g) +
                            makePadded8BitHexString(b)
                }

                function setRGBA(ctx, r, g, b, a) {
                    if(a < 0 || a > 255) {
                        console.error("setRGBA: alpha value out of bounds");
                        return;
                    }
                    ctx.globalAlpha = a / 255.0;
                    ctx.fillStyle = ctx.strokeStyle = makeHtmlColorString(r, g, b);
                }

                function lineRGBA(x1, y1, x2, y2, r, g, b, a) {
                    var ctx = getContext("2d");
                    setRGBA(ctx, r, g, b, a);
                    ctx.beginPath();
                    ctx.moveTo(x1, y1);
                    ctx.lineTo(x2, y2);
                    ctx.stroke();
                }

                function filledCircleRGBA(x, y, radius, r, g, b, a) {
                    var ctx = getContext("2d");
                    setRGBA(ctx, r, g, b, a);
                    ctx.beginPath();
                    ctx.arc(x,y,radius,0,2 * Math.PI);
                    ctx.fill();
                }

                function circleRGBA(x, y, radius, r, g, b, a) {
                    var ctx = getContext("2d");
                    setRGBA(ctx, r, g, b, a);
                    ctx.beginPath();
                    ctx.arc(x,y,radius,0,2 * Math.PI);
                    ctx.stroke();
                }

                function pieRGBA(x, y, radius, start, end, r, g, b, a) {
                    var ctx = getContext("2d");
                    setRGBA(ctx, r, g, b, a);
                    ctx.beginPath();
                    ctx.arc(x,y,radius,(start / 360.0) * (2.0 * Math.PI),(end / 360.0) * (2.0 * Math.PI));
                    ctx.fill();
                }

                function filledPolygonRGBA(vx, vy, r, g, b, a) {
                    if(vx.length < 2 || vy.length < 2) {
                        console.error("Not enough polygon points provided");
                        return;
                    } else if(vx.length !== vy.length) {
                        console.error("Not the same number of x and y coordinates");
                        return;
                    } else {
                        var ctx = getContext("2d");
                        setRGBA(ctx, r, g, b, a);
                        ctx.beginPath();
                        ctx.moveTo(vx[0], vy[0]);
                        for( var index=1 ; index < vx.length; index++ ){
                            ctx.lineTo( vx[index] , vy[index] )
                        }

                        ctx.closePath();
                        ctx.fill();
                    }
                }

                function ellipseRGBA(x, y, radiusx, radiusy, r, g, b, a) {
                    var ctx = getContext("2d");
                    ctx.ellipse(x, y, radiusx, radiusy, 0, 0, 2 * Math.PI, false);
                }

                function rectangleRGBA(x1, y1, x2, y2, r, g, b, a) {
                    var ctx = c.getContext("2d");
                    ctx.beginPath();
                    ctx.rect(x1, y1, Math.abs(x1 - x2), Math.abs(y1 - y2));
                    ctx.stroke();
                }

                function clear() {
                    var ctx = getContext("2d");
                    setRGBA(ctx, 0,0,0,255);
                    ctx.fillRect(0, 0, width, height);
                }

                function flip() {
                    requestPaint();
                }
                Item {
                    anchors.fill: parent
                    Row {
                        id: panRow
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter

                        AppButton {
                            id: leftBtn
                            text: "Left"
                            onClicked: nc_canvas.left()
                        }

                        AppButton {
                            id: rightBtn
                            text: "Right"
                            onClicked: nc_canvas.right()
                        }

                        AppButton {
                            id: upBtn
                            text: "Up"
                            onClicked: nc_canvas.up()
                        }

                        AppButton {
                            id: downBtn
                            text: "Down"
                            onClicked: nc_canvas.down()
                        }
                    }
                    Row {
                        id: zoomAndTiltRow
                        anchors.top: panRow.bottom
                        anchors.horizontalCenter: parent.horizontalCenter

                        AppButton {
                            id: tiltUpBtn
                            text: "Tilt Up"
                            onClicked: nc_canvas.tiltUp()
                        }

                        AppButton {
                            id: tiltDownBtn
                            text: "Tilt Down"
                            onClicked: nc_canvas.tiltDown()
                        }

                        AppButton {
                            id: zoomInBtn
                            text: "Zoom In"
                            onClicked: nc_canvas.zoomIn()
                        }

                        AppButton {
                            id: zoomOutBtn
                            text: "Zoom Out"
                            onClicked: nc_canvas.zoomOut()
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
                            text: gameSpeed === 0 ? "Fast" : (gameSpeed === 1 ? "Slow" : "Slowest")
                            onClicked: {
                                if(gameSpeed > 1)
                                    gameSpeed = 0;
                                else
                                    gameSpeed++;
                                nc_canvas.setSpeed(gameSpeed)
                            }
                        }

                        // ToggleButton component is ugly and bloated: https://doc.qt.io/qt-5/qml-qtquick-extras-togglebutton.htm
                        AppButton {
                            id: pauseGameTgl
                            property bool enabled: false
                            backgroundColor: !enabled ? "green" : "red"
                            text: "Pause"
                            onClicked: {
                                enabled = !enabled
                                nc_canvas.togglePauseGame()
                            }
                        }

                        AppButton {
                            id: dumpTeamsBtn
                            text: "Dump Teams"
                            onClicked: nc_canvas.dumpTeams()
                        }
                    }
                }
            }
        }
    }
}

