QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
        main.cpp \
        server.cpp

HEADERS += \
        server.h

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
