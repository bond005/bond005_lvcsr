#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "load_phones_vocabulary_test.h"

#define VOCABULARY_SIZE 10

static char *vocabulary_filename = "tested_phones_vocabulary.txt";
static char *target_phones_vocabulary[VOCABULARY_SIZE];

static void create_phones_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        target_phones_vocabulary[i] = malloc(2 * sizeof(char));
        target_phones_vocabulary[i][0] = (char)((int)'a' + i);
        target_phones_vocabulary[i][1] = 0;
    }
}

static int save_phones_vocabulary_for_testing()
{
    int i, res = 1;
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(vocabulary_filename, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    for (i = 0; i < VOCABULARY_SIZE / 2; i++)
    {
        if (fprintf(vocabulary_file, " %s \t\n",
                    target_phones_vocabulary[VOCABULARY_SIZE-i-1]) <= 0)
        {
            res = 0;
            break;
        }
    }
    if (res)
    {
        for (i = VOCABULARY_SIZE / 2; i < VOCABULARY_SIZE; i++)
        {
            if (fprintf(vocabulary_file, " %s \t\n",
                        target_phones_vocabulary[i-VOCABULARY_SIZE/2]) <= 0)
            {
                res = 0;
                break;
            }
        }
    }
    fclose(vocabulary_file);

    return res;
}

int prepare_for_testing_of_load_phones_vocabulary()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for load_phones_vocabulary)",
                          init_suite_load_phones_vocabulary,
                          clean_suite_load_phones_vocabulary);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             load_phones_vocabulary_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    load_phones_vocabulary_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_load_phones_vocabulary()
{
    create_phones_vocabulary();
    if (!save_phones_vocabulary_for_testing())
    {
        return -1;
    }
    return 0;
}

int clean_suite_load_phones_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        if (target_phones_vocabulary[i] != NULL)
        {
            free(target_phones_vocabulary[i]);
            target_phones_vocabulary[i] = NULL;
        }
    }
    remove(vocabulary_filename);
    return 0;
}

void load_phones_vocabulary_valid_test_1()
{
    char **phones_vocabulary = NULL;
    int i, phones_number = 0, is_equal = 0;

    phones_number = load_phones_vocabulary(vocabulary_filename,
                                           &phones_vocabulary);
    if (phones_number == VOCABULARY_SIZE)
    {
        is_equal = 1;
        for (i = 0; i < VOCABULARY_SIZE; i++)
        {
            if (strcmp(phones_vocabulary[i], target_phones_vocabulary[i]) != 0)
            {
                is_equal = 0;
                break;
            }
        }
    }
    free_string_array(&phones_vocabulary, phones_number);

    CU_ASSERT_EQUAL_FATAL(VOCABULARY_SIZE, phones_number);
    CU_ASSERT_TRUE_FATAL(is_equal);
}

void load_phones_vocabulary_invalid_test_1()
{
    char **phones_vocabulary = NULL;
    int phones_number = 0;

    phones_number = load_phones_vocabulary(NULL, &phones_vocabulary);
    free_string_array(&phones_vocabulary, phones_number);
    CU_ASSERT_EQUAL_FATAL(0, phones_number);

    phones_number = load_phones_vocabulary(vocabulary_filename, NULL);
    free_string_array(&phones_vocabulary, phones_number);
    CU_ASSERT_EQUAL_FATAL(0, phones_number);
}
