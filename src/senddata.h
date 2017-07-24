#ifndef SENDDATA_H
#define SENDDATA_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "gpsqueueitem.h"
#define MAXDATASIZE 10

class SendData
{
  QJsonObject value;
  QString login;
  QString passwd;
  QString url;
public:
  SendData(): value(), login(), passwd(), url(){}
  bool adddata(const QList<GpsQueueItem> &import);
  void clear();
  QByteArray result() const;
  int capacity() const;
  int size() const;
  bool validate(const GpsQueueItem &item) const;
  QString geturl() const {return url;}
};

#endif // SENDDATA_H
