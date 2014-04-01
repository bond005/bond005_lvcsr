#include <float.h>
#include <stdlib.h>
#include <time.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "calculate_language_model_test.h"

#define FILES_NUMBER 3
#define WORDS_NUMBER 10

static TMLFFilePart words_MLF_data[FILES_NUMBER];
static char *names_of_files[FILES_NUMBER] = { "file1.lab", "file2.lab",
                                              "file3.lab" };
static float lambda = 1.0;
static float eps = 0.0;
static TLanguageModel target_language_model;

static void initialize_words_MLF_data()
{
    int i, j, n;

    n = strlen(names_of_files[0]);
    words_MLF_data[0].name = malloc((n+1)*sizeof(char));
    memset(words_MLF_data[0].name, 0, (n+1)*sizeof(char));
    strcpy(words_MLF_data[0].name, names_of_files[0]);
    words_MLF_data[0].transcription_size = 12;
    words_MLF_data[0].transcription = malloc(12*sizeof(TTranscriptionNode));
    words_MLF_data[0].transcription[0].node_data = 3;
    words_MLF_data[0].transcription[1].node_data = 9;
    words_MLF_data[0].transcription[2].node_data = 0;
    words_MLF_data[0].transcription[3].node_data = 1;
    words_MLF_data[0].transcription[4].node_data = 2;
    words_MLF_data[0].transcription[5].node_data = 5;
    words_MLF_data[0].transcription[6].node_data = 7;
    words_MLF_data[0].transcription[7].node_data = 5;
    words_MLF_data[0].transcription[8].node_data = 8;
    words_MLF_data[0].transcription[9].node_data = 9;
    words_MLF_data[0].transcription[10].node_data = 6;
    words_MLF_data[0].transcription[11].node_data = 4;

    n = strlen(names_of_files[1]);
    words_MLF_data[1].name = malloc((n+1)*sizeof(char));
    memset(words_MLF_data[1].name, 0, (n+1)*sizeof(char));
    strcpy(words_MLF_data[1].name, names_of_files[1]);
    words_MLF_data[1].transcription_size = 10;
    words_MLF_data[1].transcription = malloc(10*sizeof(TTranscriptionNode));
    words_MLF_data[1].transcription[0].node_data = 5;
    words_MLF_data[1].transcription[1].node_data = 7;
    words_MLF_data[1].transcription[2].node_data = 6;
    words_MLF_data[1].transcription[3].node_data = 3;
    words_MLF_data[1].transcription[4].node_data = 9;
    words_MLF_data[1].transcription[5].node_data = 2;
    words_MLF_data[1].transcription[6].node_data = 5;
    words_MLF_data[1].transcription[7].node_data = 0;
    words_MLF_data[1].transcription[8].node_data = 1;
    words_MLF_data[1].transcription[9].node_data = 6;

    n = strlen(names_of_files[2]);
    words_MLF_data[2].name = malloc((n+1)*sizeof(char));
    memset(words_MLF_data[2].name, 0, (n+1)*sizeof(char));
    strcpy(words_MLF_data[2].name, names_of_files[2]);
    words_MLF_data[2].transcription_size = 15;
    words_MLF_data[2].transcription = malloc(15*sizeof(TTranscriptionNode));
    words_MLF_data[2].transcription[0].node_data = 5;
    words_MLF_data[2].transcription[1].node_data = 0;
    words_MLF_data[2].transcription[2].node_data = 9;
    words_MLF_data[2].transcription[3].node_data = 1;
    words_MLF_data[2].transcription[4].node_data = 2;
    words_MLF_data[2].transcription[5].node_data = 6;
    words_MLF_data[2].transcription[6].node_data = 4;
    words_MLF_data[2].transcription[7].node_data = 2;
    words_MLF_data[2].transcription[8].node_data = 3;
    words_MLF_data[2].transcription[9].node_data = 9;
    words_MLF_data[2].transcription[10].node_data = 8;
    words_MLF_data[2].transcription[11].node_data = 1;
    words_MLF_data[2].transcription[12].node_data = 6;
    words_MLF_data[2].transcription[13].node_data = 4;
    words_MLF_data[2].transcription[14].node_data = 2;

    for (i = 0; i < FILES_NUMBER; i++)
    {
        for (j = 0; j < words_MLF_data[i].transcription_size; j++)
        {
            words_MLF_data[i].transcription[j].start_time = -1;
            words_MLF_data[i].transcription[j].end_time = -1;
            words_MLF_data[i].transcription[j].probability = 1.0;
        }
    }
}

static void free_words_MLF_data()
{
    int i;
    for (i = 0; i < FILES_NUMBER; i++)
    {
        if (words_MLF_data[i].name != NULL)
        {
            free(words_MLF_data[i].name);
            words_MLF_data[i].name = NULL;
        }
        if (words_MLF_data[i].transcription != NULL)
        {
            free(words_MLF_data[i].transcription);
            words_MLF_data[i].transcription = NULL;
        }
    }
}

static void create_target_language_model()
{
    target_language_model.unigrams_number = WORDS_NUMBER;
    target_language_model.unigrams_probabilities
            = malloc(WORDS_NUMBER * sizeof(float));
    target_language_model.unigrams_probabilities[0] = 3.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[1] = 4.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[2] = 5.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[3] = 3.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[4] = 3.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[5] = 5.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[6] = 5.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[7] = 2.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[8] = 2.0/(12.0+10.0+15.0);
    target_language_model.unigrams_probabilities[9] = 5.0/(12.0+10.0+15.0);

    target_language_model.bigrams_number = 23;
    target_language_model.bigrams
            = malloc(target_language_model.bigrams_number*sizeof(TWordBigram));

    target_language_model.bigrams[0].first_word = 0;
    target_language_model.bigrams[0].second_word = 1;
    target_language_model.bigrams[0].probability = 2.0 / 3.0;

    target_language_model.bigrams[1].first_word = 0;
    target_language_model.bigrams[1].second_word = 9;
    target_language_model.bigrams[1].probability = 1.0 / 3.0;

    target_language_model.bigrams[2].first_word = 1;
    target_language_model.bigrams[2].second_word = 2;
    target_language_model.bigrams[2].probability = 2.0 / 4.0;

    target_language_model.bigrams[3].first_word = 1;
    target_language_model.bigrams[3].second_word = 6;
    target_language_model.bigrams[3].probability = 2.0 / 4.0;

    target_language_model.bigrams[4].first_word = 2;
    target_language_model.bigrams[4].second_word = 3;
    target_language_model.bigrams[4].probability = 1.0 / 5.0;

    target_language_model.bigrams[5].first_word = 2;
    target_language_model.bigrams[5].second_word = 5;
    target_language_model.bigrams[5].probability = 2.0 / 5.0;

    target_language_model.bigrams[6].first_word = 2;
    target_language_model.bigrams[6].second_word = 6;
    target_language_model.bigrams[6].probability = 1.0 / 5.0;

    target_language_model.bigrams[7].first_word = 3;
    target_language_model.bigrams[7].second_word = 9;
    target_language_model.bigrams[7].probability = 3.0 / 3.0;

    target_language_model.bigrams[8].first_word = 4;
    target_language_model.bigrams[8].second_word = 2;
    target_language_model.bigrams[8].probability = 2.0 / 3.0;

    target_language_model.bigrams[9].first_word = 5;
    target_language_model.bigrams[9].second_word = 0;
    target_language_model.bigrams[9].probability = 2.0 / 5.0;

    target_language_model.bigrams[10].first_word = 5;
    target_language_model.bigrams[10].second_word = 7;
    target_language_model.bigrams[10].probability = 2.0 / 5.0;

    target_language_model.bigrams[11].first_word = 5;
    target_language_model.bigrams[11].second_word = 8;
    target_language_model.bigrams[11].probability = 1.0 / 5.0;

    target_language_model.bigrams[12].first_word = 6;
    target_language_model.bigrams[12].second_word = 3;
    target_language_model.bigrams[12].probability = 1.0 / 5.0;

    target_language_model.bigrams[13].first_word = 6;
    target_language_model.bigrams[13].second_word = 4;
    target_language_model.bigrams[13].probability = 3.0 / 5.0;

    target_language_model.bigrams[14].first_word = 7;
    target_language_model.bigrams[14].second_word = 5;
    target_language_model.bigrams[14].probability = 1.0 / 2.0;

    target_language_model.bigrams[15].first_word = 7;
    target_language_model.bigrams[15].second_word = 6;
    target_language_model.bigrams[15].probability = 1.0 / 2.0;

    target_language_model.bigrams[16].first_word = 8;
    target_language_model.bigrams[16].second_word = 1;
    target_language_model.bigrams[16].probability = 1.0 / 2.0;

    target_language_model.bigrams[17].first_word = 8;
    target_language_model.bigrams[17].second_word = 9;
    target_language_model.bigrams[17].probability = 1.0 / 2.0;

    target_language_model.bigrams[18].first_word = 9;
    target_language_model.bigrams[18].second_word = 0;
    target_language_model.bigrams[18].probability = 1.0 / 5.0;

    target_language_model.bigrams[19].first_word = 9;
    target_language_model.bigrams[19].second_word = 1;
    target_language_model.bigrams[19].probability = 1.0 / 5.0;

    target_language_model.bigrams[20].first_word = 9;
    target_language_model.bigrams[20].second_word = 2;
    target_language_model.bigrams[20].probability = 1.0 / 5.0;

    target_language_model.bigrams[21].first_word = 9;
    target_language_model.bigrams[21].second_word = 6;
    target_language_model.bigrams[21].probability = 1.0 / 5.0;

    target_language_model.bigrams[22].first_word = 9;
    target_language_model.bigrams[22].second_word = 8;
    target_language_model.bigrams[22].probability = 1.0 / 5.0;
}

static int compare_language_models(TLanguageModel m1, TLanguageModel m2)
{
    int i, n, res = 1;

    if ((m1.bigrams_number <= 0)||(m1.bigrams == NULL)
            || (m1.unigrams_number <= 0)||(m1.unigrams_probabilities == NULL))
    {
        return 0;
    }
    if ((m2.bigrams_number <= 0)||(m2.bigrams == NULL)
            || (m2.unigrams_number <= 0)||(m2.unigrams_probabilities == NULL))
    {
        return 0;
    }

    if ((m1.unigrams_number != m2.unigrams_number)
            || (m1.bigrams_number != m2.bigrams_number))
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

    n = m1.bigrams_number;
    for (i = 0; i < n; i++)
    {
        if (m1.bigrams[i].first_word != m2.bigrams[i].first_word)
        {
            res = 0;
            break;
        }
        if (m1.bigrams[i].second_word != m2.bigrams[i].second_word)
        {
            res = 0;
            break;
        }
        if (fabs(m1.bigrams[i].probability - m2.bigrams[i].probability)
                > FLT_EPSILON)
        {
            res = 0;
            break;
        }
    }
    return res;
}

int prepare_for_testing_of_calculate_language_model()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for calculate_language_model()",
                          init_suite_calculate_language_model,
                          clean_suite_calculate_language_model);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             calculate_language_model_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    calculate_language_model_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_calculate_language_model()
{
    initialize_words_MLF_data();
    create_target_language_model();
    return 0;
}

int clean_suite_calculate_language_model()
{
    free_words_MLF_data();
    free_language_model(&target_language_model);
    return 0;
}

void calculate_language_model_valid_test_1()
{
    TLanguageModel calculated_model;
    int calculated_res = 0, compare_res = 0;

    calculated_model.bigrams = NULL;
    calculated_model.bigrams_number = 0;
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda, eps,
                &calculated_model);
    if (calculated_res)
    {
        compare_res = compare_language_models(target_language_model,
                                              calculated_model);
    }
    free_language_model(&calculated_model);

    CU_ASSERT_TRUE_FATAL(calculated_res);
    CU_ASSERT_TRUE_FATAL(compare_res);
}

void calculate_language_model_invalid_test_1()
{
    TLanguageModel calculated_model;
    int calculated_res = 0;

    calculated_model.bigrams = NULL;
    calculated_model.bigrams_number = 0;
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                NULL, FILES_NUMBER, WORDS_NUMBER, lambda, eps,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, 0, WORDS_NUMBER, lambda, eps,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, 0, lambda, eps,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, -0.5, eps,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda, -0.5,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda, eps,
                NULL);
    CU_ASSERT_FALSE_FATAL(calculated_res);
}
