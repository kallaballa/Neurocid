#include "widgets.hpp"
#include "theme.hpp"
#include <sstream>
#include <iostream>

namespace neurocid {

using std::stringstream;

OsdWidget::OsdWidget() : NeurocidWidget<gcn::TextBox>(){
	gcn::Color c = Theme::osdWidgetBg;
	gcn::Color f = Theme::osdWidgetFg;
	setBackgroundColor(c);
	setBaseColor(c);
	setForegroundColor(f);
	setFrameColor(c);
	setFrameSize(5);
	setEditable(false);
}

void OsdWidget::clear() {
	gcn::TextBox::setText("");
}

void OsdWidget::addLine(const string& key, const string& value) {
	stringstream ss;
	string text = gcn::TextBox::getText();
	if(!text.empty())
		ss <<  text << std::endl;
	ss << key << ": " << value;
	gcn::TextBox::setText(ss.str());
}

void OsdWidget::addLine(const string& value) {
	stringstream ss;
	string text = gcn::TextBox::getText();
	if(!text.empty())
		ss <<  text << std::endl;
	ss << value;
	gcn::TextBox::setText(ss.str());
}

Button::Button(const string& s) : NeurocidWidget<gcn::Button>() {
	setCaption(s);
	gcn::Color c = Theme::buttonBg;
	gcn::Color f = Theme::buttonFg;
	setBackgroundColor(c);
	setBaseColor(c);
	setForegroundColor(f);
	setFrameColor(c);
	setFrameSize(5);
}
} /* namespace neurocid */
