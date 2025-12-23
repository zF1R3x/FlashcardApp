TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
QT += core
QT -= gui

APP_DIR  = $$PWD/../code/FlashcardStudy
GTEST_DIR = $$PWD/third_party/googletest/googletest
GMOCK_DIR = $$PWD/third_party/googletest/googlemock

INCLUDEPATH += \
    $$APP_DIR \
    $$GTEST_DIR/include \
    $$GMOCK_DIR/include \
    $$GTEST_DIR \
    $$GMOCK_DIR

SOURCES += \
    deck_gtest.cpp \
    deck_gmock.cpp \
    $$APP_DIR/deck.cpp \
    $$APP_DIR/flashcard.cpp \
    $$APP_DIR/flashcardmanager.cpp \
    $$GTEST_DIR/src/gtest-all.cc \
    $$GMOCK_DIR/src/gmock-all.cc \
    flashcardmanager_gmock.cpp \
    flashcardmanager_gtest.cpp \
    main.cpp

HEADERS += \
    qt_gtest_print.h \
    $$APP_DIR/deck.h \
    $$APP_DIR/flashcard.h \
    $$APP_DIR/flashcardmanager.h

DEFINES += GTEST_HAS_PTHREAD=0
TARGET = deck_tests
