TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    bond005_lvcsr_lib.c \
    backtrack_pointers_lib.c

HEADERS += \
    bond005_lvcsr_lib.h \
    backtrack_pointers_lib.h

