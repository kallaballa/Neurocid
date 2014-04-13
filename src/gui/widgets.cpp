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
	//setBevel(true);
	gcn::Color c = Theme::buttonBg;
	gcn::Color f = Theme::buttonFg;
	setBackgroundColor(c);
	setBaseColor(c);
	setForegroundColor(f);
	setFrameColor(c);
	setFrameSize(5);
}

void Button::draw(gcn::Graphics* graphics) {
       Color faceColor = getBaseColor();
       Color bgColor = getBackgroundColor();
       int alpha = getBaseColor().a;

       if (isPressed())
       {
           faceColor = faceColor - 0x303030;
           faceColor.a = alpha;
       }

       size_t w = getWidth();
       size_t h = getHeight();

       for(size_t i = 0; i < h; i++) {
    	   bgColor = bgColor - 0x050505;
    	   graphics->setColor(bgColor);
    	   graphics->drawLine(0, i, w, i);
       }

       graphics->setColor(getForegroundColor());

       int textX;
       int textY = getHeight() / 2 - getFont()->getHeight() / 2;

       switch (getAlignment())
       {
         case gcn::Graphics::Left:
             textX = mSpacing;
             break;
         case gcn::Graphics::Center:
             textX = getWidth() / 2;
             break;
         case gcn::Graphics::Right:
             textX = getWidth() - mSpacing;
             break;
         default:
             throw GCN_EXCEPTION("Unknown alignment.");
       }

       graphics->setFont(getFont());

       if (isPressed())
       {
           graphics->drawText(getCaption(), textX + 1, textY + 1, getAlignment());
       }
       else
       {
           graphics->drawText(getCaption(), textX, textY, getAlignment());

           if (isFocused())
           {
               graphics->drawRectangle(2, 2, getWidth() - 4,
                                       getHeight() - 4);
           }
       }
   }

Label::Label(const string& s) : NeurocidWidget<gcn::Label>() {
	setCaption(s);
	//setBevel(true);
	gcn::Color c = Theme::labelBg;
	gcn::Color f = Theme::labelFg;
	setBackgroundColor(c);
	setBaseColor(c);
	setForegroundColor(f);
	setFrameColor(c);
	setFrameSize(5);
}
} /* namespace neurocid */
