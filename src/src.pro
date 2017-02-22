include(../config.pri)

TARGET = $$CORE_LIB_NAME
TEMPLATE = lib
DESTDIR = ../lib

QT += core network
QT -= gui

DEFINES += CORE_BUILD

static {
	CONFIG += static
	DEFINES += CORE_BUILD_STATIC
}

INCLUDEPATH += ../include/QHttpEngine/ ../include/
DEPENDSPATH += ../include/QHttpEngine/ ../include/

SOURCES += qfilesystemhandler.cpp \
    qhttpbasicauth.cpp \
    qhttphandler.cpp \
    qhttpparser.cpp \
    qhttprange.cpp \
    qhttpserver.cpp \
    qhttpsocket.cpp \
    qiodevicecopier.cpp \
    qlocalauth.cpp \
    qlocalfile.cpp \
    qobjecthandler.cpp \
    qproxyhandler.cpp \
    qproxysocket.cpp

HEADERS += qfilesystemhandler_p.h \
    qhttpbasicauth_p.h \
    qhttphandler_p.h \
    qhttprange_p.h \
    qhttpserver_p.h \
    qhttpsocket_p.h \
    qiodevicecopier_p.h \
    qlocalauth_p.h \
    qlocalfile_p.h \
    QHttpEngine/qfilesystemhandler.h \
    QHttpEngine/qhttpbasicauth.h \
    QHttpEngine/qhttpengine_global.h \
    QHttpEngine/qhttphandler.h \
    QHttpEngine/qhttpmiddleware.h \
    QHttpEngine/qhttpparser.h \
    QHttpEngine/qhttprange.h \
    QHttpEngine/qhttpserver.h \
    QHttpEngine/qhttpsocket.h \
    QHttpEngine/qibytearray.h \
    QHttpEngine/qiodevicecopier.h \ 
    QHttpEngine/qlocalauth.h \
    QHttpEngine/qlocalfile.h \
    QHttpEngine/qobjecthandler.h \
    QHttpEngine/qproxyhandler.h

header.path = ../include/QHttpEngine/
header.files = QHttpEngine/*

INSTALLS += header
