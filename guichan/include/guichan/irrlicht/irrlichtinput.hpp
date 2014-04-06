/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004 - 2008 Olof Naess�n and Per Larsson
 *
 *
 * Per Larsson a.k.a finalman
 * Olof Naess�n a.k.a jansem/yakslem
 *
 * Visit: http://guichan.sourceforge.net
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Guichan nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GCN_IRRLICHTINPUT_HPP
#define GCN_IRRLICHTINPUT_HPP

#include <queue>

#include "irrlicht.h"

#include "guichan/input.hpp"
#include "guichan/keyinput.hpp"
#include "guichan/mouseinput.hpp"
#include "guichan/platform.hpp"

namespace gcn
{
    class Key;

    /**
    * Irrlicht implementation of Input.
    *
    * @author Aaron Andersen
    * @since 0.9.0
    */
    class GCN_EXTENSION_DECLSPEC IrrlichtInput : public Input
    {
    public:

        /**
        * Constructor.
        */
        IrrlichtInput(irr::IrrlichtDevice* device);

        /**
        * Pushes an Irrlicht event. It should be called at least once per frame to
        * update input with user input.
        *
        * @param event an event from Irrlicht.
        */
        virtual void pushInput(const irr::SEvent& event);

        /**
        * Polls all input. It exists for input driver compatibility. If you
        * only use Irrlicht and plan sticking with Irrlicht you can safely ignore this
        * function as it in the Irrlicht case does nothing.
        */
        virtual void _pollInput() { }


        // Inherited from Input

        virtual bool isKeyQueueEmpty();

        virtual KeyInput dequeueKeyInput();

        virtual bool isMouseQueueEmpty();

        virtual MouseInput dequeueMouseInput();

    protected:
        /**
        * Converts an Irrlicht event to a Guichan key value.
        *
        * @param keysym The Irrlicht event to convert.
        * @return A Guichan key value. -1 if no conversion took place.
        * @see Key
        */
        int convertIrrlichtEventToGuichanKeyValue(const irr::SEvent& event);

        std::queue<KeyInput> mKeyInputQueue;
        std::queue<MouseInput> mMouseInputQueue;

        irr::IrrlichtDevice* mIrrlichtDevice;
        bool mAltKeyDown;
        bool mMetaKeyDown;
    };
}

#endif // end GCN_IRRLICHTINPUT_HPP
