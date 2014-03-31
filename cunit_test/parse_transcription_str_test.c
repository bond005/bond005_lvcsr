#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "parse_transcription_str_test.h"

#define STR_SIZE 100
#define VOCABULARY_SIZE 5

static char transcription_str[STR_SIZE];
static char *vocabulary[VOCABULARY_SIZE];

int prepare_for_testing_of_parse_transcription_str()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for parse_transcription_str()",
                          init_suite_parse_transcription_str,
                          clean_suite_parse_transcription_str);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition",
                             parse_transcription_str_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    parse_transcription_str_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_parse_transcription_str()
{
    int i;
    memset(transcription_str, 0, STR_SIZE);
    strcpy(transcription_str, "a d c");
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        vocabulary[i] = malloc(STR_SIZE);
        memset(vocabulary[i], 0, STR_SIZE);
        vocabulary[i][0] = (char)((int)'a' + i);
    }
    return 0;
}

int clean_suite_parse_transcription_str()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        free(vocabulary[i]);
    }
    return 0;
}

void parse_transcription_str_valid_test_1()
{
    int phonemes_number = VOCABULARY_SIZE;
    int phonemes_sequence[3];
    int phonemes_sequence_length = 3;
    int target_phonemes_sequence[3] = { 0, 3, 2 };
    int i;

    CU_ASSERT_EQUAL_FATAL(phonemes_sequence_length, parse_transcription_str(
                              transcription_str, vocabulary, phonemes_number,
                              phonemes_sequence));
    for (i = 0; i < phonemes_sequence_length; i++)
    {
        CU_ASSERT_EQUAL(target_phonemes_sequence[i], phonemes_sequence[i]);
    }
}

void parse_transcription_str_invalid_test_1()
{
    int phonemes_number = VOCABULARY_SIZE;
    int phonemes_sequence[3];

    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              NULL, vocabulary, phonemes_number,
                              phonemes_sequence));
    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              transcription_str, NULL, phonemes_number,
                              phonemes_sequence));
    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              transcription_str, vocabulary, 0,
                              phonemes_sequence));
    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              transcription_str, vocabulary, phonemes_number,
                              NULL));
}
