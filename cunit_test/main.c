#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "add_word_to_words_tree_test.h"
#include "create_linear_words_lexicon_test.h"
#include "create_words_vocabulary_tree_test.h"
#include "find_in_vocabulary_test.h"
#include "get_bigram_probability_test.h"
#include "load_language_model_test.h"
#include "load_phonemes_MLF_test.h"
#include "load_phonemes_vocabulary_test.h"
#include "load_words_MLF_test.h"
#include "load_words_vocabulary_test.h"
#include "parse_transcription_str_test.h"
#include "prepare_filename_test.h"
#include "read_string_test.h"
#include "save_language_model_test.h"
#include "save_words_MLF_test.h"
#include "select_word_and_transcription_test.h"
#include "string_to_transcription_node_test.h"
#include "word_exists_in_words_tree_test.h"

int main()
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    if (!prepare_for_testing_of_add_word_to_words_tree())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_prepare_filename())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_read_string())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_find_in_vocabulary())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_select_word_and_transcription())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_parse_transcription_str())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_string_to_transcription_node())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_word_exists_in_words_tree())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_load_phonemes_MLF())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_load_words_MLF())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_save_words_MLF())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_load_phonemes_vocabulary())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_load_words_vocabulary())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_create_words_vocabulary_tree())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_create_linear_words_lexicon())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_load_language_model())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_save_language_model())
    {
        return CU_get_error();
    }
    if (!prepare_for_testing_of_get_bigram_probability())
    {
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
