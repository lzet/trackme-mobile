# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-trackme

CONFIG += sailfishapp c++11
QT += positioning network

DEFINES += SAILFISH

HEADERS += \
    src/tmservice.h \
    src/settings.h \
    src/tmclient.h \
    src/senddata.h \
    src/gpsqueueitem.h

SOURCES += src/harbour-trackme.cpp \
    src/tmservice.cpp \
    src/settings.cpp \
    src/tmclient.cpp \
    src/senddata.cpp \
    src/gpsqueueitem.cpp
RESOURCES += res/qml-sailfish.qrc

OTHER_FILES += \
    rpm/harbour-trackme.changes.in \
    rpm/harbour-trackme.spec \
    rpm/harbour-trackme.yaml \
    translations/*.ts \
    harbour-trackme.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
# TRANSLATIONS += translations/harbour-trackme-de.ts
