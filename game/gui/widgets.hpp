#ifndef WIDGETS_HPP_
#define WIDGETS_HPP_

#include "gui.hpp"
#include <string>

namespace neurocid {

using std::string;

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

class Container : public NeurocidWidget<gcn::Container> {
public:
};

class OsdWidget : public NeurocidWidget<gcn::TextBox> {
public:
	OsdWidget();
	void clear();
	void addLine(const string& key, const string& value);
	void addLine(const string& value);
};

class Button : public NeurocidWidget<gcn::Button> {
public:
	Button(const string& s);
	virtual void draw(gcn::Graphics* graphics) override;
};

class Label : public NeurocidWidget<gcn::Label> {
public:
	Label(const string& s);
};

}
#endif /* WIDGETS_HPP_ */
