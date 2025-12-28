QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += db_watcher.py


# --------- Add MySQL Client libraries (IMPORTANT) ----------
INCLUDEPATH += "C:/Program Files/MySQL/MySQL Connector C 6.1/include"
LIBS += -L"C:/Program Files/MySQL/MySQL Connector C 6.1/lib" -llibmysql
