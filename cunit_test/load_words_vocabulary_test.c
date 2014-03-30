#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "load_words_vocabulary_test.h"

#define VOCABULARY_SIZE 10

static char *name_of_valid_vocabulary_file = "valid_words_vocabulary.txt";
static char *name_of_invalid_vocabulary_file = "invalid_words_vocabulary.txt";
static char *target_words_vocabulary[VOCABULARY_SIZE];
static char *target_transcriptions[VOCABULARY_SIZE];

static void create_words_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        target_words_vocabulary[i] = malloc(4 * sizeof(char));
        target_words_vocabulary[i][0] = (char)((int)'a' + i);
        target_words_vocabulary[i][1] = target_words_vocabulary[i][0];
        target_words_vocabulary[i][2] = target_words_vocabulary[i][1];
        target_words_vocabulary[i][3] = 0;

        target_transcriptions[i] = malloc(3 * 2 * sizeof(char));
        target_transcriptions[i][0] = target_words_vocabulary[i][0];
        target_transcriptions[i][1] = ' ';
        target_transcriptions[i][2] = target_words_vocabulary[i][0];
        target_transcriptions[i][3] = ' ';
        target_transcriptions[i][4] = target_words_vocabulary[i][0];
        target_transcriptions[i][5] = 0;
    }
}

static int save_words_vocabulary_for_testing()
{
    int i, res = 1;
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(name_of_valid_vocabulary_file, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    for (i = 0; i < VOCABULARY_SIZE / 2; i++)
    {
        if (fprintf(vocabulary_file, " %s = \t %s \t\n",
                    target_words_vocabulary[VOCABULARY_SIZE-i-1],
                    target_transcriptions[VOCABULARY_SIZE-i-1]) <= 0)
        {
            res = 0;
            break;
        }
    }
    if (res)
    {
        for (i = VOCABULARY_SIZE / 2; i < VOCABULARY_SIZE; i++)
        {
            if (fprintf(vocabulary_file, " \t %s =%s \t\n",
                        target_words_vocabulary[i-VOCABULARY_SIZE/2],
                        target_transcriptions[i-VOCABULARY_SIZE/2]) <= 0)
            {
                res = 0;
                break;
            }
        }
    }
    fclose(vocabulary_file);

    return res;
}

static int save_words_vocabulary_without_errors_for_testing()
{
    int i, res = 1;
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(name_of_invalid_vocabulary_file, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    for (i = 0; i < VOCABULARY_SIZE / 2; i++)
    {
        if (fprintf(vocabulary_file, " %s = \t %s \t\n",
                    target_words_vocabulary[VOCABULARY_SIZE-i-1],
                    target_transcriptions[VOCABULARY_SIZE-i-1]) <= 0)
        {
            res = 0;
            break;
        }
    }
    if (res)
    {
        for (i = VOCABULARY_SIZE / 2; i < VOCABULARY_SIZE; i++)
        {
            if (fprintf(vocabulary_file, " \t %s =%s \t\n",
                        target_words_vocabulary[i-VOCABULARY_SIZE/2],
                        target_transcriptions[i-VOCABULARY_SIZE/2]) <= 0)
            {
                res = 0;
                break;
            }
        }
    }
    if (res)
    {
        if (fprintf(vocabulary_file, " \t %s %s \t\n", "abc", "a b c") <= 0)
        {
            res = 0;
        }
    }
    fclose(vocabulary_file);

    return res;
}

int prepare_for_testing_of_load_words_vocabulary()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for load_words_vocabulary)",
                          init_suite_load_words_vocabulary,
                          clean_suite_load_words_vocabulary);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             load_words_vocabulary_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    load_words_vocabulary_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_load_words_vocabulary()
{
    create_words_vocabulary();
    if (!save_words_vocabulary_for_testing())
    {
        return -1;
    }
    if (!save_words_vocabulary_without_errors_for_testing())
    {
        return -1;
    }
    return 0;
}

int clean_suite_load_words_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        if (target_words_vocabulary[i] != NULL)
        {
            free(target_words_vocabulary[i]);
            target_words_vocabulary[i] = NULL;
        }
        if (target_transcriptions[i] != NULL)
        {
            free(target_transcriptions[i]);
            target_transcriptions[i] = NULL;
        }
    }
    remove(name_of_valid_vocabulary_file);
    remove(name_of_invalid_vocabulary_file);
    return 0;
}

void load_words_vocabulary_valid_test_1()
{
    char **words_vocabulary = NULL;
    int i, words_number = 0, is_equal = 0;

    words_number = load_words_vocabulary(name_of_valid_vocabulary_file,
                                         &words_vocabulary);
    if (words_number == VOCABULARY_SIZE)
    {
        is_equal = 1;
        for (i = 0; i < VOCABULARY_SIZE; i++)
        {
            if (strcmp(words_vocabulary[i], target_words_vocabulary[i]) != 0)
            {
                is_equal = 0;
                break;
            }
        }
    }
    free_string_array(&words_vocabulary, words_number);

    CU_ASSERT_EQUAL_FATAL(VOCABULARY_SIZE, words_number);
    CU_ASSERT_TRUE_FATAL(is_equal);
}

void load_words_vocabulary_valid_test_2()
{
    char **words_vocabulary = NULL;
    int words_number = 0;

    words_number = load_words_vocabulary(name_of_invalid_vocabulary_file,
                                         &words_vocabulary);
    free_string_array(&words_vocabulary, words_number);

    CU_ASSERT_EQUAL_FATAL(0, words_number);
}

void load_words_vocabulary_invalid_test_1()
{
    char **words_vocabulary = NULL;
    int words_number = 0;

    words_number = load_words_vocabulary(NULL, &words_vocabulary);
    free_string_array(&words_vocabulary, words_number);
    CU_ASSERT_EQUAL_FATAL(0, words_number);

    words_number = load_words_vocabulary(name_of_valid_vocabulary_file, NULL);
    free_string_array(&words_vocabulary, words_number);
    CU_ASSERT_EQUAL_FATAL(0, words_number);
}
