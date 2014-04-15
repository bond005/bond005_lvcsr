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
static float eps1 = 0.0;
static float eps2 = 0.3;
static TLanguageModel target_language_model1;
static TLanguageModel target_language_model2;

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
            words_MLF_data[i].transcription[j].start_time = 0;
            words_MLF_data[i].transcription[j].end_time = 0;
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

static void create_target_language_model1()
{
    int i;

    target_language_model1.unigrams_number = WORDS_NUMBER;
    target_language_model1.unigrams_probabilities
            = malloc(WORDS_NUMBER * sizeof(float));
    target_language_model1.unigrams_probabilities[0] = 3.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[1] = 4.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[2] = 5.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[3] = 3.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[4] = 3.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[5] = 5.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[6] = 5.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[7] = 2.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[8] = 2.0/(12.0+10.0+15.0);
    target_language_model1.unigrams_probabilities[9] = 5.0/(12.0+10.0+15.0);

    target_language_model1.bigrams = malloc(WORDS_NUMBER * sizeof(TWordBigram));
    for (i = 0; i < WORDS_NUMBER; i++)
    {
        target_language_model1.bigrams[i].begins_number = 0;
        target_language_model1.bigrams[i].begins = NULL;
    }

    target_language_model1.bigrams[0].begins_number = 2;
    target_language_model1.bigrams[0].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    // 5->0
    target_language_model1.bigrams[0].begins[0].word_i = 5;
    target_language_model1.bigrams[0].begins[0].probability = 2.0 / 5.0;

    // 9->0
    target_language_model1.bigrams[0].begins[1].word_i = 9;
    target_language_model1.bigrams[0].begins[1].probability = 1.0 / 5.0;

    target_language_model1.bigrams[1].begins_number = 3;
    target_language_model1.bigrams[1].begins
            = malloc(3 * sizeof(TWordBigram));

    // 0->1
    target_language_model1.bigrams[1].begins[0].word_i = 0;
    target_language_model1.bigrams[1].begins[0].probability = 2.0 / 3.0;

    // 8->1
    target_language_model1.bigrams[1].begins[1].word_i = 8;
    target_language_model1.bigrams[1].begins[1].probability = 1.0 / 2.0;

    // 9->1
    target_language_model1.bigrams[1].begins[2].word_i = 9;
    target_language_model1.bigrams[1].begins[2].probability = 1.0 / 5.0;

    target_language_model1.bigrams[2].begins_number = 3;
    target_language_model1.bigrams[2].begins
            = malloc(3 * sizeof(TWordBigramBegin));

    // 1->2
    target_language_model1.bigrams[2].begins[0].word_i = 1;
    target_language_model1.bigrams[2].begins[0].probability = 2.0 / 4.0;

    // 4->2
    target_language_model1.bigrams[2].begins[1].word_i = 4;
    target_language_model1.bigrams[2].begins[1].probability = 2.0 / 3.0;

    // 9->2
    target_language_model1.bigrams[2].begins[2].word_i = 9;
    target_language_model1.bigrams[2].begins[2].probability = 1.0 / 5.0;

    target_language_model1.bigrams[3].begins_number = 2;
    target_language_model1.bigrams[3].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    // 2->3
    target_language_model1.bigrams[3].begins[0].word_i = 2;
    target_language_model1.bigrams[3].begins[0].probability = 1.0 / 5.0;

    // 6->3
    target_language_model1.bigrams[3].begins[1].word_i = 6;
    target_language_model1.bigrams[3].begins[1].probability = 1.0 / 5.0;

    target_language_model1.bigrams[4].begins_number = 1;
    target_language_model1.bigrams[4].begins
            = malloc(1 * sizeof(TWordBigramBegin));

    // 6->4
    target_language_model1.bigrams[4].begins[0].word_i = 6;
    target_language_model1.bigrams[4].begins[0].probability = 3.0 / 5.0;

    target_language_model1.bigrams[5].begins_number = 2;
    target_language_model1.bigrams[5].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    // 2->5
    target_language_model1.bigrams[5].begins[0].word_i = 2;
    target_language_model1.bigrams[5].begins[0].probability = 2.0 / 5.0;

    // 7->5
    target_language_model1.bigrams[5].begins[1].word_i = 7;
    target_language_model1.bigrams[5].begins[1].probability = 1.0 / 2.0;

    target_language_model1.bigrams[6].begins_number = 4;
    target_language_model1.bigrams[6].begins
            = malloc(4 * sizeof(TWordBigramBegin));

    // 1->6
    target_language_model1.bigrams[6].begins[0].word_i = 1;
    target_language_model1.bigrams[6].begins[0].probability = 2.0 / 4.0;

    // 2->6
    target_language_model1.bigrams[6].begins[1].word_i = 2;
    target_language_model1.bigrams[6].begins[1].probability = 1.0 / 5.0;

    // 7->6
    target_language_model1.bigrams[6].begins[2].word_i = 7;
    target_language_model1.bigrams[6].begins[2].probability = 1.0 / 2.0;

    // 9->6
    target_language_model1.bigrams[6].begins[3].word_i = 9;
    target_language_model1.bigrams[6].begins[3].probability = 1.0 / 5.0;

    target_language_model1.bigrams[7].begins_number = 1;
    target_language_model1.bigrams[7].begins
            = malloc(1 * sizeof(TWordBigramBegin));

    // 5->7
    target_language_model1.bigrams[7].begins[0].word_i = 5;
    target_language_model1.bigrams[7].begins[0].probability = 2.0 / 5.0;

    target_language_model1.bigrams[8].begins_number = 2;
    target_language_model1.bigrams[8].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    // 5->8
    target_language_model1.bigrams[8].begins[0].word_i = 5;
    target_language_model1.bigrams[8].begins[0].probability = 1.0 / 5.0;

    // 9->8
    target_language_model1.bigrams[8].begins[1].word_i = 9;
    target_language_model1.bigrams[8].begins[1].probability = 1.0 / 5.0;

    target_language_model1.bigrams[9].begins_number = 3;
    target_language_model1.bigrams[9].begins
            = malloc(3 * sizeof(TWordBigramBegin));

    // 0->9
    target_language_model1.bigrams[9].begins[0].word_i = 0;
    target_language_model1.bigrams[9].begins[0].probability = 1.0 / 3.0;

    // 3->9
    target_language_model1.bigrams[9].begins[1].word_i = 3;
    target_language_model1.bigrams[9].begins[1].probability = 3.0 / 3.0;

    target_language_model1.bigrams[9].begins[2].word_i = 8;
    target_language_model1.bigrams[9].begins[2].probability = 1.0 / 2.0;
}

static void create_target_language_model2()
{
    int i, j, n;

    target_language_model2.unigrams_number = WORDS_NUMBER;
    target_language_model2.unigrams_probabilities
            = malloc(WORDS_NUMBER * sizeof(float));
    target_language_model2.unigrams_probabilities[0] = 3.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[1] = 4.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[2] = 5.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[3] = 3.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[4] = 3.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[5] = 5.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[6] = 5.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[7] = 2.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[8] = 2.0/(12.0+10.0+15.0);
    target_language_model2.unigrams_probabilities[9] = 5.0/(12.0+10.0+15.0);

    target_language_model2.bigrams = malloc(WORDS_NUMBER * sizeof(TWordBigram));
    for (i = 0; i < WORDS_NUMBER; i++)
    {
        target_language_model2.bigrams[i].begins_number = 0;
        target_language_model2.bigrams[i].begins = NULL;
    }

    target_language_model2.bigrams[0].begins_number = 2;
    target_language_model2.bigrams[0].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[0].begins[0].word_i = 5;
    target_language_model2.bigrams[0].begins[0].probability = 2.0 / 5.0;

    target_language_model2.bigrams[0].begins[1].word_i = 9;
    target_language_model2.bigrams[0].begins[1].probability = 1.0 / 5.0;

    target_language_model2.bigrams[1].begins_number = 3;
    target_language_model2.bigrams[1].begins
            = malloc(3 * sizeof(TWordBigram));

    target_language_model2.bigrams[1].begins[0].word_i = 0;
    target_language_model2.bigrams[1].begins[0].probability = 2.0 / 3.0;

    target_language_model2.bigrams[1].begins[1].word_i = 8;
    target_language_model2.bigrams[1].begins[1].probability = 1.0 / 2.0;

    target_language_model2.bigrams[1].begins[2].word_i = 9;
    target_language_model2.bigrams[1].begins[2].probability = 1.0 / 5.0;

    target_language_model2.bigrams[2].begins_number = 3;
    target_language_model2.bigrams[2].begins
            = malloc(3 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[2].begins[0].word_i = 1;
    target_language_model2.bigrams[2].begins[0].probability = 2.0 / 4.0;

    target_language_model2.bigrams[2].begins[1].word_i = 4;
    target_language_model2.bigrams[2].begins[1].probability = 2.0 / 3.0;

    target_language_model2.bigrams[2].begins[2].word_i = 9;
    target_language_model2.bigrams[2].begins[2].probability = 1.0 / 5.0;

    target_language_model2.bigrams[3].begins_number = 2;
    target_language_model2.bigrams[3].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[3].begins[0].word_i = 2;
    target_language_model2.bigrams[3].begins[0].probability = 1.0 / 5.0;

    target_language_model2.bigrams[3].begins[1].word_i = 6;
    target_language_model2.bigrams[3].begins[1].probability = 1.0 / 5.0;

    target_language_model2.bigrams[4].begins_number = 1;
    target_language_model2.bigrams[4].begins
            = malloc(1 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[4].begins[0].word_i = 6;
    target_language_model2.bigrams[4].begins[0].probability = 3.0 / 5.0;

    target_language_model2.bigrams[5].begins_number = 2;
    target_language_model2.bigrams[5].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[5].begins[0].word_i = 2;
    target_language_model2.bigrams[5].begins[0].probability = 2.0 / 5.0;

    target_language_model2.bigrams[5].begins[1].word_i = 7;
    target_language_model2.bigrams[5].begins[1].probability = 1.0 / 2.0;

    target_language_model2.bigrams[6].begins_number = 4;
    target_language_model2.bigrams[6].begins
            = malloc(4 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[6].begins[0].word_i = 1;
    target_language_model2.bigrams[6].begins[0].probability = 2.0 / 4.0;

    target_language_model2.bigrams[6].begins[1].word_i = 2;
    target_language_model2.bigrams[6].begins[1].probability = 1.0 / 5.0;

    target_language_model2.bigrams[6].begins[2].word_i = 7;
    target_language_model2.bigrams[6].begins[2].probability = 1.0 / 2.0;

    target_language_model2.bigrams[6].begins[3].word_i = 9;
    target_language_model2.bigrams[6].begins[3].probability = 1.0 / 5.0;

    target_language_model2.bigrams[7].begins_number = 1;
    target_language_model2.bigrams[7].begins
            = malloc(1 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[7].begins[0].word_i = 5;
    target_language_model2.bigrams[7].begins[0].probability = 2.0 / 5.0;

    target_language_model2.bigrams[8].begins_number = 2;
    target_language_model2.bigrams[8].begins
            = malloc(2 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[8].begins[0].word_i = 5;
    target_language_model2.bigrams[8].begins[0].probability = 1.0 / 5.0;

    target_language_model2.bigrams[8].begins[1].word_i = 9;
    target_language_model2.bigrams[8].begins[1].probability = 1.0 / 5.0;

    target_language_model2.bigrams[9].begins_number = 3;
    target_language_model2.bigrams[9].begins
            = malloc(3 * sizeof(TWordBigramBegin));

    target_language_model2.bigrams[9].begins[0].word_i = 0;
    target_language_model2.bigrams[9].begins[0].probability = 1.0 / 3.0;

    target_language_model2.bigrams[9].begins[1].word_i = 3;
    target_language_model2.bigrams[9].begins[1].probability = 3.0 / 3.0;

    target_language_model2.bigrams[9].begins[2].word_i = 8;
    target_language_model2.bigrams[9].begins[2].probability = 1.0 / 2.0;

    for (i = 0; i < target_language_model2.unigrams_number; i++)
    {
        j = 0;
        while (j < target_language_model2.bigrams[i].begins_number)
        {
            if (target_language_model2.bigrams[i].begins[j].probability>=eps2)
            {
                j++;
            }
            else
            {
                n = target_language_model2.bigrams[i].begins_number;
                memmove(target_language_model2.bigrams[i].begins+j,
                        target_language_model2.bigrams[i].begins+j+1,
                        (n-j-1) * sizeof(TWordBigramBegin));
                target_language_model2.bigrams[i].begins_number--;
            }
        }
    }
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
        n = m1.bigrams[i].begins_number;
        if (n != m2.bigrams[i].begins_number)
        {
            res = 0;
            break;
        }
        for (j = 0; j < n; j++)
        {
            if (m1.bigrams[i].begins[j].word_i!=m2.bigrams[i].begins[j].word_i)
            {
                res = 0;
                break;
            }
            if (fabs(m1.bigrams[i].begins[j].probability
                     - m2.bigrams[i].begins[j].probability) > FLT_EPSILON)
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
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    calculate_language_model_valid_test_2))
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
    create_target_language_model1();
    create_target_language_model2();
    return 0;
}

int clean_suite_calculate_language_model()
{
    free_words_MLF_data();
    free_language_model(&target_language_model1);
    free_language_model(&target_language_model2);
    return 0;
}

void calculate_language_model_valid_test_1()
{
    TLanguageModel calculated_model;
    int calculated_res = 0, compare_res = 0;

    calculated_model.bigrams = NULL;
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, eps1,
                &calculated_model);
    if (calculated_res)
    {
        compare_res = compare_language_models(target_language_model1,
                                              calculated_model);
    }
    free_language_model(&calculated_model);

    CU_ASSERT_TRUE_FATAL(calculated_res);
    CU_ASSERT_TRUE_FATAL(compare_res);
}

void calculate_language_model_valid_test_2()
{
    TLanguageModel calculated_model;
    int calculated_res = 0, compare_res = 0;

    calculated_model.bigrams = NULL;
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, eps2,
                &calculated_model);
    if (calculated_res)
    {
        compare_res = compare_language_models(target_language_model2,
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
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                NULL, FILES_NUMBER, WORDS_NUMBER, eps1, &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, 0, WORDS_NUMBER, eps1, &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, 0, eps1, &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, -0.5,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, eps1, NULL);
    CU_ASSERT_FALSE_FATAL(calculated_res);
}
