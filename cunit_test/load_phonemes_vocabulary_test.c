#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "load_phonemes_vocabulary_test.h"

#define VOCABULARY_SIZE 10

static char *vocabulary_filename = "tested_phonemes_vocabulary.txt";
static char *target_phonemes_vocabulary[VOCABULARY_SIZE];

static void create_phonemes_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        target_phonemes_vocabulary[i] = malloc(2 * sizeof(char));
        target_phonemes_vocabulary[i][0] = (char)((int)'a' + i);
        target_phonemes_vocabulary[i][1] = 0;
    }
}

static int save_phonemes_vocabulary_for_testing()
{
    int i, res = 1;
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(vocabulary_filename, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        if (fprintf(vocabulary_file, " %s \t\n",
                    target_phonemes_vocabulary[i]) <= 0)
        {
            res = 0;
            break;
        }
    }
    fclose(vocabulary_file);

    return res;
}

int prepare_for_testing_of_load_phonemes_vocabulary()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for load_phonemes_vocabulary)",
                          init_suite_load_phonemes_vocabulary,
                          clean_suite_load_phonemes_vocabulary);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             load_phonemes_vocabulary_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    load_phonemes_vocabulary_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_load_phonemes_vocabulary()
{
    create_phonemes_vocabulary();
    if (!save_phonemes_vocabulary_for_testing())
    {
        return -1;
    }
    return 0;
}

int clean_suite_load_phonemes_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        if (target_phonemes_vocabulary[i] != NULL)
        {
            free(target_phonemes_vocabulary[i]);
            target_phonemes_vocabulary[i] = NULL;
        }
    }
    remove(vocabulary_filename);
    return 0;
}

void load_phonemes_vocabulary_valid_test_1()
{
    char **phonemes_vocabulary = NULL;
    int i, phonemes_number = 0, is_equal = 0;

    phonemes_number = load_phonemes_vocabulary(vocabulary_filename,
                                               &phonemes_vocabulary);
    if (phonemes_number == VOCABULARY_SIZE)
    {
        is_equal = 1;
        for (i = 0; i < VOCABULARY_SIZE; i++)
        {
            if (strcmp(phonemes_vocabulary[i], target_phonemes_vocabulary[i])
                    != 0)
            {
                is_equal = 0;
                break;
            }
        }
    }
    free_string_array(&phonemes_vocabulary, phonemes_number);

    CU_ASSERT_EQUAL_FATAL(VOCABULARY_SIZE, phonemes_number);
    CU_ASSERT_TRUE_FATAL(is_equal);
}

void load_phonemes_vocabulary_invalid_test_1()
{
    char **phonemes_vocabulary = NULL;
    int phonemes_number = 0;

    phonemes_number = load_phonemes_vocabulary(NULL, &phonemes_vocabulary);
    free_string_array(&phonemes_vocabulary, phonemes_number);
    CU_ASSERT_EQUAL_FATAL(0, phonemes_number);

    phonemes_number = load_phonemes_vocabulary(vocabulary_filename, NULL);
    free_string_array(&phonemes_vocabulary, phonemes_number);
    CU_ASSERT_EQUAL_FATAL(0, phonemes_number);
}
