#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "parse_transcription_str_test.h"

int create_test_suites_for_parse_transcription_str()
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
    return 0;
}

int clean_suite_parse_transcription_str()
{
    return 0;
}

void parse_transcription_str_valid_test_1()
{
    char *transcription_str = "a d c";
    char *phones_vocabulary[] = { "a", "b", "c", "d", "e" };
    int phones_number = 5;
    int phones_sequence[3];
    int phones_sequence_length = 3;
    int target_phones_sequence[3] = { 0, 3, 2 };
    int i;

    CU_ASSERT_EQUAL_FATAL(phones_sequence_length, parse_transcription_str(
                              transcription_str, phones_vocabulary,
                              phones_number, phones_sequence));
    for (i = 0; i < phones_sequence_length; i++)
    {
        CU_ASSERT_EQUAL(target_phones_sequence[i], phones_sequence[i]);
    }
}

void parse_transcription_str_invalid_test_1()
{
    char *transcription_str = "a d c";
    char *phones_vocabulary[] = { "a", "b", "c", "d", "e" };
    int phones_number = 5;
    int phones_sequence[3];

    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              NULL, phones_vocabulary, phones_number,
                              phones_sequence));
    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              transcription_str, NULL, phones_number,
                              phones_sequence));
    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              transcription_str, phones_vocabulary, 0,
                              phones_sequence));
    CU_ASSERT_EQUAL_FATAL(0, parse_transcription_str(
                              transcription_str, phones_vocabulary,
                              phones_number, NULL));
}
