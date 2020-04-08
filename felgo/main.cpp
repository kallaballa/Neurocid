#include <QApplication>
#include <FelgoApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <boost/program_options.hpp>

#include "../src/population.hpp"
#include "../src/frontend.hpp"
#include "../src/procedural_scenarios.hpp"
#include "../src/json_scenario.hpp"
#include "../src/2d.hpp"
#include "../src/error.hpp"
#include "felgocanvas.hpp"
#include "../src/canvas.hpp"

#include "qneurocidcontrol.hpp"

namespace nc = neurocid;
namespace po = boost::program_options;

using std::string;
using std::cerr;
using std::endl;
using std::vector;

static QNeurocidControl NC_CONTROL;

struct CommandLineOptions {
  string loadAFile_;
  string saveAFile_;
  string loadBFile_;
  string saveBFile_;
  string captureFile_;
  string brainDumpFileA_;
  string brainDumpFileB_;
  string scenarioName_;
  size_t gameIterations_ = 1000;
  size_t width_ = 800;
  size_t height_ = 800;
  size_t frameRate_ = 25;
  long autosaveInterval_ = 0;

  int parse(const int argc, const char* const argv[]) {
    po::options_description genericDesc("Options");
    genericDesc.add_options()
        ("iterations,i", po::value<size_t>(&this->gameIterations_), "Run n iterations of the game")
        ("autosave,a", po::value<long>(&this->autosaveInterval_), "Automatically save teams every n minutes")
        ("loadA", po::value<string>(&this->loadAFile_), "Load the population as team A from a file before running the scenario")
        ("loadB", po::value<string>(&this->loadBFile_), "Load the population as team B from a file before running the scenario")
        ("saveA", po::value<string>(&this->saveAFile_), "Save the team A population to a file after running the scenario")
        ("saveB", po::value<string>(&this->saveBFile_), "Save the team B population to a file after running the scenario")
        ("capture,c", po::value<string>(&this->captureFile_), "Capture the game to a video file")
        ("brainDumpA", po::value<string>(&this->brainDumpFileA_), "Dump the neural network output of team A to an AU (audio) file.")
        ("brainDumpB", po::value<string>(&this->brainDumpFileB_), "Dump the neural network output of team B to an AU (audio) file.")
        ("width,x", po::value<size_t>(&this->width_), "The window width")
        ("height,y", po::value<size_t>(&this->height_), "The window height")
        ("framerate,f", po::value<size_t>(&this->frameRate_), "The frame rate of the renderer and video encoder")
        ("help,h", "Produce help message");

    po::options_description hidden("Hidden options");
    hidden.add_options()("scenario", po::value<string>(&this->scenarioName_), "scenario");

    po::options_description cmdline_options;
    cmdline_options.add(genericDesc).add(hidden);

    po::positional_options_description p;
    p.add("scenario", -1);

    po::options_description visible;
    visible.add(genericDesc);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cerr << "Usage: felgo-neurocid [options]\n";
      std::cerr << visible;
      return 0;
    }
    return 0;
  }
};

void run_felgo_neurocid(const CommandLineOptions& opts, QObject *qmlGfxCanvas) {
	srand(time(0));
	  /* Let's load the scenario.
	   * Either we load a hard coded (procedural) scenario
	   * or if a filename is given we load a json scenario
	   */

	  //initialize frontend subsystems
	  nc::init_core(opts.width_, opts.height_, opts.frameRate_);

	  string suffix = ".nsj";
	  nc::Scenario* scenario = NULL;

	  if (opts.scenarioName_.length() > suffix.length() && std::equal(suffix.rbegin(), suffix.rend(), opts.scenarioName_.rbegin())) {
	    scenario = new nc::json::JsonScenario(opts.scenarioName_);
	  } else {
	    // this has to be done once before trying to access procedural scenarios.
	    nc::load_procedural_scenarios();
	    // get a declarative scenario by name
	    scenario = nc::get_procedural_scenario(opts.scenarioName_);

	    CHECK_MSG(scenario != NULL, "Unknown Scenario");
	  }
	  CHECK_MSG(scenario != NULL, "Unable to load a Scenario");
	  nc::FelgoGFX* felgoGfx = new nc::FelgoGFX(qmlGfxCanvas);
	  nc::FelgoCanvas* felgoCanvas = new nc::FelgoCanvas(felgoGfx, opts.width_, opts.height_, scenario->bfl_);
	  nc::Canvas* canvas = dynamic_cast<nc::Canvas*>(felgoCanvas);
	  CHECK_MSG(canvas != nullptr, "Can't cast FelgoCanvas to Canvas!")

	  //initialize rendering
		nc::init_canvas(canvas);

	  //make default layouts
	  nc::PopulationLayout pl = nc::make_default_population_layout();
	  nc::GeneticLayout gl = nc::make_default_genetic_layout();

	  //make default genetic pools
	  vector<nc::GeneticPool> pools = nc::make_pools(2, gl);

	  vector<nc::Population> teams(2);

	  //either load or create a team
	  if (!opts.loadAFile_.empty()) {
	    ifstream is(opts.loadAFile_);
	    nc::read_population(0, teams[0], is);
	  } else {
	    teams[0] = nc::make_population(0, pl);
	  }

	  if (!opts.loadBFile_.empty()) {
	    ifstream is(opts.loadBFile_);
	    nc::read_population(1, teams[1], is);
	  } else {
	    teams[1] = nc::make_population(1, pl);
	  }


	  std::thread gameThread([&]() {
		      teams[0].score_ = 0;
	      teams[1].score_ = 0;

	      //play the game!
	      neurocid::play_game(opts.gameIterations_, scenario, teams, pools, opts.captureFile_, opts.autosaveInterval_);

	      //save the result if requested
	      if(!opts.saveAFile_.empty()) {
	        ofstream os(opts.saveAFile_);
	        write_population(teams[0],os);
	      }

	      if(!opts.saveBFile_.empty()) {
	        ofstream os(opts.saveBFile_);
	        write_population(teams[1],os);
	      }
	    });


	  while (neurocid::is_running()) {
	    neurocid::render();
	  }

	  gameThread.join();
	  nc::quit();
}

int main(int argc, char *argv[]) {
		CommandLineOptions opts;
		opts.parse(argc, argv);

		QApplication app(argc, argv);
    FelgoApplication felgo;
    QQmlApplicationEngine engine;
    felgo.initialize(&engine);
    felgo.setMainQmlFileName(QStringLiteral("qml/Main.qml"));
    engine.load(QUrl(felgo.mainQmlFileName()));

    QList<QObject*> rootObjs = engine.rootObjects();
    assert(rootObjs.size() == 1);

    QObject *qmlGfxCanvas = rootObjs.first()->findChild<QObject*>("gfxCanvas");
    QObject *qmlApp = rootObjs.first();
    assert(qmlGfxCanvas != nullptr);
    assert(qmlApp != nullptr);

    QObject::connect(qmlApp, SIGNAL(zoomIn()),
                     &NC_CONTROL, SLOT(zoomIn()));
    QObject::connect(qmlApp, SIGNAL(zoomOut()),
                     &NC_CONTROL, SLOT(zoomOut()));
    QObject::connect(qmlApp, SIGNAL(left()),
                     &NC_CONTROL, SLOT(left()));
    QObject::connect(qmlApp, SIGNAL(right()),
                     &NC_CONTROL, SLOT(right()));
    QObject::connect(qmlApp, SIGNAL(up()),
                     &NC_CONTROL, SLOT(up()));
    QObject::connect(qmlApp, SIGNAL(down()),
                     &NC_CONTROL, SLOT(down()));
    QObject::connect(qmlApp, SIGNAL(tiltUp()),
                     &NC_CONTROL, SLOT(tiltUp()));
    QObject::connect(qmlApp, SIGNAL(tiltDown()),
                     &NC_CONTROL, SLOT(tiltDown()));
    QObject::connect(qmlApp, SIGNAL(resizeCanvasBackend(int,int)),
                     &NC_CONTROL, SLOT(resizeCanvasBackend(int,int)));
    QObject::connect(qmlApp, SIGNAL(setSpeed(int)),
                     &NC_CONTROL, SLOT(setSpeed(int)));
    QObject::connect(qmlApp, SIGNAL(setPaused(bool)),
                     &NC_CONTROL, SLOT(setPaused(bool)));
    QObject::connect(qmlApp, SIGNAL(dumpTeams()),
                     &NC_CONTROL, SLOT(dumpTeams()));

    std::thread neurocidThread([=]() {
        while(!qmlGfxCanvas->property("available").toBool()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cerr << "wait" << std::endl;
        }

        run_felgo_neurocid(opts,qmlGfxCanvas);
    });

    neurocidThread.detach();

    return app.exec();
}
