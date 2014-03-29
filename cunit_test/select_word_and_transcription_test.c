#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "select_word_and_transcription_test.h"

#define STR_SIZE 100

static char source_str1[STR_SIZE];
static char source_str2[STR_SIZE];
static char target_word_str1[STR_SIZE];
static char target_word_str2[STR_SIZE];
static char target_transcription_str1[STR_SIZE];
static char target_transcription_str2[STR_SIZE];

void prepare_source_data_for_testing()
{
    memset(source_str1, 0, STR_SIZE);
    strcpy(source_str1, "word = a b' c");

    memset(source_str2, 0, STR_SIZE);
    strcpy(source_str2, "other_word=c1 d e'");
}

void prepare_target_data_for_testing()
{
    memset(target_word_str1, 0, STR_SIZE);
    strcpy(target_word_str1, "word");

    memset(target_transcription_str1, 0, STR_SIZE);
    strcpy(target_transcription_str1, "a b' c");

    memset(target_word_str2, 0, STR_SIZE);
    strcpy(target_word_str2, "other_word");

    memset(target_transcription_str2, 0, STR_SIZE);
    strcpy(target_transcription_str2, "c1 d e'");
}

int prepare_for_testing_of_select_word_and_transcription()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for select_word_and_transcription()",
                          init_suite_select_word_and_transcription,
                          clean_suite_select_word_and_transcription);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition",
                             select_word_and_transcription_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    select_word_and_transcription_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_select_word_and_transcription()
{
    prepare_source_data_for_testing();
    prepare_target_data_for_testing();
    return 0;
}

int clean_suite_select_word_and_transcription()
{
    return 0;
}

void select_word_and_transcription_valid_test_1()
{
    char *word_str = NULL, *transcription_str = NULL;

    CU_ASSERT_TRUE_FATAL(select_word_and_transcription(source_str1, &word_str,
                                                       &transcription_str));
    CU_ASSERT_PTR_NOT_NULL_FATAL(word_str);
    CU_ASSERT_PTR_NOT_NULL_FATAL(transcription_str);
    CU_ASSERT_STRING_EQUAL_FATAL(target_word_str1, word_str);
    CU_ASSERT_STRING_EQUAL_FATAL(target_transcription_str1, transcription_str);

    CU_ASSERT_TRUE_FATAL(select_word_and_transcription(source_str2, &word_str,
                                                       &transcription_str));
    CU_ASSERT_PTR_NOT_NULL_FATAL(word_str);
    CU_ASSERT_PTR_NOT_NULL_FATAL(transcription_str);
    CU_ASSERT_STRING_EQUAL_FATAL(target_word_str2, word_str);
    CU_ASSERT_STRING_EQUAL_FATAL(target_transcription_str2, transcription_str);
}

void select_word_and_transcription_invalid_test_1()
{
    char *word_str = NULL;
    char *transcription_str = NULL;

    CU_ASSERT_FALSE_FATAL(select_word_and_transcription(NULL, &word_str,
                                                        &transcription_str));
    CU_ASSERT_PTR_NULL_FATAL(word_str);
    CU_ASSERT_PTR_NULL_FATAL(transcription_str);

    CU_ASSERT_FALSE_FATAL(select_word_and_transcription(source_str1, NULL,
                                                        &transcription_str));
    CU_ASSERT_PTR_NULL_FATAL(word_str);
    CU_ASSERT_PTR_NULL_FATAL(transcription_str);

    CU_ASSERT_FALSE_FATAL(select_word_and_transcription(source_str1, &word_str,
                                                        NULL));
    CU_ASSERT_PTR_NULL_FATAL(word_str);
    CU_ASSERT_PTR_NULL_FATAL(transcription_str);
}
