#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "prepare_filename_test.h"

char src_filename1_correct[BUFFER_SIZE];
char src_filename2_correct[BUFFER_SIZE];
char src_filename1_incorrect[BUFFER_SIZE];
char src_filename2_incorrect[BUFFER_SIZE];
char *trg_filename = "test_file.dat";

int prepare_for_testing_of_prepare_filename()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for prepare_filename()",
                          init_suite_prepare_filename,
                          clean_suite_prepare_filename);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition",
                             prepare_filename_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    prepare_filename_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_prepare_filename()
{
    int n =strlen(trg_filename);

    memset(src_filename1_correct, 0, BUFFER_SIZE);
    src_filename1_correct[0] = '"';
    memmove(&src_filename1_correct[1], trg_filename, sizeof(char) * n);
    src_filename1_correct[n+1] = '"';

    memset(src_filename2_correct, 0, BUFFER_SIZE);
    strcpy(src_filename2_correct, trg_filename);

    memset(src_filename1_incorrect, 0, BUFFER_SIZE);
    src_filename1_incorrect[0] = '"';
    memmove(&src_filename1_incorrect[1], trg_filename, sizeof(char) * n);

    memset(src_filename2_incorrect, 0, BUFFER_SIZE);
    strcpy(src_filename2_incorrect, trg_filename);
    src_filename2_incorrect[n] = '"';

    return 0;
}

int clean_suite_prepare_filename()
{
    return 0;
}

void prepare_filename_valid_test_1()
{
    int n = strlen(trg_filename);

    CU_ASSERT_EQUAL_FATAL(n, prepare_filename(src_filename1_correct));
    CU_ASSERT_STRING_EQUAL_FATAL(trg_filename, src_filename1_correct);

    CU_ASSERT_EQUAL_FATAL(n, prepare_filename(src_filename2_correct));
    CU_ASSERT_STRING_EQUAL_FATAL(trg_filename, src_filename2_correct);
}

void prepare_filename_invalid_test_1()
{
    CU_ASSERT_EQUAL_FATAL(0, prepare_filename(src_filename1_incorrect));
    CU_ASSERT_EQUAL_FATAL(0, prepare_filename(src_filename2_incorrect));
    CU_ASSERT_EQUAL_FATAL(0, prepare_filename(NULL));
}
