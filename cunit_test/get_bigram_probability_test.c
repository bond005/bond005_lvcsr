#include <float.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "get_bigram_probability_test.h"

static TLanguageModel language_model;
static int incorrect_first_word;
static int incorrect_second_word;

static void create_language_model_for_testing()
{
    language_model.unigrams_number = 4;
    language_model.unigrams_probabilities = malloc(4 * sizeof(float));
    language_model.bigrams_number = 6;
    language_model.bigrams = malloc(6 * sizeof(TWordBigram));

    language_model.bigrams[0].first_word = 1;
    language_model.bigrams[0].second_word = 0;
    language_model.bigrams[0].probability = 0.13;

    language_model.bigrams[1].first_word = 3;
    language_model.bigrams[1].second_word = 0;
    language_model.bigrams[1].probability = 0.2;

    language_model.bigrams[2].first_word = 0;
    language_model.bigrams[2].second_word = 2;
    language_model.bigrams[2].probability = 0.23;

    language_model.bigrams[3].first_word = 3;
    language_model.bigrams[3].second_word = 2;
    language_model.bigrams[3].probability = 0.25;

    language_model.bigrams[4].first_word = 0;
    language_model.bigrams[4].second_word = 3;
    language_model.bigrams[4].probability = 0.11;

    language_model.bigrams[5].first_word = 2;
    language_model.bigrams[5].second_word = 3;
    language_model.bigrams[5].probability = 0.08;

    language_model.unigrams_probabilities[0] = 0.2;
    language_model.unigrams_probabilities[1] = 0.3;
    language_model.unigrams_probabilities[2] = 0.15;
    language_model.unigrams_probabilities[3] = 0.35;

    incorrect_first_word = 1;
    incorrect_second_word = 2;
}

int prepare_for_testing_of_get_bigram_probability()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for get_bigrams_probability()",
                          init_suite_get_bigram_probability,
                          clean_suite_get_bigram_probability);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             get_bigram_probability_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    get_bigram_probability_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Valid partition 3",
                                    get_bigram_probability_valid_test_3))
            || (NULL == CU_add_test(pSuite, "Valid partition 4",
                                    get_bigram_probability_valid_test_4))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    get_bigram_probability_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_get_bigram_probability()
{
    create_language_model_for_testing();
    return 0;
}

int clean_suite_get_bigram_probability()
{
    free_language_model(&language_model);
    return 0;
}

void get_bigram_probability_valid_test_1()
{
    float probability, target_probability;
    int first_word, second_word;

    target_probability = language_model.bigrams[2].probability;
    first_word = language_model.bigrams[2].first_word;
    second_word = language_model.bigrams[2].second_word;
    probability = get_bigram_probability(language_model,
                                         first_word, second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(target_probability, probability, FLT_EPSILON);
}

void get_bigram_probability_valid_test_2()
{
    float probability, target_probability;
    int first_word, second_word;

    target_probability = language_model.bigrams[0].probability;
    first_word = language_model.bigrams[0].first_word;
    second_word = language_model.bigrams[0].second_word;
    probability = get_bigram_probability(language_model,
                                         first_word, second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(target_probability, probability, FLT_EPSILON);
}

void get_bigram_probability_valid_test_3()
{
    float probability, target_probability;
    int first_word, second_word;
    int n;

    n = language_model.bigrams_number;

    target_probability = language_model.bigrams[n-1].probability;
    first_word = language_model.bigrams[n-1].first_word;
    second_word = language_model.bigrams[n-1].second_word;
    probability = get_bigram_probability(language_model,
                                         first_word, second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(target_probability, probability, FLT_EPSILON);
}

void get_bigram_probability_valid_test_4()
{
    float probability;
    probability = get_bigram_probability(
                language_model, incorrect_first_word, incorrect_second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);
}

void get_bigram_probability_invalid_test_1()
{
    float probability;
    TLanguageModel incorrect_model;

    incorrect_model = language_model;
    incorrect_model.bigrams = NULL;
    probability = get_bigram_probability(
                incorrect_model, incorrect_first_word, incorrect_second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);

    incorrect_model = language_model;
    incorrect_model.bigrams_number = 0;
    probability = get_bigram_probability(
                incorrect_model, incorrect_first_word, incorrect_second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);

    incorrect_model = language_model;
    incorrect_model.unigrams_probabilities = NULL;
    probability = get_bigram_probability(
                incorrect_model, incorrect_first_word, incorrect_second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);

    incorrect_model = language_model;
    incorrect_model.unigrams_number = 0;
    probability = get_bigram_probability(
                incorrect_model, incorrect_first_word, incorrect_second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);

    probability = get_bigram_probability(
                language_model, -1, incorrect_second_word);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);

    probability = get_bigram_probability(
                language_model, incorrect_first_word, -1);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(0.0, probability, FLT_EPSILON);
}
