#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QString>
#include <QQmlContext>
#include "tmservice.h"
#include "tmclient.h"

int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  if (QCoreApplication::arguments().count() > 1){
    TMService cppengine(&app);
    cppengine.mainloop();
  } else {
    qmlRegisterType<TMClient>("TMClient",1,0,"TMClient");
    engine.load(QUrl(QLatin1String("qrc:/main-android.qml")));
    if (engine.rootObjects().isEmpty())
      return -1;
  }
  return app.exec();
}
