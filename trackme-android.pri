QT += qml quick positioning network androidextras

CONFIG += c++11
SOURCES += src/main.cpp \
    src/tmservice.cpp \
    src/settings.cpp \
    src/tmclient.cpp \
    src/senddata.cpp \
    src/gpsqueueitem.cpp

RESOURCES += res/qml-android.qrc

QML_IMPORT_PATH =

QML_DESIGNER_IMPORT_PATH =

DEFINES += QT_DEPRECATED_WARNINGS

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/tmservice.h \
    src/settings.h \
    src/tmclient.h \
    src/senddata.h \
    src/gpsqueueitem.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_JAVA_SOURCES.path = /src/name/lzet/trackme
ANDROID_JAVA_SOURCES.files = $$files($$PWD/src/*.java)
INSTALLS += ANDROID_JAVA_SOURCES
ANDROID_EXTRA_LIBS = \
    $$PWD/android/libs/openssl/libcrypto.so \
    $$PWD/android/libs/openssl/libssl.so
