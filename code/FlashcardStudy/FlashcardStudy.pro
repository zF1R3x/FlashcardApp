TEMPLATE = app
TARGET = MainWindow

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
        deck.cpp \
        deckwindow.cpp \
        flashcard.cpp \
        flashcardmanager.cpp \
        main.cpp \
        mainwindow.cpp \
        statstracker.cpp \
        studywindow.cpp

HEADERS += \
    deck.h \
    deckwindow.h \
    flashcard.h \
    flashcardfactory.h \
    flashcardmanager.h \
    mainwindow.h \
    statstracker.h \
    studywindow.h

FORMS += \
    deckwindow.ui \
    mainwindow.ui \
    studywindow.ui

DISTFILES += \
    README
