#include "gui.hpp"

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
	font_ = new gcn::contrib::SDLTrueTypeFont("DejaVuSansMono-Bold.ttf",14);
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

	gcn::Color highlightColor = frameColor_;
	if(compound_)
		highlightColor = frameColor_ + 0x303030;

	gcn::Color shadowColor = frameColor_;
	if(compound_)
		shadowColor = frameColor_ - 0x303030;

	highlightColor.a = frameColor_.a;
	shadowColor.a = frameColor_.a;

	unsigned int i;
	for (i = 0; i < w->getFrameSize(); ++i) {
		graphics->setColor(highlightColor);
		graphics->fillRectangle(i, i, width - (i * 2) + 1, 1);
		graphics->fillRectangle(i, i + 1, 1, height - (i * 2) - 1);
		graphics->setColor(shadowColor);
		graphics->fillRectangle(width - i, i + 1, 1, height - (i * 2));
		graphics->fillRectangle(i, height - i, width - (i * 2), 1);
	}
}

} /* namespace tankwar */
