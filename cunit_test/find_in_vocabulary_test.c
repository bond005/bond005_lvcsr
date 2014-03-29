#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "find_in_vocabulary_test.h"

int prepare_for_testing_of_find_in_vocabulary()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for find_in_vocabulary()",
                          init_suite_find_in_vocabulary,
                          clean_suite_find_in_vocabulary);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             find_in_vocabulary_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    find_in_vocabulary_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    find_in_vocabulary_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_find_in_vocabulary()
{
    return 0;
}

int clean_suite_find_in_vocabulary()
{
    return 0;
}

void find_in_vocabulary_valid_test_1()
{
    char *vocabulary[] = { "aaa", "bbb", "ccc", "ddd", "eee" };
    int vocabulary_size = 5;
    char *found_name = "bbb";
    int target_index = 1, is_equal = 0;

    CU_ASSERT_EQUAL_FATAL(target_index, find_in_vocabulary(
                              vocabulary, vocabulary_size, found_name,
                              &is_equal));
    CU_ASSERT_TRUE_FATAL(is_equal);
}

void find_in_vocabulary_valid_test_2()
{
    char *vocabulary[] = { "aaa", "bbb", "ccc", "ddd", "eee" };
    int vocabulary_size = 5;
    char *found_name = "cac";
    int target_index = 2, is_equal = 0;

    CU_ASSERT_EQUAL_FATAL(target_index, find_in_vocabulary(
                              vocabulary, vocabulary_size, found_name,
                              &is_equal));
    CU_ASSERT_FALSE_FATAL(is_equal);
}

void find_in_vocabulary_invalid_test_1()
{
    char *vocabulary[] = { "aaa", "bbb", "ccc", "ddd", "eee" };
    int vocabulary_size = 5;
    char *found_name = "bbb";
    int target_index = -1, is_equal = 0;

    CU_ASSERT_EQUAL_FATAL(target_index, find_in_vocabulary(
                              NULL, vocabulary_size, found_name, &is_equal));
    CU_ASSERT_FALSE_FATAL(is_equal);

    CU_ASSERT_EQUAL_FATAL(target_index, find_in_vocabulary(
                              vocabulary, 0, found_name, &is_equal));
    CU_ASSERT_FALSE_FATAL(is_equal);

    CU_ASSERT_EQUAL_FATAL(target_index, find_in_vocabulary(
                              vocabulary, vocabulary_size, NULL,
                              &is_equal));
    CU_ASSERT_FALSE_FATAL(is_equal);
}
