#include "guichangui.hpp"
#include "util.hpp"
#include "guichan/sdl/sdlgraphics.hpp"
#include "../theme.hpp"
#include "screen.hpp"
#include "osd.hpp"
#include "help.hpp"
#include <thread>

namespace neurocid {

GuiChanGui::GuiChanGui(SDL_Surface* surface) : gcn::Gui() {
	imageLoader_ = new gcn::SDLImageLoader();
	gcn::Image::setImageLoader(imageLoader_);
	graphics_ = new gcn::SDLGraphics();

	graphics_->setTarget(surface);
	input_ = new gcn::SDLInput();
	 //Initialize SDL_ttf
	if( TTF_Init() == -1 ) {
		std::cerr << "Can't initialize TTF" << std::endl;
		exit(1);
	}
	try {
	std::string neurocidPath = get_env("NEUROCID_PATH");
#ifndef _JAVASCRIPT
	if(neurocidPath.empty()) {
		std::cerr << "Environment variable $NEUROCID_PATH not set" << std::endl;
		exit(1);
	}
#endif
	font_ = new gcn::contrib::SDLTrueTypeFont(neurocidPath + "/DejaVuSansMono-Bold.ttf",18);
	} catch(gcn::Exception& ex) {
		std::cerr << ex.getMessage() << std::endl;
		exit(1);
	}
	gcn::Widget::setGlobalFont(font_);
	setGraphics(graphics_);
	setInput(input_);

	if(SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL )) {
		fprintf( stderr, "Setting keyboard repeat failed: %s\n", SDL_GetError( ) );
		exit(1);
	}

	screenMap_[OSD] = new OsdScreenWidget();
	screenMap_[HELP] = new HelpScreen();
	setScreen(OSD);
}

void GuiChanGui::setScreen(ScreenID id) {
	Screen* last = static_cast<Screen*>(getTop());
	setTop(screenMap_[id]);
	Screen* current = static_cast<Screen*>(getTop());

	if(current != NULL && current->isForm()) {
		Form* form = static_cast<Form*>(getTop());
		std::thread t([=]() {
			form->query();
			if(last != NULL)
				setTop(last);
		});
		t.detach();
	}
}

void GuiChanGui::pushEvent(SDL_Event event) {
	input_->pushInput(event);
}

void GuiChanGui::update(BattleField& field) {
	static_cast<Screen*>(getTop())->update(field);
}

} /* namespace tankwar */
