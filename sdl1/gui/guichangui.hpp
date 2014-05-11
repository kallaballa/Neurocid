#ifndef GUI_CHAN_GUI_HPP_
#define GUI_CHAN_GUI_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/contrib/sdl/sdltruetypefont.hpp>
#include "gui.hpp"
#include "screen.hpp"
#include <cassert>
#include <map>

namespace neurocid {

enum ScreenID {
	OSD,
	HELP
};

class GuiChanGui : public gcn::Gui, public neurocid::Gui {
	gcn::SDLInput* input_;
	gcn::SDLGraphics* graphics_;
	gcn::SDLImageLoader* imageLoader_;
	gcn::contrib::SDLTrueTypeFont* font_;
	std::map<ScreenID, Screen*> screenMap_;
public:
	GuiChanGui(SDL_Surface* surface);

	~GuiChanGui() {
		delete input_;
		delete graphics_;
		delete imageLoader_;
		delete font_;
		TTF_Quit();
	}

	void setScreen(ScreenID id);
	void pushEvent(SDL_Event event);
	virtual void update(BattleField& field) override;

	virtual void draw() override {
			gcn::Gui::draw();
	}
	virtual void logic () override {
			gcn::Gui::logic();
	}
};

} /* namespace neurocid */

#endif /* GUI_CHAN_GUI_HPP_ */
