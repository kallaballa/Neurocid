#include <QApplication>
#include <FelgoApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  FelgoApplication felgo;
  QQmlApplicationEngine engine;
  felgo.initialize(&engine);
  felgo.setMainQmlFileName(QStringLiteral("qml/Main.qml"));
  engine.load(QUrl(felgo.mainQmlFileName()));


  return app.exec();
}
