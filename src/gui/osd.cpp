#include "osd.hpp"
#include "../time_tracker.hpp"
#include "../battlefield.hpp"
#include "../renderer.hpp"
#include <sstream>
#include <iostream>

namespace neurocid {

using std::stringstream;

OsdWidget::OsdWidget() : NeurocidWidget<gcn::TextBox>(){
	gcn::Color c(255,150,150,120);
	gcn::Color f(255,230,230,120);
	setBackgroundColor(c);
	setBaseColor(c);
	setForegroundColor(f);
	setFrameColor(c);
	setFrameSize(2);
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

OsdScreenWidget* OsdScreenWidget::instance_ = NULL;

OsdScreenWidget::OsdScreenWidget(Sint16 width, Sint16 height) : NeurocidWidget<gcn::Container>() {
	setOpaque(false);
	int fs = 10;
	setDimension(gcn::Rectangle(fs, fs, width - (fs * 2), height - (fs * 2)));
	setFrameSize(fs);
	gcn::Color c(255,150,150,120);
	setFrameColor(c);

	osdStatistics_ = new OsdWidget();
    osdStatistics_->setPosition(10, 10);
    osdStatistics_->setFrameSize(10);

    osdTracker_ = new OsdWidget();
    osdTracker_->setPosition(10, 115);
    osdTracker_->setFrameSize(10);
    osdTracker_->setVisible(false);

    osdStatus_ = new OsdWidget();
    osdStatus_->setPosition(10, height - 43);
    osdStatus_->setBackgroundColor(c);
    osdStatus_->setForegroundColor({255,180,180,128});
    osdStatus_->setFrameSize(10);

    add(osdStatistics_);
    add(osdTracker_);
    add(osdStatus_);
    gcn::Container cont;
}

void OsdScreenWidget::update(BattleField& field) {
	const map<string, TimeInfo>& tiMap = TimeTracker::getInstance()->getMap();
	GameState& gs = *GameState::getInstance();
	if(osdStatus_->isVisible()) {
		osdStatus_->clear();
		string status;
		if(gs.isSlow())
			status = "Real time";
		else if(gs.isSlower())
			status = "Slow";
		else
			status = "Fast";

		if(!Renderer::getInstance()->isEnabled())
			status = "Game and video rendering disabled";

		if(gs.isPaused())
			status = "Paused";

		osdStatus_->addLine(status);
	}

	if(osdStatistics_->isVisible()) {
		osdStatistics_->clear();
		osdStatistics_->addLine("Score", "  " + std::to_string(field.teams_[0].score_) + " : " + std::to_string(field.teams_[1].score_));
		osdStatistics_->addLine("Gen", "    " + std::to_string(field.teams_[0].stats_.generationCnt_) + " / " + std::to_string(field.teams_[1].stats_.generationCnt_));
		osdStatistics_->addLine("Best", "   " + std::to_string(field.teams_[0].stats_.bestFitness_) + " / " + std::to_string(field.teams_[1].stats_.bestFitness_));
		osdStatistics_->addLine("Avg", "    " + std::to_string(field.teams_[0].stats_.averageFitness_) + " / " + std::to_string(field.teams_[1].stats_.averageFitness_));

		string name = "game";
		auto it = tiMap.find(name);
		if(it != tiMap.end()) {
			double gamesPerSecond = 0;
			if((*it).second.last > 0)
				gamesPerSecond = 1000000.0 / (*it).second.last;

			osdStatistics_->addLine("Games/s", std::to_string(gamesPerSecond));
		}
	}

	if(osdTracker_->isVisible()) {
		osdTracker_->clear();
		for(auto it : tiMap) {
			osdTracker_->addLine(it.first, it.second.str());
			for(auto itc : it.second.children_) {
				osdTracker_->addLine(("  "  + itc.first), itc.second.str());
			}
		}
	}
}
} /* namespace tankwar */
