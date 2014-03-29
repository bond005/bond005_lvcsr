#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "read_string_test.h"

static int number_of_target_strings = 3;
static char *test_file_name = "test.txt";
static FILE *test_file = NULL;
static char test_string[BUFFER_SIZE];
static char *target_strings[3] = { "aa agfdrjk k", "bbb j", "1246 990 d" };

int prepare_for_testing_of_read_string()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for read_string()",
                          init_suite_read_string, clean_suite_read_string);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition",
                             read_string_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    read_string_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_read_string()
{
    int i, res = 0;

    memset(test_string, 0, BUFFER_SIZE);

    test_file = fopen(test_file_name, "w");
    if (test_file == NULL)
    {
        return -1;
    }
    for (i = 0; i < number_of_target_strings; i++)
    {
        if (fprintf(test_file, "   %s    \n\n", target_strings[i]) <= 0)
        {
            res = -1;
            break;
        }
    }
    fclose(test_file);
    test_file = NULL;
    if (res != 0)
    {
        remove(test_file_name);
        return -1;
    }

    test_file = fopen(test_file_name, "r");
    if (test_file == NULL)
    {
        remove(test_file_name);
        return -1;
    }

    return 0;
}

int clean_suite_read_string()
{
    if (test_file != NULL)
    {
        fclose(test_file);
        test_file = NULL;
        remove(test_file_name);
    }
    return 0;
}

void read_string_valid_test_1()
{
    int i = 0, n;
    while (!feof(test_file))
    {
        n = read_string(test_file, test_string);
        CU_ASSERT_FATAL(n >= 0);
        if (n > 0)
        {
            CU_ASSERT_EQUAL_FATAL(strlen(target_strings[i]), strlen(test_string));
            CU_ASSERT_STRING_EQUAL_FATAL(target_strings[i], test_string);
            i++;
        }
        CU_ASSERT_FATAL(i <= number_of_target_strings);
    }
}

void read_string_invalid_test_1()
{
    CU_ASSERT_EQUAL_FATAL(0, read_string(NULL, test_string));
    CU_ASSERT_EQUAL_FATAL(0, read_string(test_file, NULL));
}
