#include <stdlib.h>
#include <time.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../backtrack_pointers_lib.h"
#include "backtrack_pointers_test.h"

static PBacktrackPointer btp1 = NULL;
static PBacktrackPointer btp2 = NULL;
static PBacktrackPointer btp3 = NULL;
static PBacktrackPointer btp4 = NULL;
static PBacktrackPointer btp5 = NULL;

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
                    pSuite, "add_to_backtrack_pointer(): invalid partition",
                    add_to_backtrack_pointer_invalid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_from_backtrack_pointer(): valid "\
                    "partition",
                    remove_from_backtrack_pointer_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_from_backtrack_pointer(): invalid "\
                    "partition", remove_from_backtrack_pointer_invalid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_all_from_backtrack_pointer(): valid "\
                    "partition",
                    remove_all_from_backtrack_pointer_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "remove_all_from_backtrack_pointer(): invalid "\
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
    srand(time(NULL));
    create_backtrack_pointer(&btp1);
    create_backtrack_pointer(&btp2);
    create_backtrack_pointer(&btp3);
    create_backtrack_pointer(&btp4);
    create_backtrack_pointer(&btp5);
    return 0;
}

int clean_suite_backtrack_pointers()
{
    free_backtrack_pointer(&btp1);
    free_backtrack_pointer(&btp2);
    free_backtrack_pointer(&btp3);
    free_backtrack_pointer(&btp4);
    free_backtrack_pointer(&btp5);
    return 0;
}

void add_to_backtrack_pointer_valid_test_1()
{
    int i;
    for (i = 0; i < CAPACITY_INC; i++)
    {
        add_to_backtrack_pointer(btp1, i, i);
        CU_ASSERT_EQUAL_FATAL(i+1, btp1->size);
        CU_ASSERT_EQUAL_FATAL(CAPACITY_INC, btp1->capacity);
        CU_ASSERT_EQUAL_FATAL(i, btp1->words[btp1->size-1]);
        CU_ASSERT_EQUAL_FATAL(i, btp1->times[btp1->size-1]);
    }
    i = CAPACITY_INC;
    add_to_backtrack_pointer(btp1, i, i);
    CU_ASSERT_EQUAL_FATAL(i+1, btp1->size);
    CU_ASSERT_EQUAL_FATAL(2 * CAPACITY_INC, btp1->capacity);
    CU_ASSERT_EQUAL_FATAL(i, btp1->words[btp1->size-1]);
    CU_ASSERT_EQUAL_FATAL(i, btp1->times[btp1->size-1]);
}

void remove_from_backtrack_pointer_valid_test_1()
{
    add_to_backtrack_pointer(btp2, 1, 1);
    add_to_backtrack_pointer(btp2, 2, 2);
    remove_from_backtrack_pointer(btp2);
    CU_ASSERT_EQUAL_FATAL(1, btp2->size);
    CU_ASSERT_EQUAL_FATAL(CAPACITY_INC, btp2->capacity);
    CU_ASSERT_EQUAL_FATAL(1, btp2->words[btp2->size-1]);
    CU_ASSERT_EQUAL_FATAL(1, btp2->times[btp2->size-1]);
}

void remove_all_from_backtrack_pointer_valid_test_1()
{
    add_to_backtrack_pointer(btp3, 1, 1);
    add_to_backtrack_pointer(btp3, 2, 2);
    remove_all_from_backtrack_pointer(btp3);
    CU_ASSERT_EQUAL_FATAL(0, btp3->size);
    CU_ASSERT_EQUAL_FATAL(CAPACITY_INC, btp3->capacity);
    CU_ASSERT_PTR_NOT_NULL_FATAL(btp3->words);
    CU_ASSERT_PTR_NOT_NULL_FATAL(btp3->times);
}

void copy_backtrack_pointers_valid_test_1()
{
    int i;
    add_to_backtrack_pointer(btp4, 1, 1);
    add_to_backtrack_pointer(btp4, 2, 2);
    add_to_backtrack_pointer(btp5, 10, 10);
    add_to_backtrack_pointer(btp5, 20, 20);
    add_to_backtrack_pointer(btp5, 30, 30);
    copy_backtrack_pointers(btp4, btp5);
    CU_ASSERT_EQUAL_FATAL(btp4->size, btp5->size);
    CU_ASSERT_EQUAL_FATAL(btp4->capacity, btp5->capacity);
    for (i = 0; i < btp4->size; i++)
    {
        CU_ASSERT_EQUAL_FATAL(btp4->words[i], btp5->words[i]);
        CU_ASSERT_EQUAL_FATAL(btp4->times[i], btp5->times[i]);
    }
}

void add_to_backtrack_pointer_invalid_test_1()
{
    add_to_backtrack_pointer(NULL, 1, 1);
    CU_PASS(add_to_backtrack_pointer(NULL, 1, 1););
}

void remove_from_backtrack_pointer_invalid_test_1()
{
    remove_from_backtrack_pointer(NULL);
    CU_PASS(remove_from_backtrack_pointer(NULL););
}

void remove_all_from_backtrack_pointer_invalid_test_1()
{
    remove_all_from_backtrack_pointer(NULL);
    CU_PASS(remove_all_from_backtrack_pointer(NULL););
}

void copy_backtrack_pointers_invalid_test_1()
{
    copy_backtrack_pointers(NULL, btp5);
    CU_PASS(copy_backtrack_pointers(NULL, btp5););

    copy_backtrack_pointers(btp4, NULL);
    CU_PASS(copy_backtrack_pointers(btp4, NULL););
}
