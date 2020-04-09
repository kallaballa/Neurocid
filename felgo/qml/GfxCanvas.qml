import QtQuick 2.0

/*
  This is an incomplete drop-in replacement for SDL_gfx based rasterisation
  (https://en.wikipedia.org/wiki/Rasterisation) functions.
  It only implements functions required by Neurocid and doesn't yet honor all
  the api conventions. e.g.: no error reporting by returning an integer value.
*/
Canvas {
    // FIXME Benchmark renderTarget and renderStrategy, though the current choice should be fine
    renderTarget: Canvas.Image;
    renderStrategy: Canvas.Immediate;

    /**
        Creates a hex string from an integer value with a value from 0 to 255. If the resulting
        string consists of only one character a leading zero is added.
    */
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

    //Creates an html like rgb hex-string. e.g.: "#ff0000" for pure red
    function makeHtmlColorString(r, g, b) {
        return "#" + makePadded8BitHexString(r) +
                makePadded8BitHexString(g) +
                makePadded8BitHexString(b)
    }

    //Sets the fill and stroke color of the 2d context.
    function setRGBA(ctx, r, g, b, a) {
        if(a < 0 || a > 255) {
            console.error("setRGBA: alpha value out of bounds");
            return;
        }
        ctx.globalAlpha = a / 255.0;
        ctx.fillStyle = ctx.strokeStyle = makeHtmlColorString(r, g, b);
    }

    /*  -------------------------------
    *   All following methods have a pendant in felgogfx.hpp. Every function ending with RGBA
    *   is modelled after a SDL_gfx primitive function. the functions "clear" and "flip" don't have a
    *   counterpart in SDL_gfx. Please see felgogfx.hpp for more information.
    *
    *   You can find documentation on the SDL_gfx functions here:
    *   https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/_s_d_l2__gfx_primitives_8h.html
    *   -------------------------------
    */
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

    function clear(r,g,b) {
        var ctx = getContext("2d");
        setRGBA(ctx, r,g,b,255);
        ctx.fillRect(0, 0, width, height);
    }

    function flip() {
        //FIXME there are several ways to achieve a buffer flip. Find the optimal way.
        requestPaint();
    }
}
