#include <float.h>
#include <stdlib.h>
#include <time.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "load_language_model_test.h"

#define UNIGRAMS_NUMBER 10

static TLanguageModel target_language_model;
static char *correct_language_model_name = "correct_language_model.dat";
static char *incorrect_language_model_name1 = "incorrect_language_model1.dat";
static char *incorrect_language_model_name2 = "incorrect_language_model2.dat";

static int compare_int_values(const void *ptr1, const void *ptr2)
{
    int *val1 = (int*)ptr1;
    int *val2 = (int*)ptr2;

    if ((val1 == NULL) && (val2 == NULL))
    {
        return 0;
    }
    if (val1 == NULL)
    {
        return -1;
    }
    if (val2 == NULL)
    {
        return 1;
    }

    return (*val1 - *val2);
}

static void create_target_language_model()
{
    int i, j, n;

    srand(time(NULL));

    target_language_model.unigrams_number = UNIGRAMS_NUMBER;
    target_language_model.unigrams_probabilities
            = malloc(UNIGRAMS_NUMBER * sizeof(float));
    target_language_model.bigrams
            = malloc(UNIGRAMS_NUMBER * sizeof(TWordBigram));

    for (i = 0; i < UNIGRAMS_NUMBER; i++)
    {
        target_language_model.unigrams_probabilities[i]
                = rand() / (float)RAND_MAX;
        target_language_model.bigrams[i].number_of_first_words = 0;
        target_language_model.bigrams[i].first_words = NULL;
        target_language_model.bigrams[i].probabilities = NULL;
    }
    for (i = 0; i < UNIGRAMS_NUMBER; i++)
    {
        n = rand() % (3 * UNIGRAMS_NUMBER) + 1;
        target_language_model.bigrams[i].number_of_first_words = n;
        target_language_model.bigrams[i].first_words = malloc(n * sizeof(int));
        target_language_model.bigrams[i].probabilities
                = malloc(n * sizeof(float));
        for (j = 0; j < n; j++)
        {
            target_language_model.bigrams[i].first_words[j]
                    = rand() % UNIGRAMS_NUMBER;
            target_language_model.bigrams[i].first_words[j]
                    = rand() / (float)RAND_MAX;
        }
        qsort(target_language_model.bigrams[i].first_words, n, sizeof(int),
              compare_int_values);
    }
}

static int save_incorrect_language_model1()
{
    int i, n, is_ok = 1;
    FILE *h_file = NULL;

    h_file = fopen(incorrect_language_model_name1, "wb");
    if (h_file == NULL)
    {
        return 0;
    }

    n = target_language_model.unigrams_number * 1000;
    if (fwrite(&n, sizeof(int), 1, h_file) != 1)
    {
        fclose(h_file);
        return 0;
    }
    n = target_language_model.unigrams_number;
    if (fwrite(target_language_model.unigrams_probabilities, sizeof(float), n,
               h_file) != n)
    {
        fclose(h_file);
        return 0;
    }

    for (i = 0; i < target_language_model.unigrams_number; i++)
    {
        n = target_language_model.bigrams[i].number_of_first_words;
        if (fwrite(&n, sizeof(int), 1, h_file) != 1)
        {
            is_ok = 0;
            break;
        }
        if (n > 0)
        {
            if (fwrite(target_language_model.bigrams[i].first_words,
                       sizeof(int), n, h_file) != n)
            {
                is_ok = 0;
                break;
            }
            if (fwrite(target_language_model.bigrams[i].probabilities,
                       sizeof(float), n, h_file) != n)
            {
                is_ok = 0;
                break;
            }
        }
    }

    fclose(h_file);
    return is_ok;
}

static int save_incorrect_language_model2()
{
    int i, n, is_ok = 1;
    FILE *h_file = NULL;

    h_file = fopen(incorrect_language_model_name1, "wb");
    if (h_file == NULL)
    {
        return 0;
    }

    n = target_language_model.unigrams_number;
    if (fwrite(&n, sizeof(int), 1, h_file) != 1)
    {
        fclose(h_file);
        return 0;
    }
    n = target_language_model.unigrams_number;
    if (fwrite(target_language_model.unigrams_probabilities, sizeof(float), n,
               h_file) != n)
    {
        fclose(h_file);
        return 0;
    }

    for (i = 0; i < target_language_model.unigrams_number; i++)
    {
        n = target_language_model.bigrams[i].number_of_first_words;
        if ((rand() / (float)RAND_MAX) > 0.5)
        {
            n += 1000;
        }
        if (fwrite(&n, sizeof(int), 1, h_file) != 1)
        {
            is_ok = 0;
            break;
        }
        n = target_language_model.bigrams[i].number_of_first_words;
        if (n > 0)
        {
            if (fwrite(target_language_model.bigrams[i].first_words,
                       sizeof(int), n, h_file) != n)
            {
                is_ok = 0;
                break;
            }
            if (fwrite(target_language_model.bigrams[i].probabilities,
                       sizeof(float), n, h_file) != n)
            {
                is_ok = 0;
                break;
            }
        }
    }

    fclose(h_file);
    return is_ok;
}

static int compare_language_models(TLanguageModel m1, TLanguageModel m2)
{
    int i, j, n, res = 1;

    if ((m1.bigrams == NULL) || (m1.unigrams_number <= 0)
            || (m1.unigrams_probabilities == NULL))
    {
        return 0;
    }
    if ((m2.bigrams == NULL) || (m2.unigrams_number <= 0)
            || (m2.unigrams_probabilities == NULL))
    {
        return 0;
    }

    if ((m1.unigrams_number != m2.unigrams_number))
    {
        return 0;
    }

    n = m1.unigrams_number;
    for (i = 0; i < n; i++)
    {
        if (fabs(m1.unigrams_probabilities[i] - m2.unigrams_probabilities[i])
                > FLT_EPSILON)
        {
            res = 0;
            break;
        }
    }
    if (!res)
    {
        return 0;
    }

    for (i = 0; i < m1.unigrams_number; i++)
    {
        n = m1.bigrams[i].number_of_first_words;
        if (n != m2.bigrams[i].number_of_first_words)
        {
            res = 0;
            break;
        }
        for (j = 0; j < n; j++)
        {
            if (m1.bigrams[i].first_words[j] != m2.bigrams[i].first_words[j])
            {
                res = 0;
                break;
            }
            if (fabs(m1.bigrams[i].probabilities[j]
                     - m2.bigrams[i].probabilities[j]) > FLT_EPSILON)
            {
                res = 0;
                break;
            }
        }
        if (!res)
        {
            break;
        }
    }
    return res;
}

int prepare_for_testing_of_load_language_model()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for load_language_model()",
                          init_suite_load_language_model,
                          clean_suite_load_language_model);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             load_language_model_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    load_language_model_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Valid partition 3",
                                    load_language_model_valid_test_3))
            || (NULL == CU_add_test(pSuite, "Valid partition 4",
                                    load_language_model_valid_test_4))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    load_language_model_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_load_language_model()
{
    create_target_language_model();
    if (!save_language_model(correct_language_model_name,
                             target_language_model))
    {
        return -1;
    }
    if (!save_incorrect_language_model1())
    {
        return -1;
    }
    if (!save_incorrect_language_model2())
    {
        return -1;
    }
    return 0;
}

int clean_suite_load_language_model()
{
    free_language_model(&target_language_model);
    remove(correct_language_model_name);
    remove(incorrect_language_model_name1);
    remove(incorrect_language_model_name2);
    return 0;
}

void load_language_model_valid_test_1()
{
    TLanguageModel cur_model;
    int load_res = 0, compare_res = 0;

    cur_model.bigrams = NULL;
    cur_model.unigrams_number = 0;
    cur_model.unigrams_probabilities = NULL;

    load_res = load_language_model(correct_language_model_name,
                                   UNIGRAMS_NUMBER, &cur_model);
    if (load_res)
    {
        compare_res = compare_language_models(cur_model,target_language_model);
    }
    free_language_model(&cur_model);

    CU_ASSERT_TRUE_FATAL(load_res);
    CU_ASSERT_TRUE_FATAL(compare_res);
}

void load_language_model_valid_test_2()
{
    TLanguageModel cur_model;
    int load_res = 1;

    cur_model.bigrams = NULL;
    cur_model.unigrams_number = 0;
    cur_model.unigrams_probabilities = NULL;

    load_res = load_language_model(correct_language_model_name,
                                   UNIGRAMS_NUMBER+1, &cur_model);
    free_language_model(&cur_model);

    CU_ASSERT_FALSE_FATAL(load_res);
}

void load_language_model_valid_test_3()
{
    TLanguageModel cur_model;
    int load_res = 1;

    cur_model.bigrams = NULL;
    cur_model.unigrams_number = 0;
    cur_model.unigrams_probabilities = NULL;

    load_res = load_language_model(incorrect_language_model_name1,
                                   UNIGRAMS_NUMBER+1, &cur_model);
    free_language_model(&cur_model);

    CU_ASSERT_FALSE_FATAL(load_res);
}

void load_language_model_valid_test_4()
{
    TLanguageModel cur_model;
    int load_res = 1;

    cur_model.bigrams = NULL;
    cur_model.unigrams_number = 0;
    cur_model.unigrams_probabilities = NULL;

    load_res = load_language_model(incorrect_language_model_name2,
                                   UNIGRAMS_NUMBER+1, &cur_model);
    free_language_model(&cur_model);

    CU_ASSERT_FALSE_FATAL(load_res);
}

void load_language_model_invalid_test_1()
{
    TLanguageModel cur_model;
    int load_res = 0;

    cur_model.bigrams = NULL;
    cur_model.unigrams_number = 0;
    cur_model.unigrams_probabilities = NULL;

    load_res = load_language_model(NULL, UNIGRAMS_NUMBER, &cur_model);
    free_language_model(&cur_model);
    CU_ASSERT_FALSE_FATAL(load_res);

    load_res = load_language_model(correct_language_model_name, 0, &cur_model);
    free_language_model(&cur_model);
    CU_ASSERT_FALSE_FATAL(load_res);

    load_res = load_language_model(correct_language_model_name,
                                   UNIGRAMS_NUMBER, NULL);
    free_language_model(&cur_model);
    CU_ASSERT_FALSE_FATAL(load_res);
}
