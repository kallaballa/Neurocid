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
#include "../sdl1/procedural_scenarios.hpp"
#include "../src/2d.hpp"
#include "../src/error.hpp"
#include "felgocanvas.hpp"
#include "../src/canvas.hpp"
#include "../sdl1/json_scenario.hpp"
#include "qneurocidcontrol.hpp"

namespace po = boost::program_options;
namespace nc = neurocid;

using std::string;
using std::cerr;
using std::endl;
using std::vector;

QNeurocidControl NC_CONTROL;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FelgoApplication felgo;
    QQmlApplicationEngine engine;
    felgo.initialize(&engine);
    felgo.setMainQmlFileName(QStringLiteral("qml/Main.qml"));
    engine.load(QUrl(felgo.mainQmlFileName()));

    QList<QObject*> rootObjs = engine.rootObjects();
    assert(rootObjs.size() == 1);

    QObject *qmlCanvas = rootObjs.first()->findChild<QObject*>("nc_canvas");
    assert(qmlCanvas != nullptr);


    QObject::connect(qmlCanvas, SIGNAL(zoomIn()),
                     &NC_CONTROL, SLOT(zoomIn()));
    QObject::connect(qmlCanvas, SIGNAL(zoomOut()),
                     &NC_CONTROL, SLOT(zoomOut()));
    QObject::connect(qmlCanvas, SIGNAL(left()),
                     &NC_CONTROL, SLOT(left()));
    QObject::connect(qmlCanvas, SIGNAL(right()),
                     &NC_CONTROL, SLOT(right()));
    QObject::connect(qmlCanvas, SIGNAL(up()),
                     &NC_CONTROL, SLOT(up()));
    QObject::connect(qmlCanvas, SIGNAL(down()),
                     &NC_CONTROL, SLOT(down()));
    QObject::connect(qmlCanvas, SIGNAL(tiltUp()),
                     &NC_CONTROL, SLOT(tiltUp()));
    QObject::connect(qmlCanvas, SIGNAL(tiltDown()),
                     &NC_CONTROL, SLOT(tiltDown()));

    QObject::connect(qmlCanvas, SIGNAL(setSpeed(int)),
                     &NC_CONTROL, SLOT(setSpeed(int)));
    QObject::connect(qmlCanvas, SIGNAL(togglePauseGame()),
                     &NC_CONTROL, SLOT(togglePauseGame()));
    QObject::connect(qmlCanvas, SIGNAL(dumpTeams()),
                     &NC_CONTROL, SLOT(dumpTeams()));

    std::thread renderThread([=]() {

        while(!qmlCanvas->property("available").toBool()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cerr << "wait" << std::endl;
        }

        //command line parsing
        string loadAFile;
        string saveAFile;
        string loadBFile;
        string saveBFile;
        string brainDumpFileA;
        string brainDumpFileB;
        string scenarioName;
        size_t gameIterations = 1000;
        size_t width = 800;
        size_t height = 800;
        size_t frameRate = 25;
        long autosaveInterval = 0;

        po::options_description genericDesc("Options");
        genericDesc.add_options()
                ("iterations,i", po::value<size_t>(&gameIterations), "Run n iterations of the game")
                ("autosave,a", po::value<long>(&autosaveInterval), "Automatically save teams every n minutes")
                ("loadA", po::value<string>(&loadAFile), "Load the population as team A from a file before running the scenario")
                ("loadB", po::value<string>(&loadBFile), "Load the population as team B from a file before running the scenario")
                ("saveA", po::value<string>(&saveAFile), "Save the team A population to a file after running the scenario")
                ("saveB", po::value<string>(&saveBFile), "Save the team B population to a file after running the scenario")
                ("brainDumpA", po::value<string>(&brainDumpFileA), "Dump the neural network output of team A to an AU (audio) file.")
                ("brainDumpB", po::value<string>(&brainDumpFileB), "Dump the neural network output of team B to an AU (audio) file.")
                ("width,x", po::value<size_t>(&width), "The window width")
                ("height,y", po::value<size_t>(&height), "The window height")
                ("framerate,f", po::value<size_t>(&frameRate), "The frame rate of the renderer and video encoder")
                ("help,h", "Produce help message");

        po::options_description hidden("Hidden options");
        hidden.add_options()("scenario", po::value<string>(&scenarioName), "scenario");

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
            std::cerr << "Usage: neurocid [options]\n";
            std::cerr << visible;
            return 0;
        }

        srand(time(nullptr));
        /* Let's load the scenario.
       * Either we load a hard coded (procedural) scenario
       * or if a filename is given we load a json scenario
       */


        //initialize frontend subsystems subsystems - core, canvas, gui
        nc::init_core(width, height, frameRate);

        string suffix = ".nsj";
        nc::Scenario* scenario = nullptr;

        // this has to be done once before trying to access procedural scenarios.
        nc::load_procedural_scenarios();
        // get a procedural scenario by name
        scenario = nc::get_procedural_scenario(scenarioName);

        CHECK_MSG(scenario != nullptr, "Unable to load a Scenario")

        nc::FelgoGFX* felgoGfx = new nc::FelgoGFX(qmlCanvas);
        nc::FelgoCanvas* felgoCanvas = new nc::FelgoCanvas(felgoGfx, width, height, scenario->bfl_);
        nc::Canvas* canvas = dynamic_cast<nc::Canvas*>(felgoCanvas);
        CHECK_MSG(canvas != nullptr, "Can't cast FelgoCanvas to Canvas?!?")

        //      nc::GuiChanGui* gui = new nc::GuiChanGui(sdlc->getSurface());
        nc::init_canvas(canvas);
        //nc::init_gui(gui);


        //make default layouts
        nc::PopulationLayout pl = nc::make_default_population_layout();
        nc::GeneticLayout gl = nc::make_default_genetic_layout();

        //make default genetic pools
        vector<nc::GeneticPool> pools = nc::make_pools(2, gl);

        vector<nc::Population> teams(2);

        //either load or create a team
        if (!loadAFile.empty()) {
            ifstream is(loadAFile);
            nc::read_population(0, teams[0], is);
        } else {
            teams[0] = nc::make_population(0, pl);
        }

        if (!loadBFile.empty()) {
            ifstream is(loadBFile);
            nc::read_population(1, teams[1], is);
        } else {
            teams[1] = nc::make_population(1, pl);
        }

        std::thread gameThread([&]() {
            teams[0].score_ = 0;
            teams[1].score_ = 0;

            //play the game!
            neurocid::play_game(gameIterations, scenario, teams, pools, "", autosaveInterval);

            //save the result if requested
            if(!saveAFile.empty()) {
                ofstream os(saveAFile);
                write_population(teams[0],os);
            }

            if(!saveBFile.empty()) {
                ofstream os(saveBFile);
                write_population(teams[1],os);
            }


        });

        while (neurocid::is_running()) {
            neurocid::render();
        }

        gameThread.join();
        nc::quit();
    });

    renderThread.detach();

    return app.exec();
}
