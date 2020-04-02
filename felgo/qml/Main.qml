import Felgo 3.0
import QtQuick 2.5


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
                        var ctx = getContext("2d");
                        setRGBA(ctx, r, g, b, a);
                        ctx.beginPath();
                        ctx.moveTo(x1, y1);
                        ctx.lineTo(x2, y2);
                        ctx.stroke();
                    }

                    function circleRGBA(x, y, radius, r, g, b, a) {
                        var ctx = getContext("2d");
                        setRGBA(ctx, r, g, b, a);
                        ctx.beginPath();
                        ctx.arc(x,y,radius,0,2 * Math.PI);
                        ctx.stroke();
                    }
                }
            }
    }
}

