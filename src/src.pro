TEMPLATE = lib
CONFIG += plugin

QT = core network qml

TARGET = mongodb-qml
TARGETPATH = me/qtquick/MongoDB

include(./3rdparty/mongo-c-driver.pri)

HEADERS += \
    plugin.h \
    database.h \
    collection.h \
    query.h

SOURCES += \
    database.cpp \
    collection.cpp \
    query.cpp

target.path = $$[QT_INSTALL_QML]/$$TARGETPATH

qmldir.files = qmldir
qmldir.path = $$[QT_INSTALL_QML]/$$TARGETPATH

INSTALLS = target qmldir

OTHER_FILES += qmldir
