QT       += core gui
DESTDIR = ../app_only/

QMAKE_CXXFLAGS += /
utf-8

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

FORMS += \
    widget.ui

RC_ICONS = files/general.ico
win32:RC_FILE += files/rc.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

static {
 CONFIG+= static
 CONFIG += staticlib
 DEFINES+= STATIC
 message("____static mode____")
 mac: TARGET = $$join(TARGET,,,_static)
 win32: TARGET = $$join(TARGET,,,_static)
}

win32:QMAKE_POST_LINK += windeployqt $$OUT_PWD/$$DESTDIR
