import QtQuick 2.0

/*!
   \qmltype GfxCanvas
   \inherits Canvas
   \brief
        This is an incomplete drop-in replacement for SDL_gfx based drawing functions.
        It only implements functions required by Neurocid and doesn't yet honor all
        the api conventions. e.g.: no error reporting through returning an int
*/
Canvas {
    // FIXME Benchmark renderTarget and renderStrategy, though the current choice should be fine
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

    /* FIXME
        Arcs drawn with this function and with SDL_gfx differ.
    */
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
}
