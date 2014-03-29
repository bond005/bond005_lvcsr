TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    ../bond005_lvcsr_lib.c \
    parse_transcription_str_test.c

HEADERS += \
    ../bond005_lvcsr_lib.h \
    parse_transcription_str_test.h

LIBS += -lcunit
