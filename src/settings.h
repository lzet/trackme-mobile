#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>

#define DELIMITER "|"
#define MINIMALINTERVAL 5000
#define MINWAITREAD 3000
#define MINNETTIMEOUTMSEC 10000
#define MINVALIDDISTANCE 5

class Settings
{
public:
  QString login;
  QString password;
  QString url;
  QString pointname;
  int timeout;
  long long session;
  explicit Settings()
    : login("user"),
      password("userpassword"),
      url("http://example.com"),
      pointname("track1"),
      timeout(120),
      session(0L)
  {}
  Settings(const QString &login,
           const QString &password,
           const QString &url,
           const QString &pointname,
           int timeout,
           long long session)
    : login(login),
      password(password),
      url(url),
      pointname(pointname),
      timeout(timeout),
      session(session)
  {}
};
#endif // SETTINGS_H
