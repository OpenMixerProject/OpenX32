QT += widgets

CONFIG += c++11 release
CONFIG -= app_bundle

TEMPLATE = app
TARGET = qtbench

SOURCES += \
    main.cpp \
    benchmarkwidget.cpp

HEADERS += \
    benchmarkwidget.h
