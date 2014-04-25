#ifndef GUI_HPP_
#define GUI_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/contrib/sdl/sdltruetypefont.hpp>
#include <cassert>

namespace neurocid {

class Gui : public gcn::Gui {
	gcn::SDLInput* input_;
	gcn::SDLGraphics* graphics_;
	gcn::SDLImageLoader* imageLoader_;
	gcn::contrib::SDLTrueTypeFont* font_;
	static Gui* instance_;
	Gui(SDL_Surface* surface);
public:
	static void init(SDL_Surface* surface) {
		assert(instance_ == NULL);
		instance_ = new Gui(surface);
	}

	static bool isInitalized() {
		return instance_ != NULL;
	}

	static Gui* getInstance() {
		assert(instance_ != NULL);
		return instance_;
	}

	~Gui() {
		delete input_;
		delete graphics_;
		delete imageLoader_;
		delete font_;
		TTF_Quit();
	}

	void setScreen(gcn::Container* sw);
	void pushEvent(SDL_Event event);
};

class NeurocidBorder {
	gcn::Color frameColor_;
	bool bevel_ = false;
public:
	virtual ~NeurocidBorder() {};

	void setBevel(bool c) {
		bevel_ = c;
	}

	void setFrameColor(gcn::Color& c) {
		frameColor_ = c;
		frameColor_.a = c.a;
	}

	virtual void drawFrame(gcn::Widget* w, gcn::Graphics* graphics);
};


template <typename Twidget> class NeurocidWidget : public Twidget, public NeurocidBorder {
public:
	NeurocidWidget() : Twidget() {
	}

	void setFrameColor(gcn::Color c) {
		NeurocidBorder::setFrameColor(c);
	}
	virtual void drawFrame(gcn::Graphics* graphics) override {
		NeurocidBorder::drawFrame(this, graphics);
	}
};

} /* namespace neurocid */

#endif /* GUI_HPP_ */
