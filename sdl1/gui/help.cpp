#include "help.hpp"
#include "../theme.hpp"
#include "form.hpp"
#include "options.hpp"

namespace neurocid {

HelpScreen::HelpScreen() : Form() {
	setOpaque(false);
	int fs = 0;
	setFrameSize(fs);
	Options& opts = *Options::getInstance();

	setDimension({int(opts.WINDOW_WIDTH/2 - 262), int(opts.WINDOW_HEIGHT/2 - 230),545,460});
	closeButton_ = new Button("Close");
	closeButton_->setDimension({235,400,70,30});
	closeButton_->setEnabled(true);
	closeButton_->setFocusable(true);
	helpText_ = new OsdWidget();
	helpText_->setPosition(10,10);

	string lines[] = {
			"Escape: quit lesson",
			"",
			"Use following hotkeys with the ALT key pressed:",
			"",
			"Space: change simulation speed",
			"P: pause game",
			"D: dump teams into a time stamped file",
			"C: draw cluster centers",
			"E: draw engines",
			"G: draw grid",
			"+: zoom in",
			"-: zoom out",
			"Left: pan left",
			"Right: pan right",
			"Up: pan up",
			"Down: pan down",
			"T: print time tracking info"
	};

	for(const string& l : lines) {
		helpText_->addLine(l);
	}

	closeButton_->addActionListener(this);
	add(helpText_);
	add(closeButton_);
}

void HelpScreen::open() {
	setVisible(true);
}

void HelpScreen::update(BattleField& field) {
}

void HelpScreen::action(const gcn::ActionEvent& event) {
	Form::submit();
	setVisible(false);
}
} /* namespace neurocid */
