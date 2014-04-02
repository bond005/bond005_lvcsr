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
    add_word_to_words_tree_test.c \
    word_exists_in_words_tree_test.c \
    load_phonemes_MLF_test.c \
    load_words_MLF_test.c \
    save_words_MLF_test.c \
    load_phonemes_vocabulary_test.c \
    load_words_vocabulary_test.c \
    create_words_vocabulary_tree_test.c \
    load_language_model_test.c \
    save_language_model_test.c \
    get_bigram_probability_test.c \
    calculate_language_model_test.c \
    create_linear_words_lexicon_test.c \
    calculate_phonemes_probabilities_test.c \
    recognize_words_test.c

HEADERS += \
    ../bond005_lvcsr_lib.h \
    parse_transcription_str_test.h \
    select_word_and_transcription_test.h \
    find_in_vocabulary_test.h \
    read_string_test.h \
    prepare_filename_test.h \
    string_to_transcription_node_test.h \
    add_word_to_words_tree_test.h \
    word_exists_in_words_tree_test.h \
    load_phonemes_MLF_test.h \
    load_words_MLF_test.h \
    save_words_MLF_test.h \
    load_phonemes_vocabulary_test.h \
    load_words_vocabulary_test.h \
    create_words_vocabulary_tree_test.h \
    load_language_model_test.h \
    save_language_model_test.h \
    get_bigram_probability_test.h \
    calculate_language_model_test.h \
    create_linear_words_lexicon_test.h \
    calculate_phonemes_probabilities_test.h \
    recognize_words_test.h

LIBS += -lcunit
