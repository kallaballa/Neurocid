#include "gui.hpp"
#include "../util.hpp"
#include "guichan/sdl/sdlgraphics.hpp"
#include "theme.hpp"

namespace neurocid {

Gui* Gui::instance_ = NULL;

Gui::Gui(SDL_Surface* surface) : gcn::Gui() {
	imageLoader_ = new gcn::SDLImageLoader();
	gcn::Image::setImageLoader(imageLoader_);
	graphics_ = new gcn::SDLGraphics();
	graphics_->setTarget(surface);
	input_ = new gcn::SDLInput();
	 //Initialize SDL_ttf
	if( TTF_Init() == -1 ) {
		std::cerr << "Can't initialize TTF" << std::endl;
		SDL_Quit();
	}
	try {
	std::string neurocidPath = get_env("NEUROCID_PATH");

	if(neurocidPath.empty()) {
		std::cerr << "Environment variable $NEUROCID_PATH not set" << std::endl;
		SDL_Quit();
	}

	font_ = new gcn::contrib::SDLTrueTypeFont(neurocidPath + "/DejaVuSansMono-Bold.ttf",18);
	} catch(gcn::Exception& ex) {
		std::cerr << ex.getMessage() << std::endl;
		SDL_Quit();
	}
	gcn::Widget::setGlobalFont(font_);
	setGraphics(graphics_);
	setInput(input_);

	if(SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL )) {
		fprintf( stderr, "Setting keyboard repeat failed: %s\n", SDL_GetError( ) );
		SDL_Quit();
	}
}

void Gui::setScreen(gcn::Container* s) {
	setTop(s);
}

void Gui::pushEvent(SDL_Event event) {
	input_->pushInput(event);
}

void NeurocidBorder::drawFrame(gcn::Widget* w, gcn::Graphics* graphics) {
	int width = w->getWidth() + w->getFrameSize() * 2 - 1;
	int height = w->getHeight() + w->getFrameSize() * 2 - 1;

	Color inner = w->getBackgroundColor();
	Color mid = Theme::osdWidgetFrameMid;
	Color outter = Theme::osdWidgetFrameOutter;

	unsigned int i, frameSize = w->getFrameSize();
	gcn::SDLGraphics* sdlg = static_cast<gcn::SDLGraphics*>(graphics);

	sdlg->setColor(outter);
	int br = frameSize;
	for (i = 0; i < frameSize; ++i) {
		if(i > 1 && i <= 2)
			sdlg->setColor(mid);
		else if(i > 2)
			sdlg->setColor(inner);

		sdlg->drawLine(br, i, 					width - br, i);
		sdlg->drawLine(width - i, br, 			width - i,  height - br);
		sdlg->drawLine(width - br,  height - i , br, height - i);
		sdlg->drawLine(i, height - br, 			i, br);

		sdlg->fillPie(br,br,br - i, -180, -90);
		sdlg->fillPie(width - br,br,br - i, -90, 0);
		sdlg->fillPie(width - br, height - br,br - i, 0, 90);
		sdlg->fillPie(br, height - br,br - i, 90, 180);
	}
}

} /* namespace tankwar */
