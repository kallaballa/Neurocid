#include <QApplication>
#include <FelgoApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

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

  QObject *canvas = rootObjs.first()->findChild<QObject*>("nc_canvas");
  assert(canvas != nullptr);

  std::thread renderThread([=]() {
      while(!canvas->property("available").toBool()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          std::cerr << "wait" << std::endl;
      }

      QMetaObject::invokeMethod(canvas, "lineRGBA",
        Q_ARG(QVariant, 0),
        Q_ARG(QVariant, 0),
        Q_ARG(QVariant, 100),
        Q_ARG(QVariant, 100),
        Q_ARG(QVariant, 0),
        Q_ARG(QVariant, 0),
        Q_ARG(QVariant, 255),
        Q_ARG(QVariant, 255)
      );
  });

  renderThread.detach();

  return app.exec();
}
