#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../backtrack_pointers_lib.h"
#include "backtrack_pointers_test.h"

int prepare_for_testing_of_backtrack_pointers()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for functions from the "\
                          "backtrack_pointers_lib library",
                          init_suite_backtrack_pointers,
                          clean_suite_backtrack_pointers);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(
             pSuite, "add_to_backtrack_pointer(): valid partition",
             add_to_backtrack_pointer_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "add_to_backtrack_pointer: invalid partition",
                    add_to_backtrack_pointer_invalid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_from_backtrack_pointer(): valid "\
                    "partition",
                    remove_from_backtrack_pointer_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_from_backtrack_pointer: invalid partition",
                    remove_from_backtrack_pointer_invalid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_all_from_backtrack_pointer(): valid "\
                    "partition",
                    remove_all_from_backtrack_pointer_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_all_from_backtrack_pointer: invalid "\
                    "partition",
                    remove_all_from_backtrack_pointer_invalid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "copy_backtrack_pointers(): valid partition",
                    copy_backtrack_pointers_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "copy_backtrack_pointers(): invalid partition",
                    copy_backtrack_pointers_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_backtrack_pointers()
{
    //...
    return 0;
}

int clean_suite_backtrack_pointers()
{
    //...
    return 0;
}

void add_to_backtrack_pointer_valid_test_1()
{
    //...
}

void remove_from_backtrack_pointer_valid_test_1()
{
    //...
}

void remove_all_from_backtrack_pointer_valid_test_1()
{
    //...
}

void copy_backtrack_pointers_valid_test_1()
{
    //...
}

void add_to_backtrack_pointer_invalid_test_1()
{
    //...
}

void remove_from_backtrack_pointer_invalid_test_1()
{
    //...
}

void remove_all_from_backtrack_pointer_invalid_test_1()
{
    //...
}

void copy_backtrack_pointers_invalid_test_1()
{
    //...
}
