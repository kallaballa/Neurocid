#include "osd.hpp"
#include "theme.hpp"
#include "help.hpp"
#include "time_tracker.hpp"
#include "battlefield.hpp"
#include "renderer.hpp"
#include "guichangui.hpp"

namespace neurocid {

OsdScreenWidget::OsdScreenWidget() : Screen() {
	setOpaque(false);
	int fs = 0;
	Options& opts = *Options::getInstance();
	size_t width = opts.WINDOW_WIDTH;
	size_t height = opts.WINDOW_HEIGHT;

	setDimension(gcn::Rectangle(fs, fs, width - (fs * 2), height - (fs * 2)));
	setFrameSize(fs);

	osdStatistics_ = new OsdWidget();
    osdStatistics_->setPosition(20, 60);

    Color da = Theme::teamA;
    da.a = 150;
    osdScoreA_ = new OsdWidget();
    osdScoreA_->setPosition(20, 20);
    osdScoreA_->setVisible(true);
    osdScoreA_->setBackgroundColor(da);

    Color db = Theme::teamB;
    db.a = 150;
    osdScoreB_ = new OsdWidget();
    osdScoreB_->setPosition(140, 20);
    osdScoreB_->setVisible(true);
    osdScoreB_->setBackgroundColor(db);

    osdDeadA_ = new OsdWidget();
    osdDeadA_->setPosition(width - 260, 20);
    osdDeadA_->setVisible(true);
    osdDeadA_->setBackgroundColor(da);

    osdDeadB_ = new OsdWidget();
    osdDeadB_->setPosition(width - 130, 20);
    osdDeadB_->setVisible(true);
    osdDeadB_->setBackgroundColor(db);

    osdTracker_ = new OsdWidget();
    osdTracker_->setPosition(20, 165);
    osdTracker_->setVisible(false);

    osdStatus_ = new OsdWidget();
    osdStatus_->setPosition(20, height - 38);

    helpButton_ = new Button("Help");
    helpButton_->setDimension({width - 70,height - 40, 50, 25});
    helpButton_->addActionListener(this);

    add(osdStatistics_);
    add(osdTracker_);
    add(osdStatus_);
    add(osdScoreA_);
    add(osdScoreB_);
    add(osdDeadA_);
    add(osdDeadB_);
    add(helpButton_);
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

	osdDeadA_->clear();
	osdDeadA_->addLine("alive", std::to_string(field.teams_[0].size() - field.teams_[0].countDead()));
	osdDeadB_->clear();
	osdDeadB_->addLine("alive", std::to_string(field.teams_[1].size() - field.teams_[1].countDead()));

	osdScoreA_->clear();
	osdScoreA_->addLine("score", std::to_string(field.teams_[0].score_));
	osdScoreB_->clear();
	osdScoreB_->addLine("score", std::to_string(field.teams_[1].score_));
}

void OsdScreenWidget::action(const gcn::ActionEvent& event) {
	GuiChanGui& gui = * static_cast<GuiChanGui*>(Gui::getInstance());
	gui.setScreen(HELP);
}
} /* namespace neurocid */
