#ifndef ENGINE_H
#define ENGINE_H

#include <QDateTime>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoPositionInfoSource>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedPointer>
#include <QTimer>

#include "settings.h"
#include "gpsqueueitem.h"
#include "senddata.h"

class TMService : public QObject
{
  Q_OBJECT
  QDateTime previoustime;
  QNetworkRequest request;
  QNetworkAccessManager netmanager;
  QLocalServer servicesrv, clientsrv;
  QGeoPositionInfoSource* location;
  QList<GpsQueueItem> queue;
  QSharedPointer<Settings> settings, settingsonce;
  bool started, startedonce;
  QLocalSocket *fromclient, *toclient;
  SendData curdata;
  bool run;
public:
  explicit TMService(QObject *parent = nullptr);
  void mainloop();
  void stop();
private:
  void sleep(int msec);
  QLocalSocket* pendingsocket(QLocalSocket*, QLocalServer&);
  void positionupdated(const QGeoPositionInfo &pos, bool startonce = false);
  void queueprocessing();
  void oncommand(const QString &);
  void getstate();
public slots:
  void mainloopstart() {mainloop();}
  void netfinished(QNetworkReply*, QTimer*);
};

#endif // ENGINE_H
