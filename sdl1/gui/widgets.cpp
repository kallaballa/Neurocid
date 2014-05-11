#include "widgets.hpp"
#include "../theme.hpp"
#include <sstream>
#include <iostream>
#include <guichan/sdl/sdlgraphics.hpp>
namespace neurocid {

using std::stringstream;

void NeurocidBorder::drawFrame(gcn::Widget* w, gcn::Graphics* graphics) {
	int width = w->getWidth() + w->getFrameSize() * 2 - 1;
	int height = w->getHeight() + w->getFrameSize() * 2 - 1;

	Color inner = w->getBackgroundColor();
	Color mid = Theme::osdWidgetFrameMid;
	Color outter = Theme::osdWidgetFrameOutter;

	unsigned int i, frameSize = w->getFrameSize();
	gcn::SDLGraphics* sdlg = static_cast<gcn::SDLGraphics*>(graphics);

	int br = frameSize;
	for (i = 0; i < frameSize; ++i) {
		if(i == 0)
			sdlg->setColor(outter);
		else if(i <= 2)
			sdlg->setColor(mid);
		else
			sdlg->setColor(inner);

		sdlg->drawLine(br, i, 					width - br, i);
		sdlg->drawLine(i, height - br, 			i, br);

		sdlg->fillPie(br,br,br - i, -180, -90);
		sdlg->fillPie(br, height - br,br - i, 90, 180);
		if(bevel_) {
			if(i == 0)
				sdlg->setColor(outter);
			else if(i <= 2)
				sdlg->setColor(inner);
			else
				sdlg->setColor(mid);
		}
		sdlg->drawLine(width - i, br, 			width - i,  height - br);
		sdlg->drawLine(width - br,  height - i , br, height - i);

		sdlg->fillPie(width - br,br,br - i, -90, 0);
		sdlg->fillPie(width - br, height - br,br - i, 0, 90);
	}
}

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
