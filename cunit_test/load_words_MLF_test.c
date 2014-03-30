#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "load_words_MLF_test.h"

#define VOCABULARY_SIZE 10

static char *name_of_correct_MLF_file = "correct_words_data.mlf";
static char *name_of_incorrect_MLF_file_1 = "incorrect_words_data1.mlf";
static char *name_of_incorrect_MLF_file_2 = "incorrect_words_data2.mlf";
static char *name_of_incorrect_MLF_file_3 = "incorrect_words_data3.mlf";
static TMLFFilePart *target_MLF = NULL;
static int target_MLF_size = 3;
static char *words_vocabulary[VOCABULARY_SIZE];

static void create_words_vocabulary()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        words_vocabulary[i] = malloc(4 * sizeof(char));
        words_vocabulary[i][0] = (char)((int)'a' + i);
        words_vocabulary[i][1] = words_vocabulary[i][0];
        words_vocabulary[i][2] = words_vocabulary[i][1];
        words_vocabulary[i][3] = 0;
    }
}

static int create_MLF_transcription(PTranscriptionNode *created_transcription)
{
    int j, res = rand() % 20 + 2;
    PTranscriptionNode transcription;

    *created_transcription = malloc(res * sizeof(TTranscriptionNode));
    transcription = *created_transcription;

    for (j = 0; j < res; j++)
    {
        transcription[j].start_time = -1;
        transcription[j].end_time = -1;
        transcription[j].node_data = rand() % VOCABULARY_SIZE;
        transcription[j].probability = 1.0;
    }

    return res;
}

static void create_target_MLF()
{
    int i, n;
    char *filenames[] = { "words_transcription_1.lab",
                          "./files/words_transcription_2.lab",
                          "../words_transcription_3.lab" };

    srand(time(NULL));

    target_MLF = malloc(target_MLF_size * sizeof(TMLFFilePart));
    for (i = 0; i < target_MLF_size; i++)
    {
        target_MLF[i].name = NULL;
        target_MLF[i].transcription = NULL;
        target_MLF[i].transcription_size = 0;
    }

    for (i = 0; i < target_MLF_size; i++)
    {
        n = strlen(filenames[i]);
        target_MLF[i].name = malloc((n+1)*sizeof(char));
        memset(target_MLF[i].name, 0, (n+1)*sizeof(char));
        memcpy(target_MLF[i].name, filenames[i], n * sizeof(char));

        target_MLF[i].transcription_size = create_MLF_transcription(
                    &target_MLF[i].transcription);
    }
}

static int create_MLF_file_by_target_data(char *filename)
{
    int i, j, n, res = 0;
    FILE *MLF_file_handle = NULL;
    char *word_name;
    TTranscriptionNode cur_node;

    MLF_file_handle = fopen(filename, "w");
    if (MLF_file_handle == NULL)
    {
        return 0;
    }
    if (fprintf(MLF_file_handle, "%s\n", MLF_HEADER) > 0)
    {
        res = 1;
        for (i = 0; i < target_MLF_size; i++)
        {
            if (fprintf(MLF_file_handle, "\"%s\"\n", target_MLF[i].name) <= 0)
            {
                res = 0;
                break;
            }
            n = target_MLF[i].transcription_size;
            for (j = 0; j < n; j++)
            {
                cur_node = target_MLF[i].transcription[j];
                word_name = words_vocabulary[cur_node.node_data];
                if (fprintf(MLF_file_handle, " %s  \n", word_name) <= 0)
                {
                    res = 0;
                    break;
                }
            }
            if (!res)
            {
                break;
            }
            if (fprintf(MLF_file_handle, "%c\n", '.') <= 0)
            {
                res = 0;
                break;
            }
        }
    }
    fclose(MLF_file_handle);
    return res;
}

static int create_correct_MLF_file()
{
    return create_MLF_file_by_target_data(name_of_correct_MLF_file);
}

static int create_incorrect_MLF_file_1()
{
    FILE *MLF_file_handle = NULL;

    MLF_file_handle = fopen(name_of_incorrect_MLF_file_1, "w");
    if (MLF_file_handle == NULL)
    {
        return 0;
    }
    if (fprintf(MLF_file_handle, "%s\n", "fnfjdcla") <= 0)
    {
        fclose(MLF_file_handle);
        return 0;
    }
    if (fprintf(MLF_file_handle, "%s\n", "nv,ld;s") <= 0)
    {
        fclose(MLF_file_handle);
        return 0;
    }
    if (fprintf(MLF_file_handle, "%s\n", "jdld fld;d d;lfd; ") <= 0)
    {
        fclose(MLF_file_handle);
        return 0;
    }
    fclose(MLF_file_handle);
    return 1;
}

static int create_incorrect_MLF_file_2()
{
    int i, res = 0, transcription_size = 0;
    FILE *MLF_file_handle = NULL;
    char *transcription_name = "incorrect_transcription.lab";
    char *incorrect_word = "abcdefgh";
    char *cur_word;
    PTranscriptionNode transcription = NULL;

    if (!create_MLF_file_by_target_data(name_of_incorrect_MLF_file_2))
    {
        return 0;
    }

    transcription_size = create_MLF_transcription(&transcription);
    transcription[transcription_size-1].node_data = VOCABULARY_SIZE + 4;

    MLF_file_handle = fopen(name_of_incorrect_MLF_file_2, "a");
    if (MLF_file_handle == NULL)
    {
        return 0;
    }
    if (fprintf(MLF_file_handle, "\"%s\"\n", transcription_name) > 0)
    {
        res = 1;
        for (i = 0; i < transcription_size; i++)
        {
            if (transcription[i].node_data < VOCABULARY_SIZE)
            {
                cur_word = words_vocabulary[transcription[i].node_data];
            }
            else
            {
                cur_word = incorrect_word;
            }
            if (fprintf(MLF_file_handle, "%s\n", cur_word) <= 0)
            {
                res = 0;
                break;
            }
        }
        if (res)
        {
            if (fprintf(MLF_file_handle, "%c\n", '.') <= 0)
            {
                res = 0;
            }
        }
    }
    fclose(MLF_file_handle);
    free(transcription);
    return res;
}

static int create_incorrect_MLF_file_3()
{
    int i, res = 0, transcription_size = 0;
    FILE *MLF_file_handle = NULL;
    char *transcription_name = "incorrect_transcription.lab";
    char *cur_word;
    PTranscriptionNode transcription = NULL;

    if (!create_MLF_file_by_target_data(name_of_incorrect_MLF_file_3))
    {
        return 0;
    }

    MLF_file_handle = fopen(name_of_incorrect_MLF_file_3, "a");
    if (MLF_file_handle == NULL)
    {
        return 0;
    }
    if (fprintf(MLF_file_handle, "\"%s\"\n", transcription_name) > 0)
    {
        res = 1;
        for (i = 0; i < transcription_size; i++)
        {
            cur_word = words_vocabulary[transcription[i].node_data];
            if (fprintf(MLF_file_handle, "%s\n", cur_word) <= 0)
            {
                res = 0;
                break;
            }
        }
    }
    fclose(MLF_file_handle);
    free(transcription);
    return res;
}

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

int prepare_for_testing_of_load_words_MLF()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for load_words_MLF()",
                          init_suite_load_words_MLF,
                          clean_suite_load_words_MLF);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             load_words_MLF_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    load_words_MLF_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Valid partition 3",
                                    load_words_MLF_valid_test_3))
            || (NULL == CU_add_test(pSuite, "Valid partition 4",
                                    load_words_MLF_valid_test_4))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    load_words_MLF_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_load_words_MLF()
{
    create_words_vocabulary();
    create_target_MLF();
    if (create_correct_MLF_file() && create_incorrect_MLF_file_1()
            && create_incorrect_MLF_file_2() && create_incorrect_MLF_file_3())
    {
        return 0;
    }
    return -1;
}

int clean_suite_load_words_MLF()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        if (words_vocabulary[i] != NULL)
        {
            free(words_vocabulary[i]);
            words_vocabulary[i] = NULL;
        }
    }
    free_MLF(&target_MLF, target_MLF_size);
    remove(name_of_correct_MLF_file);
    remove(name_of_incorrect_MLF_file_1);
    remove(name_of_incorrect_MLF_file_2);
    remove(name_of_incorrect_MLF_file_3);
    return 0;
}

void load_words_MLF_valid_test_1()
{
    TMLFFilePart *data = NULL;
    int data_size = 0, MLF_are_same = 1;

    data_size = load_words_MLF(name_of_correct_MLF_file, words_vocabulary,
                               VOCABULARY_SIZE, &data);
    MLF_are_same = compare_two_MLF(target_MLF,target_MLF_size, data,data_size);
    free_MLF(&data, data_size);

    CU_ASSERT_EQUAL_FATAL(target_MLF_size, data_size);
    CU_ASSERT_TRUE_FATAL(MLF_are_same);
}

void load_words_MLF_valid_test_2()
{
    TMLFFilePart *data = NULL;
    int data_size = 0, is_null = 0;

    data_size = load_words_MLF(name_of_incorrect_MLF_file_1, words_vocabulary,
                               VOCABULARY_SIZE, &data);
    is_null = (data == NULL);
    free_MLF(&data, data_size);

    CU_ASSERT_EQUAL_FATAL(0, data_size);
    CU_ASSERT_TRUE_FATAL(is_null);
}

void load_words_MLF_valid_test_3()
{
    TMLFFilePart *data = NULL;
    int data_size = 0, is_null = 0;

    data_size = load_words_MLF(name_of_incorrect_MLF_file_2, words_vocabulary,
                               VOCABULARY_SIZE, &data);
    is_null = (data == NULL);
    free_MLF(&data, data_size);

    CU_ASSERT_EQUAL_FATAL(0, data_size);
    CU_ASSERT_TRUE_FATAL(is_null);
}

void load_words_MLF_valid_test_4()
{
    TMLFFilePart *data = NULL;
    int data_size = 0, is_null = 0;

    data_size = load_words_MLF(name_of_incorrect_MLF_file_3, words_vocabulary,
                               VOCABULARY_SIZE, &data);
    is_null = (data == NULL);
    free_MLF(&data, data_size);

    CU_ASSERT_EQUAL_FATAL(0, data_size);
    CU_ASSERT_TRUE_FATAL(is_null);
}

void load_words_MLF_invalid_test_1()
{
    TMLFFilePart *data = NULL;
    int data_size = 0, is_null = 0;

    data_size = load_words_MLF(NULL, words_vocabulary, VOCABULARY_SIZE, &data);
    is_null = (data == NULL);
    free_MLF(&data, data_size);
    CU_ASSERT_EQUAL_FATAL(0, data_size);
    CU_ASSERT_TRUE_FATAL(is_null);

    data_size = load_words_MLF(name_of_correct_MLF_file, NULL, VOCABULARY_SIZE,
                               &data);
    is_null = (data == NULL);
    free_MLF(&data, data_size);
    CU_ASSERT_EQUAL_FATAL(0, data_size);
    CU_ASSERT_TRUE_FATAL(is_null);

    data_size = load_words_MLF(name_of_correct_MLF_file, words_vocabulary, 0,
                               &data);
    is_null = (data == NULL);
    free_MLF(&data, data_size);
    CU_ASSERT_EQUAL_FATAL(0, data_size);
    CU_ASSERT_TRUE_FATAL(is_null);

    data_size = load_words_MLF(name_of_correct_MLF_file, words_vocabulary,
                               VOCABULARY_SIZE, NULL);
    CU_ASSERT_EQUAL_FATAL(0, data_size);
}
