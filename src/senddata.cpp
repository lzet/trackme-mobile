#include "senddata.h"

bool SendData::adddata(const QList<GpsQueueItem> &import)
{
  if(import.empty())
    return true;
  auto first = import.begin()->settings;
  if(value.empty()) {
    value["method"] = "addPoint";
    value["login"] = first->login;
    value["password"] = first->password;
    login = first->login;
    passwd = first->password;
    url = first->url;
  }
  if(login != first->login || passwd != first->password || url != first->url)
    return false;
  QJsonArray points;
  auto pointsit = value.find("points");
  if(pointsit != value.end())
     points = pointsit->toArray();
  if((points.size()+import.size()) > MAXDATASIZE)
    return false;
  for(const auto &item: import) {
    QJsonObject point;
    point["lat"] = item.latitude;
    point["lon"] = item.longitude;
    point["alt"] = item.altitude;
    point["speed"] = item.speed;
    point["dir"] = item.direction;
    point["name"] = item.settings->pointname;
    point["session"] = item.settings->session;
    point["datetime"] = item.date;
    points.append(point);
  }
  value["points"] = points;
  return true;
}

void SendData::clear()
{
  while (!value.empty())
    value.erase(value.begin());
}

QByteArray SendData::result() const
{
  return QJsonDocument(value).toJson(QJsonDocument::Compact);
}

int SendData::capacity() const
{
  int r = MAXDATASIZE - size();
  return r < 0 ? 0 : r;
}

int SendData::size() const
{
  auto pointsit = value.find("points");
  if(pointsit != value.end()) {
    return pointsit->toArray().size();
  }
  return 0;
}

bool SendData::validate(const GpsQueueItem &item) const
{
  if(login.isEmpty() && passwd.isEmpty() && url.isEmpty())
    return true;
  if(size() == 0)
    return true;
  return  login == item.settings->login &&
          passwd == item.settings->password &&
          url == item.settings->url;
}
