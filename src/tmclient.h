#ifndef TMCLIENT_H
#define TMCLIENT_H

#include <QString>
#include <QLocalSocket>
#include <QSettings>
#include "settings.h"

class TMClient : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
  Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
  Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
  Q_PROPERTY(QString track READ track WRITE setTrack NOTIFY trackChanged)
  Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
  QLocalSocket toservice, toclient;
  QSettings storesettings;
  Settings settings;
  QString tosrvname, toclnname;

  void checkstored(bool forcesave = false);
  bool checkconnected();
public:
  explicit TMClient(QObject *parent = nullptr);
  QString login() const;
  void setLogin(const QString &login);
  QString password() const;
  void setPassword(const QString &password);
  QString url() const {return settings.url;}
  void setUrl(const QString &url);
  QString track() const;
  void setTrack(const QString &track);
  int timeout() const;
  void setTimeout(int tout);

signals:
  void loginChanged();
  void passwordChanged();
  void urlChanged();
  void trackChanged();
  void timeoutChanged();
public slots:
  bool command(QString);
  QString update();
};

#endif // TMCLIENT_H
