#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "recognize_words_test.h"

#define FILES_NUMBER 1
#define MLF_PART_NAME "test_record.lab"
#define PHONEMES_VOCABULARY_SIZE 3
#define WORDS_VOCABULARY_SIZE 3

static float phonemes_probabilities[] = {
    0.80, 0.05, 0.15,
    0.12, 0.75, 0.13,
    0.04, 0.11, 0.85
};
static TLinearWordsLexicon *words_lexicon = NULL;
static TLanguageModel language_model;
static TMLFFilePart *src_mlf = NULL;
static TMLFFilePart *target_res_mlf = NULL;

static int compare_two_MLF(TMLFFilePart *mlf1, int mlf1_size,
                           TMLFFilePart *mlf2, int mlf2_size)
{
    int i, j, res = 1;

    if (mlf1_size != mlf2_size)
    {
        return 0;
    }
    if (mlf1_size < 0)
    {
        return 0;
    }
    if (mlf1_size == 0)
    {
        return 1;
    }
    if ((mlf1 == NULL) || (mlf2 == NULL))
    {
        return 0;
    }

    for (i = 0; i < mlf1_size; i++)
    {
        if ((mlf1[i].name == NULL) || (mlf2[i].name == NULL))
        {
            res = 0;
            break;
        }
        if (strcmp(mlf1[i].name, mlf2[i].name) != 0)
        {
            res = 0;
            break;
        }
        if (mlf1[i].transcription_size != mlf2[i].transcription_size)
        {
            res = 0;
            break;
        }
        if (mlf1[i].transcription_size < 0)
        {
            res = 0;
            break;
        }
        if (mlf1[i].transcription_size > 0)
        {
            for (j = 0; j < mlf1[i].transcription_size; j++)
            {
                if (mlf1[i].transcription[j].start_time
                        != mlf2[i].transcription[j].start_time)
                {
                    res = 0;
                    break;
                }
                if (mlf1[i].transcription[j].end_time
                        != mlf2[i].transcription[j].end_time)
                {
                    res = 0;
                    break;
                }
                if (mlf1[i].transcription[j].node_data
                        != mlf2[i].transcription[j].node_data)
                {
                    res = 0;
                    break;
                }
                if (fabs(mlf1[i].transcription[j].probability
                         - mlf2[i].transcription[j].probability) > FLT_EPSILON)
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
    }

    return res;
}

int prepare_for_testing_of_recognize_words()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for recognize_words()",
                          init_suite_recognize_words,
                          clean_suite_recognize_words);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition",
                             recognize_words_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    recognize_words_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_recognize_words()
{
    words_lexicon = malloc(WORDS_VOCABULARY_SIZE*sizeof(TLinearWordsLexicon));
    words_lexicon[0].word_index = 0;
    words_lexicon[0].phonemes_number = 2;
    words_lexicon[0].phonemes_indexes = malloc(2*sizeof(int));
    words_lexicon[0].phonemes_indexes[0] = 0;
    words_lexicon[0].phonemes_indexes[1] = 1;
    words_lexicon[1].word_index = 1;
    words_lexicon[1].phonemes_number = 2;
    words_lexicon[1].phonemes_indexes = malloc(2*sizeof(int));
    words_lexicon[1].phonemes_indexes[0] = 2;
    words_lexicon[1].phonemes_indexes[1] = 1;
    words_lexicon[2].word_index = 2;
    words_lexicon[2].phonemes_number = 3;
    words_lexicon[2].phonemes_indexes = malloc(3*sizeof(int));
    words_lexicon[2].phonemes_indexes[0] = 1;
    words_lexicon[2].phonemes_indexes[1] = 2;
    words_lexicon[2].phonemes_indexes[2] = 0;

    language_model.unigrams_number = 3;
    language_model.unigrams_probabilities = malloc(3*sizeof(float));
    language_model.unigrams_probabilities[0] = 0.4;
    language_model.unigrams_probabilities[1] = 0.25;
    language_model.unigrams_probabilities[2] = 0.35;
    language_model.bigrams_number = 6;
    language_model.bigrams = malloc(6*sizeof(TWordBigram));
    language_model.bigrams[0].first_word = 0;
    language_model.bigrams[0].second_word = 1;
    language_model.bigrams[0].probability = 0.2;
    language_model.bigrams[1].first_word = 0;
    language_model.bigrams[1].second_word = 2;
    language_model.bigrams[1].probability = 0.8;
    language_model.bigrams[2].first_word = 1;
    language_model.bigrams[2].second_word = 0;
    language_model.bigrams[2].probability = 0.5;
    language_model.bigrams[3].first_word = 1;
    language_model.bigrams[3].second_word = 2;
    language_model.bigrams[3].probability = 0.5;
    language_model.bigrams[4].first_word = 2;
    language_model.bigrams[4].second_word = 0;
    language_model.bigrams[4].probability = 0.1;
    language_model.bigrams[5].first_word = 2;
    language_model.bigrams[5].second_word = 1;
    language_model.bigrams[5].probability = 0.9;

    src_mlf = malloc(sizeof(TMLFFilePart));
    src_mlf[0].name = malloc((strlen(MLF_PART_NAME)+1)*sizeof(char));
    memset(src_mlf[0].name, 0, (strlen(MLF_PART_NAME)+1)*sizeof(char));
    strcpy(src_mlf[0].name, MLF_PART_NAME);
    src_mlf[0].transcription_size = 14;
    src_mlf[0].transcription = malloc(14*sizeof(TTranscriptionNode));
    src_mlf[0].transcription[0].start_time = 0;
    src_mlf[0].transcription[0].end_time = 100000;
    src_mlf[0].transcription[0].node_data = 0;
    src_mlf[0].transcription[0].probability = 0.9;
    src_mlf[0].transcription[1].start_time = 100000;
    src_mlf[0].transcription[1].end_time = 200000;
    src_mlf[0].transcription[1].node_data = 0;
    src_mlf[0].transcription[1].probability = 0.8;
    src_mlf[0].transcription[2].start_time = 200000;
    src_mlf[0].transcription[2].end_time = 300000;
    src_mlf[0].transcription[2].node_data = 0;
    src_mlf[0].transcription[2].probability = 0.85;
    src_mlf[0].transcription[3].start_time = 300000;
    src_mlf[0].transcription[3].end_time = 400000;
    src_mlf[0].transcription[3].node_data = 2;
    src_mlf[0].transcription[3].probability = 0.6;
    src_mlf[0].transcription[4].start_time = 400000;
    src_mlf[0].transcription[4].end_time = 500000;
    src_mlf[0].transcription[4].node_data = 1;
    src_mlf[0].transcription[4].probability = 0.75;
    src_mlf[0].transcription[5].start_time = 500000;
    src_mlf[0].transcription[5].end_time = 600000;
    src_mlf[0].transcription[5].node_data = 1;
    src_mlf[0].transcription[5].probability = 0.9;
    src_mlf[0].transcription[6].start_time = 600000;
    src_mlf[0].transcription[6].end_time = 700000;
    src_mlf[0].transcription[6].node_data = 1;
    src_mlf[0].transcription[6].probability = 0.8;
    src_mlf[0].transcription[7].start_time = 700000;
    src_mlf[0].transcription[7].end_time = 800000;
    src_mlf[0].transcription[7].node_data = 2;
    src_mlf[0].transcription[7].probability = 0.9;
    src_mlf[0].transcription[8].start_time = 800000;
    src_mlf[0].transcription[8].end_time = 900000;
    src_mlf[0].transcription[8].node_data = 2;
    src_mlf[0].transcription[8].probability = 0.9;
    src_mlf[0].transcription[9].start_time = 900000;
    src_mlf[0].transcription[9].end_time = 1000000;
    src_mlf[0].transcription[9].node_data = 2;
    src_mlf[0].transcription[9].probability = 0.75;
    src_mlf[0].transcription[10].start_time = 1000000;
    src_mlf[0].transcription[10].end_time = 1100000;
    src_mlf[0].transcription[10].node_data = 2;
    src_mlf[0].transcription[10].probability = 0.8;
    src_mlf[0].transcription[11].start_time = 1100000;
    src_mlf[0].transcription[11].end_time = 1200000;
    src_mlf[0].transcription[11].node_data = 2;
    src_mlf[0].transcription[11].probability = 0.85;
    src_mlf[0].transcription[12].start_time = 1200000;
    src_mlf[0].transcription[12].end_time = 1300000;
    src_mlf[0].transcription[12].node_data = 0;
    src_mlf[0].transcription[12].probability = 0.7;
    src_mlf[0].transcription[13].start_time = 1300000;
    src_mlf[0].transcription[13].end_time = 1400000;
    src_mlf[0].transcription[13].node_data = 0;
    src_mlf[0].transcription[13].probability = 0.8;

    target_res_mlf = malloc(sizeof(TMLFFilePart));
    target_res_mlf[0].name = malloc((strlen(MLF_PART_NAME)+1)*sizeof(char));
    memset(target_res_mlf[0].name, 0, (strlen(MLF_PART_NAME)+1)*sizeof(char));
    strcpy(target_res_mlf[0].name, MLF_PART_NAME);
    target_res_mlf[0].transcription_size = 2;
    target_res_mlf[0].transcription = malloc(2*sizeof(TTranscriptionNode));
    target_res_mlf[0].transcription[0].start_time = -1;
    target_res_mlf[0].transcription[0].end_time = -1;
    target_res_mlf[0].transcription[0].node_data = 0;
    target_res_mlf[0].transcription[0].probability = 1.0;
    target_res_mlf[0].transcription[1].start_time = -1;
    target_res_mlf[0].transcription[1].end_time = -1;
    target_res_mlf[0].transcription[1].node_data = 2;
    target_res_mlf[0].transcription[1].probability = 1.0;

    return 0;
}

int clean_suite_recognize_words()
{
    free_language_model(&language_model);
    free_linear_words_lexicon(&words_lexicon, WORDS_VOCABULARY_SIZE);
    free_MLF(&src_mlf, FILES_NUMBER);
    free_MLF(&target_res_mlf, FILES_NUMBER);
    return 0;
}

void recognize_words_valid_test_1()
{
    TMLFFilePart *recognition_res = NULL;
    int i, is_ok = 0, is_equal = 0;

    is_ok = recognize_words(src_mlf, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            words_lexicon, language_model, &recognition_res);
    if (is_ok)
    {
        is_equal = compare_two_MLF(recognition_res, FILES_NUMBER,
                                   target_res_mlf, FILES_NUMBER);
        if (recognition_res[0].transcription_size > 0)
        {
            printf("\n%d", recognition_res[0].transcription[0].node_data);
            for (i = 1; i < recognition_res[0].transcription_size; i++)
            {
                printf(" -> %d", recognition_res[0].transcription[i].node_data);
            }
            printf("\n");
        }
    }
    free_MLF(&recognition_res, FILES_NUMBER);

    CU_ASSERT_TRUE_FATAL(is_ok);
    CU_ASSERT_TRUE_FATAL(is_equal);
}

void recognize_words_invalid_test_1()
{
    TMLFFilePart *recognition_res = NULL;
    int is_ok = 1;
    TLanguageModel incorrect_model = language_model;

    incorrect_model.bigrams = NULL;

    is_ok = recognize_words(NULL, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            words_lexicon, language_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, 0, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            words_lexicon, language_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, FILES_NUMBER, 0,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            words_lexicon, language_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            NULL, WORDS_VOCABULARY_SIZE,
                            words_lexicon, language_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, 0,
                            words_lexicon, language_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            NULL, language_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            words_lexicon, incorrect_model, &recognition_res);
    free_MLF(&recognition_res, FILES_NUMBER);
    CU_ASSERT_FALSE_FATAL(is_ok);

    is_ok = recognize_words(src_mlf, FILES_NUMBER, PHONEMES_VOCABULARY_SIZE,
                            phonemes_probabilities, WORDS_VOCABULARY_SIZE,
                            words_lexicon, language_model, NULL);
    CU_ASSERT_FALSE_FATAL(is_ok);
}
