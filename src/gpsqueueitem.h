#ifndef GPSQUEUEITEM_H
#define GPSQUEUEITEM_H
#include <QString>
#include <QSharedPointer>
#include "settings.h"

class GpsQueueItem {
public:
  QSharedPointer<Settings> settings;
  double latitude;
  double longitude;
  double altitude;
  double speed;
  double direction;
  QString date;
  explicit GpsQueueItem(QSharedPointer<Settings> set,
                        double lat, double lon,
                        double alt, double spd, double dir,
                        const QString &dt):
    settings(set),
    latitude(lat),
    longitude(lon),
    altitude(alt),
    speed(spd),
    direction(dir),
    date(dt)
  {}
};

#endif // GPSQUEUEITEM_H
