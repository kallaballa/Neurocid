import Felgo 3.0
import QtQuick 2.5


App {
    id: app

    NavigationStack {
        Page {
            title: "A Felgo Frontend for Neurocid"

            /*
                both the appbutton "invisible" and the canvas "nc_canvas" fill the whole page
                but only canvas is visible. That way, when you click in the canvas area you actually
                click the button.
            */
            Column {
                anchors.fill: parent;
                AppButton {
                    anchors.fill: parent;
                    id: invisible
                    text: "I'm invisible"
                    onClicked: nc_canvas.requestPaint()
                }

                Canvas {
                    id: nc_canvas
                    anchors.fill: parent;

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
                        return makePadded8BitHexString(r) +
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
                        setRGBA(ctx, r, g, b, a);
                        ctx.beginPath();
                        ctx.moveTo(x1, y1);
                        ctx.lineTo(x2, y2);
                        ctx.stroke();
                    }

                    function circleRGBA(x, y, radius, r, g, b, a) {
                        setRGBA(ctx, r, g, b, a);
                        ctx.beginPath();
                        ctx.arc(x,y,radius,0,2 * Math.PI);
                        ctx.stroke();
                    }

                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.fillStyle = Qt.rgba(1, 0, 0, 1);
                        ctx.fillRect(0, 0, width, height);
                        console.log("FRAME");
                    }
                }

                Item {
                }
            }
        }

    }
}

