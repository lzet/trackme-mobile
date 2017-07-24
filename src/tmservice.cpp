#include "tmservice.h"

#include <QStandardPaths>
#include <QMutex>
#include <QEventLoop>

TMService::TMService(QObject *parent)
  : QObject(parent), previoustime(), request(),
#ifdef SAILFISH
    netmanager(this), servicesrv(this), clientsrv(this),
    location(QGeoPositionInfoSource::createDefaultSource(this)),
#else
    netmanager(parent), servicesrv(parent), clientsrv(parent),
    location(QGeoPositionInfoSource::createDefaultSource(parent)),
#endif
    queue(), settings(new Settings()), settingsonce(new Settings()),
    started(false), startedonce(false),
    fromclient(nullptr), toclient(nullptr), curdata(), run(true)
{
  previoustime = QDateTime::currentDateTimeUtc().addSecs(-settings->timeout);
#ifdef SAILFISH
  QString tosrvname = "/tmp/trackmetoservice.socket";
  QString toclnname = "/tmp/trackmetoclient.socket";
#else
  QString tosrvname = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/trackmetoservice.socket";
  QString toclnname = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/trackmetoclient.socket";
#endif

  if(!servicesrv.listen(tosrvname)) {
      QLocalServer::removeServer(tosrvname);
      servicesrv.listen(tosrvname);
  }
  if(!clientsrv.listen(toclnname)) {
      QLocalServer::removeServer(toclnname);
      clientsrv.listen(toclnname);
  }

  if(location) {
      location->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
#ifdef SAILFISH
      location->startUpdates();
#endif
  }

  request.setRawHeader("User-Agent", "trackme-harbour");
  request.setRawHeader("X-Custom-User-Agent", "TrackMeApp");
  request.setRawHeader("Content-Type", "application/json");
}

void TMService::mainloop()
{
  long long starttime = QDateTime::currentMSecsSinceEpoch();
  while(run && servicesrv.isListening()) {
    queueprocessing();
    int timeout = settings->timeout*1000;
    fromclient = pendingsocket(fromclient, servicesrv);
    toclient = pendingsocket(toclient, clientsrv);
    if(!run) return;
    if(fromclient) {
      if(fromclient->waitForReadyRead(MINWAITREAD) && fromclient->canReadLine()) {
        oncommand(fromclient->readLine());
        getstate();
      }
#ifdef SAILFISH
      if(started || startedonce) {
        location->setUpdateInterval(startedonce?MINIMALINTERVAL:timeout);
      }
#endif
#ifndef SAILFISH
      if(started || startedonce)
        location->requestUpdate(startedonce?MINIMALINTERVAL:timeout);
#endif
    }
    long long endtime = QDateTime::currentMSecsSinceEpoch();
    long long difftime = static_cast<int>(endtime-starttime);
    if(!run) return;
    if(MINIMALINTERVAL - difftime > 0)
      sleep(static_cast<int>(MINIMALINTERVAL - difftime));
    if(!started && !startedonce)
      continue;
    if(!run) return;
    endtime = QDateTime::currentMSecsSinceEpoch();
    if(startedonce) {
      positionupdated(location->lastKnownPosition(), startedonce);
      startedonce = false;
#ifdef SAILFISH
      location->setUpdateInterval(startedonce?MINIMALINTERVAL:timeout);
#endif
      getstate();
    }
    if(!run) return;
    if(started && (endtime-starttime) >= timeout) {
      starttime = QDateTime::currentMSecsSinceEpoch();
      positionupdated(location->lastKnownPosition());
    }
  }
}

void TMService::stop() {
  servicesrv.close();
  clientsrv.close();
  run = false;
#ifdef SAILFISH
  location->stopUpdates();
#endif
}

void TMService::sleep(int msec)
{
  if(msec <= 0)
    return;
  QMutex mut;
  mut.lock();
  mut.tryLock(msec);
  mut.unlock();
}

QLocalSocket* TMService::pendingsocket(QLocalSocket *client, QLocalServer &srv)
{
  if(client == nullptr || !client->isValid()) {
    if(srv.waitForNewConnection(MINWAITREAD))
      client = srv.nextPendingConnection();
  }
  if(client && !client->isValid())
    client = nullptr;
  return client;
}

void TMService::positionupdated(const QGeoPositionInfo &pos, bool startonce)
{
  QString res;
  static QGeoPositionInfo lastpos;
  if(!startonce && (pos == lastpos || pos.coordinate().distanceTo(lastpos.coordinate()) < MINVALIDDISTANCE)) {
    /*if(toclient) {
      toclient->write((QString("W:Repeat ")+(startonce ? "true\n" : "false\n")).toLatin1());
      toclient->waitForBytesWritten(MINWAITREAD);
    }*/
    return;
  }
  lastpos = pos;
  if(pos.isValid()) {
    QGeoCoordinate geoCoordinate = pos.coordinate();
    qreal latitude = geoCoordinate.latitude();
    qreal longitude = geoCoordinate.longitude();
    qreal altitude = geoCoordinate.altitude();
    qreal speed = pos.hasAttribute(QGeoPositionInfo::GroundSpeed) ? pos.attribute(QGeoPositionInfo::GroundSpeed) : 0;
    qreal direction = pos.hasAttribute(QGeoPositionInfo::Direction) ? pos.attribute(QGeoPositionInfo::Direction) : 0;
    const QString &dt = pos.timestamp().toString(Qt::ISODate);
    const QString &dtlog = QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate);
    res = QString("I:[%1] %2:%3/%4/%5/%6 %7").arg(queue.size()+curdata.size())
            .arg(latitude)
            .arg(longitude)
            .arg(altitude)
            .arg(speed)
            .arg(direction)
            .arg(dtlog);
    if(latitude == qQNaN())
        latitude = 0;
    queue.push_back(GpsQueueItem(startonce ? settingsonce : settings, latitude, longitude, altitude, speed, direction, dt));
  } else {
    res = "E:NOVALID";
  }
  res.append('\n');
  if(toclient) {
    toclient->write(res.toLatin1());
    toclient->waitForBytesWritten(MINWAITREAD);
  }
}

void TMService::queueprocessing()
{
  if(queue.empty() && curdata.size() == 0)
    return;
  int cap = curdata.capacity();
  if(cap > 0) {
    auto it = queue.begin();
    for(; it < queue.end(); ++it) {
      if(!curdata.validate(*it))
        break;
    }
    int size = it - queue.begin();
    if(size > 0) {
      if(size > cap) {
        size = cap;
      }
      if(curdata.adddata(queue.mid(0, size)))
        queue.erase(queue.begin(), queue.begin()+size);
    }
  }
  if(curdata.size() > 0) {
    QByteArray postData = curdata.result();
    QByteArray postDataSize = QByteArray::number(postData.size());
    request.setUrl(QUrl(curdata.geturl()));
    request.setRawHeader("Content-Length", postDataSize);
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(&netmanager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(MINNETTIMEOUTMSEC);
    QNetworkReply *reply = netmanager.post(request, postData);
    loop.exec();
    netfinished(reply, &timer);
  }
}
void TMService::netfinished(QNetworkReply *rep, QTimer *timer)
{
  QString log;
  if(timer->isActive() && rep->isFinished()) {
    QString answer(rep->readAll());
    int index = answer.indexOf("status\":");
    if(index < 0) {
      log = QString("E:Server unknown error\n");
    } else {
      index += 9;
      QString err;
      int index2 = answer.indexOf("\"", index);
      if(index2 < 0) {
        err = answer.mid(index);
      } else {
        err = answer.mid(index, index2-index);
      }
      if(err != "OK")
        log = QString("E:Server: %1\n").arg(err);
    }
    curdata.clear();
  } else {
    if(timer->isActive()) {
      log = QString("E:%1\n").arg(rep->errorString());
    } else {
      log = QString("E:Server not responding\n");
      if(!started && !startedonce) {
        queue.clear();
        curdata.clear();
      }
    }
  }
  if(!log.isEmpty() && toclient) {
    toclient->write(log.toUtf8());
    toclient->waitForBytesWritten(MINWAITREAD);
  }
  rep->deleteLater();
}

void TMService::oncommand(const QString &cmd)
{
  if(cmd.startsWith("start")) {
    QStringList params = cmd.split(DELIMITER);
    if(params[0] == "start")
      started = !started;
    else if(params[0] == "startonce")
      startedonce = true;
    else
      return;
    if(!started && !startedonce) {
      queue.clear();
      curdata.clear();
      return;
    }
    if(params.size() < 6)
      return;
    auto val = QSharedPointer<Settings>(new Settings(
                            params[1],//login
                            params[2],//password
                            params[3],//url
                            params[5].trimmed(),//trackname
                            params[4].toInt(),//timeout
                            QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch()//sessionid
                           ));
    if(startedonce)
      settingsonce = val;
    else
      settings = val;
  }
}

void TMService::getstate()
{
  if(toclient) {
    QString state("S:");
    state.append(started ? "1" : "0");
    state.append("|");
    state.append(startedonce ? "1" : "0");
    state.append("\n");
    toclient->write(state.toLatin1());
    toclient->waitForBytesWritten(MINWAITREAD);
    return;
  }
}
