#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"

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

int main()
{
    CU_pSuite pSuite1 = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    /* add a suite to the registry */
    pSuite1 = CU_add_suite("Test suite for parse_transcription_str()",
                           init_suite_parse_transcription_str,
                           clean_suite_parse_transcription_str);
    if (NULL == pSuite1)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite1, "Valid partition",
                             parse_transcription_str_valid_test_1))
            || (NULL == CU_add_test(pSuite1, "Invalid partitions",
                                    parse_transcription_str_invalid_test_1)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
