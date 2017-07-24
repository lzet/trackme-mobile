#include "tmclient.h"
#include <QStandardPaths>

void TMClient::checkstored(bool forcesave)
{
  if(forcesave) {
    storesettings.setValue("login",     login());
    storesettings.setValue("passwd",    password());
    storesettings.setValue("url",       url());
    storesettings.setValue("interval",  timeout());
  } else {
    setLogin(   storesettings.value("login",    login()).toString());
    setPassword(storesettings.value("passwd",   password()).toString());
    setUrl(     storesettings.value("url",      url()).toString());
    setTimeout( storesettings.value("interval", timeout()).toInt());
  }
}

bool TMClient::checkconnected()
{
  bool ret1 = true;
  if(toservice.state() == QLocalSocket::UnconnectedState) {
    toservice.connectToServer(tosrvname, QIODevice::ReadWrite);
    ret1 = toservice.waitForConnected(-1);
  }
  if(ret1 && !toservice.isOpen())
    ret1 = toservice.open(QIODevice::ReadWrite);
  bool ret2 = true;
  if(toclient.state() == QLocalSocket::UnconnectedState) {
    toclient.connectToServer(toclnname, QIODevice::ReadWrite);
    ret2 = toclient.waitForConnected(-1);
  }
  if(ret2 && !toclient.isOpen())
    ret2 = toclient.open(QIODevice::ReadWrite);
  return ret1 && ret2;
}

TMClient::TMClient(QObject *parent)
  : QObject(parent), toservice(parent), toclient(parent),
    storesettings("harbour-trackme", "trackme"), settings(),
    tosrvname(), toclnname()
{
  checkstored();
#ifdef SAILFISH
  tosrvname = "/tmp/trackmetoservice.socket";
  toclnname = "/tmp/trackmetoclient.socket";
#else
  tosrvname = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/trackmetoservice.socket";
  toclnname = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/trackmetoclient.socket";
#endif
  checkconnected();
}

QString TMClient::login() const {
  return settings.login;
}

void TMClient::setLogin(const QString &login){
  if(login != settings.login) {
    settings.login = login; emit loginChanged();
  }
}

QString TMClient::password() const {
  return settings.password;
}

void TMClient::setPassword(const QString &password) {
  if(password != settings.password) {
    settings.password = password; emit passwordChanged();
  }
}

void TMClient::setUrl(const QString &url) {
  QString checkedurl;
  if(!url.startsWith("http"))
    checkedurl = QString("http://")+url;
  else
    checkedurl = url;
  if(checkedurl != settings.url) {
    settings.url = checkedurl; emit urlChanged();
  }
}

QString TMClient::track() const {
  return settings.pointname;
}

void TMClient::setTrack(const QString &track) {
  if(track != settings.pointname) {
    settings.pointname = track; emit trackChanged();
  }
}

int TMClient::timeout() const {
  return settings.timeout;
}

void TMClient::setTimeout(int tout) {
  if(tout != settings.timeout) {
    settings.timeout = tout; emit timeoutChanged();
  }
}

bool TMClient::command(QString str)
{
  checkstored(true);
  QString command = str.append(DELIMITER).append(login().toUtf8().toBase64()).
                        append(DELIMITER).append(password().toUtf8().toBase64()).
                        append(DELIMITER).append(url()).
                        append(DELIMITER).append(QString::number(timeout())).
                        append(DELIMITER).append(track().toUtf8().toBase64()).append("\n");
  toservice.write(command.toLatin1());
  bool ret = toservice.waitForBytesWritten(MINWAITREAD);
  return ret;
}

QString TMClient::update()
{
  return toclient.readLine();
}
