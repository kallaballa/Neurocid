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

#include "population.hpp"
#include "frontend.hpp"
#include "procedural_scenarios.hpp"
#include "json_scenario.hpp"
#include "2d.hpp"
#include "error.hpp"
#include "felgocanvas.hpp"
#include "canvas.hpp"

#include "qneurocidcontrol.hpp"

namespace nc = neurocid;
namespace po = boost::program_options;

using std::string;
using std::cerr;
using std::endl;
using std::vector;

static QNeurocidControl NC_CONTROL;

/*!
 * \class CommandLineOptions
 * \brief The collection of all Neurocid options in a struct.
 */
struct CommandLineOptions {
    /*!
   * \brief The file from which to load Team A
   */
    string loadAFile_;
    /*!
  * \brief The file to save Team A in after the scenario has ended
  */
    string saveAFile_;
    /*!
  * \brief The file from which to load Team B
  */
    string loadBFile_;
    /*!
  * \brief The file to save Team B in after the scenario has ended
  */
    string saveBFile_;
    /*!
  * \brief Either a name of a scenario defined in ../src/procedural_scenarios.cpp or a json file ending with the suffix nsj
  */
    string scenarioName_;
    /*!
  * \brief The number of times the scenario should be run
  */
    size_t gameIterations_ = 1000;
    /*!
  * \brief The frame rate of the renderer
  */
    size_t frameRate_ = 25;
    /*!
  * \brief interval in minutes for autosaving
  */
    long autosaveInterval_ = 0;

    /*!
   * \brief parse the command line options and setup the above variables
   * \return zero on success.
   */
    int parse(const int argc, const char* const argv[]) {
        po::options_description genericDesc("Options");
        genericDesc.add_options()
                ("iterations,i", po::value<size_t>(&this->gameIterations_), "Run n iterations of the game")
                ("autosave,a", po::value<long>(&this->autosaveInterval_), "Automatically save teams every n minutes")
                ("loadA", po::value<string>(&this->loadAFile_), "Load the population as team A from a file before running the scenario")
                ("loadB", po::value<string>(&this->loadBFile_), "Load the population as team B from a file before running the scenario")
                ("saveA", po::value<string>(&this->saveAFile_), "Save the team A population to a file after running the scenario")
                ("saveB", po::value<string>(&this->saveBFile_), "Save the team B population to a file after running the scenario")
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

/*!
 * \brief run_felgo_neurocid
 */
void run_felgo_neurocid(const CommandLineOptions& opts, QObject *qmlGfxCanvas) {
    //seed the pseudo-random generator
    srand(static_cast<unsigned int>(time(nullptr)));

    //initialize frontend subsystems. the width and height arguments provided here are rather meaningless (but required) once the Feglo frontend takes over.
    nc::init_core(800, 600, opts.frameRate_);

    string suffix = ".nsj";
    nc::Scenario* scenario = nullptr;

    if (opts.scenarioName_.length() > suffix.length() && std::equal(suffix.rbegin(), suffix.rend(), opts.scenarioName_.rbegin())) {
        scenario = new nc::json::JsonScenario(opts.scenarioName_);
    } else {
        // this has to be done once before trying to access procedural scenarios.
        nc::load_procedural_scenarios();
        // get a declarative scenario by name
        scenario = nc::get_procedural_scenario(opts.scenarioName_);

        CHECK_MSG(scenario != nullptr, "Unknown Scenario")
    }
    CHECK_MSG(scenario != nullptr, "Unable to load a Scenario")
            nc::FelgoGFX* felgoGfx = new nc::FelgoGFX(qmlGfxCanvas);
    nc::FelgoCanvas* felgoCanvas = new nc::FelgoCanvas(felgoGfx, 800, 600, scenario->bfl_);
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
        neurocid::play_game(opts.gameIterations_, scenario, teams, pools, "", opts.autosaveInterval_);

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

    //renderer loop
    while (neurocid::is_running()) {
        neurocid::render();
    }

    gameThread.join();
    nc::quit();
}

int main(int argc, char *argv[]) {
    CommandLineOptions opts;
    opts.parse(argc, argv);

    //pretty standard initialization of the Felgo app
    QApplication app(argc, argv);
    FelgoApplication felgo;
    QQmlApplicationEngine engine;
    felgo.initialize(&engine);
    //load main qml file
    felgo.setMainQmlFileName(QStringLiteral("qml/Main.qml"));
    engine.load(QUrl(felgo.mainQmlFileName()));

    //we know there is only one root object but it's better to check anyways
    QList<QObject*> rootObjs = engine.rootObjects();
    assert(rootObjs.size() == 1);

    //find the gfxCanvas QML object
    QObject *qmlGfxCanvas = rootObjs.first()->findChild<QObject*>("gfxCanvas");
    //get the neurocidApp QML object
    QObject *qmlApp = rootObjs.first();
    assert(qmlGfxCanvas != nullptr);
    assert(qmlApp != nullptr);

    /*
     * connect the back-channel (to the neurocid backend) signals of the neurocid app
     * to the global QNeurocidControl object
     */
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

    //run neurocid in a thread
    std::thread neurocidThread([=]() {
        //wait for the GfxCanvas to become available
        while(!qmlGfxCanvas->property("available").toBool()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cerr << "wait" << std::endl;
        }

        //setup the neurocid backend according to the opts and run it.
        run_felgo_neurocid(opts,qmlGfxCanvas);
    });

    neurocidThread.detach();

    //now that everything is setup, run the felgo applications
    return app.exec();
}
