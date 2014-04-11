#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "create_linear_words_lexicon_test.h"

static char *correct_words_vocabulary_name = "correct_words_vocabulary.txt";
static char *incorrect_words_vocabulary_name ="incorrect_words_vocabulary.txt";
static TLinearWordsLexicon *target_words_lexicon = NULL;
static int phonemes_number = 8;
static char *phonemes_vocabulary[8];
static int words_number = 6;
static char *words_vocabulary[6];
static char *words_transcriptions[6];

static TLinearWordsLexicon* create_target_words_lexicon()
{
    TLinearWordsLexicon *created_lexicon;

    created_lexicon = malloc(words_number * sizeof(TLinearWordsLexicon));

    // word: 0
    // word phonemes: 2->5->1
    created_lexicon[0].word_index = 0;
    created_lexicon[0].phonemes_number = 3 + 1;
    created_lexicon[0].phonemes_indexes = malloc((3 + 1) * sizeof(int));
    created_lexicon[0].phonemes_indexes[0] = 2;
    created_lexicon[0].phonemes_indexes[1] = 5;
    created_lexicon[0].phonemes_indexes[2] = 1;
    created_lexicon[0].phonemes_indexes[3] = 0;

    // word: 1
    // word phonemes: 2->7
    created_lexicon[1].word_index = 1;
    created_lexicon[1].phonemes_number = 2 + 1;
    created_lexicon[1].phonemes_indexes = malloc((2 + 1) * sizeof(int));
    created_lexicon[1].phonemes_indexes[0] = 2;
    created_lexicon[1].phonemes_indexes[1] = 7;
    created_lexicon[1].phonemes_indexes[2] = 0;

    // word: 4
    // word phonemes: 3->6->2
    created_lexicon[4].word_index = 4;
    created_lexicon[4].phonemes_number = 3 + 1;
    created_lexicon[4].phonemes_indexes = malloc((3 + 1) * sizeof(int));
    created_lexicon[4].phonemes_indexes[0] = 3;
    created_lexicon[4].phonemes_indexes[1] = 6;
    created_lexicon[4].phonemes_indexes[2] = 2;
    created_lexicon[4].phonemes_indexes[3] = 0;

    // word: 3
    // word phonemes: 3->1
    created_lexicon[3].word_index = 3;
    created_lexicon[3].phonemes_number = 2 + 1;
    created_lexicon[3].phonemes_indexes = malloc((2 + 1) * sizeof(int));
    created_lexicon[3].phonemes_indexes[0] = 3;
    created_lexicon[3].phonemes_indexes[1] = 1;
    created_lexicon[3].phonemes_indexes[2] = 0;


    // word: 2
    // word phonemes: 3->1->2
    created_lexicon[2].word_index = 2;
    created_lexicon[2].phonemes_number = 3 + 1;
    created_lexicon[2].phonemes_indexes = malloc((3 + 1) * sizeof(int));
    created_lexicon[2].phonemes_indexes[0] = 3;
    created_lexicon[2].phonemes_indexes[1] = 1;
    created_lexicon[2].phonemes_indexes[2] = 2;
    created_lexicon[2].phonemes_indexes[3] = 0;


    // word: 5
    // word phonemes: 1->2
    created_lexicon[5].word_index = 5;
    created_lexicon[5].phonemes_number = 2 + 1;
    created_lexicon[5].phonemes_indexes = malloc((2 + 1) * sizeof(int));
    created_lexicon[5].phonemes_indexes[0] = 1;
    created_lexicon[5].phonemes_indexes[1] = 2;
    created_lexicon[5].phonemes_indexes[2] = 0;

    return created_lexicon;
}

static void create_phonemes_vocabulary()
{
    int i;
    i = 0;
    phonemes_vocabulary[i] = malloc(3 * sizeof(char));
    phonemes_vocabulary[i][0] = '-';
    phonemes_vocabulary[i][1] = '1';
    phonemes_vocabulary[i][2] = 0;
    for (i = 1; i < phonemes_number; i++)
    {
        phonemes_vocabulary[i] = malloc(2 * sizeof(char));
        phonemes_vocabulary[i][0] = (char)((int)'a' + i);
        phonemes_vocabulary[i][1] = 0;
    }
}

static void create_words_vocabulary()
{
    int i;
    for (i = 0; i < words_number; i++)
    {
        words_vocabulary[i] = malloc(4 * sizeof(char));
        words_vocabulary[i][0] = (char)((int)'a' + i);
        words_vocabulary[i][1] = words_vocabulary[i][0];
        words_vocabulary[i][2] = words_vocabulary[i][1];
        words_vocabulary[i][3] = 0;
    }
}

static void create_words_transcriptions()
{
    // word: 0
    // word phonemes: 2->5->1
    words_transcriptions[0] = malloc(3 * 2 * sizeof(char));
    words_transcriptions[0][0] = phonemes_vocabulary[2][0];
    words_transcriptions[0][1] = ' ';
    words_transcriptions[0][2] = phonemes_vocabulary[5][0];
    words_transcriptions[0][3] = ' ';
    words_transcriptions[0][4] = phonemes_vocabulary[1][0];
    words_transcriptions[0][5] = 0;

    // word: 1
    // word phonemes: 2->7
    words_transcriptions[1] = malloc(2 * 2 * sizeof(char));
    words_transcriptions[1][0] = phonemes_vocabulary[2][0];
    words_transcriptions[1][1] = ' ';
    words_transcriptions[1][2] = phonemes_vocabulary[7][0];
    words_transcriptions[1][3] = 0;

    // word: 4
    // word phonemes: 3->6->2
    words_transcriptions[4] = malloc(3 * 2 * sizeof(char));
    words_transcriptions[4][0] = phonemes_vocabulary[3][0];
    words_transcriptions[4][1] = ' ';
    words_transcriptions[4][2] = phonemes_vocabulary[6][0];
    words_transcriptions[4][3] = ' ';
    words_transcriptions[4][4] = phonemes_vocabulary[2][0];
    words_transcriptions[4][5] = 0;

    // word: 3
    // word phonemes: 3->1
    words_transcriptions[3] = malloc(2 * 2 * sizeof(char));
    words_transcriptions[3][0] = phonemes_vocabulary[3][0];
    words_transcriptions[3][1] = ' ';
    words_transcriptions[3][2] = phonemes_vocabulary[1][0];
    words_transcriptions[3][3] = 0;

    // word: 2
    // word phonemes: 3->1->2
    words_transcriptions[2] = malloc(3 * 2 * sizeof(char));
    words_transcriptions[2][0] = phonemes_vocabulary[3][0];
    words_transcriptions[2][1] = ' ';
    words_transcriptions[2][2] = phonemes_vocabulary[1][0];
    words_transcriptions[2][3] = ' ';
    words_transcriptions[2][4] = phonemes_vocabulary[2][0];
    words_transcriptions[2][5] = 0;

    // word: 5
    // word phonemes: 1->2
    words_transcriptions[5] = malloc(2 * 2 * sizeof(char));
    words_transcriptions[5][0] = phonemes_vocabulary[1][0];
    words_transcriptions[5][1] = ' ';
    words_transcriptions[5][2] = phonemes_vocabulary[2][0];
    words_transcriptions[5][3] = 0;
}

static int save_correct_words_vocabulary_for_testing()
{
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(correct_words_vocabulary_name, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }

    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[0],
                words_transcriptions[0]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[1],
                words_transcriptions[1]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[4],
                words_transcriptions[4]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[3],
                words_transcriptions[3]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[2],
                words_transcriptions[2]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[5],
                words_transcriptions[5]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }

    fclose(vocabulary_file);
    return 1;
}

static int save_incorrect_words_vocabulary_for_testing()
{
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(incorrect_words_vocabulary_name, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }

    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[0],
                words_transcriptions[0]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[1], "c o r") <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[4],
                words_transcriptions[4]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[3],
                words_transcriptions[3]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[2],
                words_transcriptions[2]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[5],
                words_transcriptions[5]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }

    fclose(vocabulary_file);
    return 1;
}

static int same_words_lexicons(TLinearWordsLexicon lexicon1[], int n1,
                               TLinearWordsLexicon lexicon2[], int n2)
{
    int i, j, n, res = 1;

    if ((n1 <= 0) || (n2 <= 0) || (lexicon1 == NULL) || (lexicon2 == NULL))
    {
        return 0;
    }

    n = n1;
    if (n != n2)
    {
        return 0;
    }
    for (i = 0; i < n; i++)
    {
        if (lexicon1[i].word_index != lexicon2[i].word_index)
        {
            res = 0;
            break;
        }
        if (lexicon1[i].phonemes_number != lexicon2[i].phonemes_number)
        {
            res = 0;
            break;
        }
        if ((lexicon1[i].phonemes_indexes == NULL)
                || (lexicon2[i].phonemes_indexes == NULL))
        {
            res = 0;
            break;
        }
        for (j = 0; j < lexicon1[i].phonemes_number; j++)
        {
            if (lexicon1[i].phonemes_indexes[j]
                    != lexicon2[i].phonemes_indexes[j])
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

int prepare_for_testing_of_create_linear_words_lexicon()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for create_linear_words_lexicon()",
                          init_suite_create_linear_words_lexicon,
                          clean_suite_create_linear_words_lexicon);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             create_linear_words_lexicon_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    create_linear_words_lexicon_valid_test_2))
            || (NULL == CU_add_test(
                    pSuite, "Invalid partitions",
                    create_linear_words_lexicon_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_create_linear_words_lexicon()
{
    target_words_lexicon = create_target_words_lexicon();
    create_phonemes_vocabulary();
    create_words_vocabulary();
    create_words_transcriptions();
    if (!save_correct_words_vocabulary_for_testing())
    {
        return -1;
    }
    if (!save_incorrect_words_vocabulary_for_testing())
    {
        return -1;
    }
    return 0;
}

int clean_suite_create_linear_words_lexicon()
{
    int i;
    free_linear_words_lexicon(&target_words_lexicon, words_number);
    for (i = 0; i < phonemes_number; i++)
    {
        if (phonemes_vocabulary[i] != NULL)
        {
            free(phonemes_vocabulary[i]);
            phonemes_vocabulary[i] = NULL;
        }
    }
    for (i = 0; i < words_number; i++)
    {
        if (words_vocabulary[i] != NULL)
        {
            free(words_vocabulary[i]);
            words_vocabulary[i] = NULL;
        }
        if (words_transcriptions[i] != NULL)
        {
            free(words_transcriptions[i]);
            words_transcriptions[i] = NULL;
        }
    }
    remove(correct_words_vocabulary_name);
    remove(incorrect_words_vocabulary_name);
    return 0;
}

void create_linear_words_lexicon_valid_test_1()
{
    TLinearWordsLexicon *created_lexicon = NULL;
    int created_lexicon_size, lexicon_is_correct = 0;

    created_lexicon_size = create_linear_words_lexicon(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, words_number,
                &created_lexicon);
    if (created_lexicon_size > 0)
    {
        if (same_words_lexicons(target_words_lexicon, words_number,
                                created_lexicon, created_lexicon_size))
        {
            lexicon_is_correct = 1;
        }
    }
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);

    CU_ASSERT_TRUE_FATAL(created_lexicon_size > 0);
    CU_ASSERT_TRUE_FATAL(lexicon_is_correct);
}

void create_linear_words_lexicon_valid_test_2()
{
    TLinearWordsLexicon *created_lexicon = NULL;
    int created_lexicon_size;

    created_lexicon_size = create_linear_words_lexicon(
                incorrect_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, words_number,
                &created_lexicon);
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);
}

void create_linear_words_lexicon_invalid_test_1()
{
    TLinearWordsLexicon *created_lexicon = NULL;
    int created_lexicon_size;

    created_lexicon_size = create_linear_words_lexicon(
                NULL, phonemes_vocabulary,
                phonemes_number, words_vocabulary, words_number,
                &created_lexicon);
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);

    created_lexicon_size = create_linear_words_lexicon(
                correct_words_vocabulary_name, NULL,
                phonemes_number, words_vocabulary, words_number,
                &created_lexicon);
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);

    created_lexicon_size = create_linear_words_lexicon(
                correct_words_vocabulary_name, phonemes_vocabulary,
                0, words_vocabulary, words_number,
                &created_lexicon);
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);

    created_lexicon_size = create_linear_words_lexicon(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, NULL, words_number,
                &created_lexicon);
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);

    created_lexicon_size = create_linear_words_lexicon(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, 0,
                &created_lexicon);
    free_linear_words_lexicon(&created_lexicon, created_lexicon_size);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);

    created_lexicon_size = create_linear_words_lexicon(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, words_number,
                NULL);
    CU_ASSERT_TRUE_FATAL(created_lexicon_size == 0);
}
