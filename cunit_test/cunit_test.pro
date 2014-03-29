TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    ../bond005_lvcsr_lib.c \
    parse_transcription_str_test.c \
    select_word_and_transcription_test.c \
    find_in_vocabulary_test.c \
    read_string_test.c \
    prepare_filename_test.c \
    string_to_transcription_node_test.c \
    add_word_to_words_tree_test.c

HEADERS += \
    ../bond005_lvcsr_lib.h \
    parse_transcription_str_test.h \
    select_word_and_transcription_test.h \
    find_in_vocabulary_test.h \
    read_string_test.h \
    prepare_filename_test.h \
    string_to_transcription_node_test.h \
    add_word_to_words_tree_test.h

LIBS += -lcunit
