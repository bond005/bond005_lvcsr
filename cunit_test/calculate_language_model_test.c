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
static float lambda1 = 1.0;
static float eps1 = 0.0;
static float lambda2 = 0.7;
static float eps2 = 0.0;
static float lambda3 = 0.7;
static float eps3 = 0.2;
static TLanguageModel target_language_model1;
static TLanguageModel target_language_model2;
static TLanguageModel target_language_model3;

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

    target_language_model1.bigrams_number = 23;
    target_language_model1.bigrams
            = malloc(target_language_model1.bigrams_number*sizeof(TWordBigram));

    target_language_model1.bigrams[0].first_word = 5;
    target_language_model1.bigrams[0].second_word = 0;
    target_language_model1.bigrams[0].probability = 2.0 / 5.0;

    target_language_model1.bigrams[1].first_word = 9;
    target_language_model1.bigrams[1].second_word = 0;
    target_language_model1.bigrams[1].probability = 1.0 / 5.0;

    target_language_model1.bigrams[2].first_word = 0;
    target_language_model1.bigrams[2].second_word = 1;
    target_language_model1.bigrams[2].probability = 2.0 / 3.0;

    target_language_model1.bigrams[3].first_word = 8;
    target_language_model1.bigrams[3].second_word = 1;
    target_language_model1.bigrams[3].probability = 1.0 / 2.0;

    target_language_model1.bigrams[4].first_word = 9;
    target_language_model1.bigrams[4].second_word = 1;
    target_language_model1.bigrams[4].probability = 1.0 / 5.0;

    target_language_model1.bigrams[5].first_word = 1;
    target_language_model1.bigrams[5].second_word = 2;
    target_language_model1.bigrams[5].probability = 2.0 / 4.0;

    target_language_model1.bigrams[6].first_word = 4;
    target_language_model1.bigrams[6].second_word = 2;
    target_language_model1.bigrams[6].probability = 2.0 / 3.0;

    target_language_model1.bigrams[7].first_word = 9;
    target_language_model1.bigrams[7].second_word = 2;
    target_language_model1.bigrams[7].probability = 1.0 / 5.0;

    target_language_model1.bigrams[8].first_word = 2;
    target_language_model1.bigrams[8].second_word = 3;
    target_language_model1.bigrams[8].probability = 1.0 / 5.0;

    target_language_model1.bigrams[9].first_word = 6;
    target_language_model1.bigrams[9].second_word = 3;
    target_language_model1.bigrams[9].probability = 1.0 / 5.0;

    target_language_model1.bigrams[10].first_word = 6;
    target_language_model1.bigrams[10].second_word = 4;
    target_language_model1.bigrams[10].probability = 3.0 / 5.0;

    target_language_model1.bigrams[11].first_word = 2;
    target_language_model1.bigrams[11].second_word = 5;
    target_language_model1.bigrams[11].probability = 2.0 / 5.0;

    target_language_model1.bigrams[12].first_word = 7;
    target_language_model1.bigrams[12].second_word = 5;
    target_language_model1.bigrams[12].probability = 1.0 / 2.0;

    target_language_model1.bigrams[13].first_word = 1;
    target_language_model1.bigrams[13].second_word = 6;
    target_language_model1.bigrams[13].probability = 2.0 / 4.0;

    target_language_model1.bigrams[14].first_word = 2;
    target_language_model1.bigrams[14].second_word = 6;
    target_language_model1.bigrams[14].probability = 1.0 / 5.0;

    target_language_model1.bigrams[15].first_word = 7;
    target_language_model1.bigrams[15].second_word = 6;
    target_language_model1.bigrams[15].probability = 1.0 / 2.0;

    target_language_model1.bigrams[16].first_word = 9;
    target_language_model1.bigrams[16].second_word = 6;
    target_language_model1.bigrams[16].probability = 1.0 / 5.0;

    target_language_model1.bigrams[17].first_word = 5;
    target_language_model1.bigrams[17].second_word = 7;
    target_language_model1.bigrams[17].probability = 2.0 / 5.0;

    target_language_model1.bigrams[18].first_word = 5;
    target_language_model1.bigrams[18].second_word = 8;
    target_language_model1.bigrams[18].probability = 1.0 / 5.0;

    target_language_model1.bigrams[19].first_word = 9;
    target_language_model1.bigrams[19].second_word = 8;
    target_language_model1.bigrams[19].probability = 1.0 / 5.0;

    target_language_model1.bigrams[20].first_word = 0;
    target_language_model1.bigrams[20].second_word = 9;
    target_language_model1.bigrams[20].probability = 1.0 / 3.0;

    target_language_model1.bigrams[21].first_word = 3;
    target_language_model1.bigrams[21].second_word = 9;
    target_language_model1.bigrams[21].probability = 3.0 / 3.0;

    target_language_model1.bigrams[22].first_word = 8;
    target_language_model1.bigrams[22].second_word = 9;
    target_language_model1.bigrams[22].probability = 1.0 / 2.0;
}

static void create_target_language_model2()
{
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

    target_language_model2.bigrams_number = 100;
    target_language_model2.bigrams
            = malloc(target_language_model2.bigrams_number*sizeof(TWordBigram));

    target_language_model2.bigrams[0].first_word = 0;
    target_language_model2.bigrams[0].second_word = 0;
    target_language_model2.bigrams[0].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[1].first_word = 1;
    target_language_model2.bigrams[1].second_word = 0;
    target_language_model2.bigrams[1].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[2].first_word = 2;
    target_language_model2.bigrams[2].second_word = 0;
    target_language_model2.bigrams[2].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[3].first_word = 3;
    target_language_model2.bigrams[3].second_word = 0;
    target_language_model2.bigrams[3].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[4].first_word = 4;
    target_language_model2.bigrams[4].second_word = 0;
    target_language_model2.bigrams[4].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[5].first_word = 5;
    target_language_model2.bigrams[5].second_word = 0;
    target_language_model2.bigrams[5].probability = lambda2 * (2.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[6].first_word = 6;
    target_language_model2.bigrams[6].second_word = 0;
    target_language_model2.bigrams[6].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[7].first_word = 7;
    target_language_model2.bigrams[7].second_word = 0;
    target_language_model2.bigrams[7].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[8].first_word = 8;
    target_language_model2.bigrams[8].second_word = 0;
    target_language_model2.bigrams[8].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[9].first_word = 9;
    target_language_model2.bigrams[9].second_word = 0;
    target_language_model2.bigrams[9].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[0];

    target_language_model2.bigrams[10].first_word = 0;
    target_language_model2.bigrams[10].second_word = 1;
    target_language_model2.bigrams[10].probability = lambda2 * (2.0 / 3.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[11].first_word = 1;
    target_language_model2.bigrams[11].second_word = 1;
    target_language_model2.bigrams[11].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[12].first_word = 2;
    target_language_model2.bigrams[12].second_word = 1;
    target_language_model2.bigrams[12].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[13].first_word = 3;
    target_language_model2.bigrams[13].second_word = 1;
    target_language_model2.bigrams[13].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[14].first_word = 4;
    target_language_model2.bigrams[14].second_word = 1;
    target_language_model2.bigrams[14].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[15].first_word = 5;
    target_language_model2.bigrams[15].second_word = 1;
    target_language_model2.bigrams[15].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[16].first_word = 6;
    target_language_model2.bigrams[16].second_word = 1;
    target_language_model2.bigrams[16].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[17].first_word = 7;
    target_language_model2.bigrams[17].second_word = 1;
    target_language_model2.bigrams[17].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[18].first_word = 8;
    target_language_model2.bigrams[18].second_word = 1;
    target_language_model2.bigrams[18].probability = lambda2 * (1.0 / 2.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[19].first_word = 9;
    target_language_model2.bigrams[19].second_word = 1;
    target_language_model2.bigrams[19].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[1];

    target_language_model2.bigrams[20].first_word = 0;
    target_language_model2.bigrams[20].second_word = 2;
    target_language_model2.bigrams[20].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[21].first_word = 1;
    target_language_model2.bigrams[21].second_word = 2;
    target_language_model2.bigrams[21].probability = lambda2 * (2.0 / 4.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[22].first_word = 2;
    target_language_model2.bigrams[22].second_word = 2;
    target_language_model2.bigrams[22].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[23].first_word = 3;
    target_language_model2.bigrams[23].second_word = 2;
    target_language_model2.bigrams[23].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[24].first_word = 4;
    target_language_model2.bigrams[24].second_word = 2;
    target_language_model2.bigrams[24].probability = lambda2 * (2.0 / 3.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[25].first_word = 5;
    target_language_model2.bigrams[25].second_word = 2;
    target_language_model2.bigrams[25].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[26].first_word = 6;
    target_language_model2.bigrams[26].second_word = 2;
    target_language_model2.bigrams[26].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[27].first_word = 7;
    target_language_model2.bigrams[27].second_word = 2;
    target_language_model2.bigrams[27].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[28].first_word = 8;
    target_language_model2.bigrams[28].second_word = 2;
    target_language_model2.bigrams[28].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[29].first_word = 9;
    target_language_model2.bigrams[29].second_word = 2;
    target_language_model2.bigrams[29].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[2];

    target_language_model2.bigrams[30].first_word = 0;
    target_language_model2.bigrams[30].second_word = 3;
    target_language_model2.bigrams[30].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[31].first_word = 1;
    target_language_model2.bigrams[31].second_word = 3;
    target_language_model2.bigrams[31].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[32].first_word = 2;
    target_language_model2.bigrams[32].second_word = 3;
    target_language_model2.bigrams[32].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[33].first_word = 3;
    target_language_model2.bigrams[33].second_word = 3;
    target_language_model2.bigrams[33].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[34].first_word = 4;
    target_language_model2.bigrams[34].second_word = 3;
    target_language_model2.bigrams[34].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[35].first_word = 5;
    target_language_model2.bigrams[35].second_word = 3;
    target_language_model2.bigrams[35].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[36].first_word = 6;
    target_language_model2.bigrams[36].second_word = 3;
    target_language_model2.bigrams[36].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[37].first_word = 7;
    target_language_model2.bigrams[37].second_word = 3;
    target_language_model2.bigrams[37].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[38].first_word = 8;
    target_language_model2.bigrams[38].second_word = 3;
    target_language_model2.bigrams[38].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[39].first_word = 9;
    target_language_model2.bigrams[39].second_word = 3;
    target_language_model2.bigrams[39].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[3];

    target_language_model2.bigrams[40].first_word = 0;
    target_language_model2.bigrams[40].second_word = 4;
    target_language_model2.bigrams[40].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[41].first_word = 1;
    target_language_model2.bigrams[41].second_word = 4;
    target_language_model2.bigrams[41].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[42].first_word = 2;
    target_language_model2.bigrams[42].second_word = 4;
    target_language_model2.bigrams[42].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[43].first_word = 3;
    target_language_model2.bigrams[43].second_word = 4;
    target_language_model2.bigrams[43].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[44].first_word = 4;
    target_language_model2.bigrams[44].second_word = 4;
    target_language_model2.bigrams[44].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[45].first_word = 5;
    target_language_model2.bigrams[45].second_word = 4;
    target_language_model2.bigrams[45].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[46].first_word = 6;
    target_language_model2.bigrams[46].second_word = 4;
    target_language_model2.bigrams[46].probability = lambda2 * (3.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[47].first_word = 7;
    target_language_model2.bigrams[47].second_word = 4;
    target_language_model2.bigrams[47].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[48].first_word = 8;
    target_language_model2.bigrams[48].second_word = 4;
    target_language_model2.bigrams[48].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[49].first_word = 9;
    target_language_model2.bigrams[49].second_word = 4;
    target_language_model2.bigrams[49].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[4];

    target_language_model2.bigrams[50].first_word = 0;
    target_language_model2.bigrams[50].second_word = 5;
    target_language_model2.bigrams[50].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[51].first_word = 1;
    target_language_model2.bigrams[51].second_word = 5;
    target_language_model2.bigrams[51].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[52].first_word = 2;
    target_language_model2.bigrams[52].second_word = 5;
    target_language_model2.bigrams[52].probability = lambda2 * (2.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[53].first_word = 3;
    target_language_model2.bigrams[53].second_word = 5;
    target_language_model2.bigrams[53].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[54].first_word = 4;
    target_language_model2.bigrams[54].second_word = 5;
    target_language_model2.bigrams[54].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[55].first_word = 5;
    target_language_model2.bigrams[55].second_word = 5;
    target_language_model2.bigrams[55].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[56].first_word = 6;
    target_language_model2.bigrams[56].second_word = 5;
    target_language_model2.bigrams[56].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[57].first_word = 7;
    target_language_model2.bigrams[57].second_word = 5;
    target_language_model2.bigrams[57].probability = lambda2 * (1.0 / 2.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[58].first_word = 8;
    target_language_model2.bigrams[58].second_word = 5;
    target_language_model2.bigrams[58].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[59].first_word = 9;
    target_language_model2.bigrams[59].second_word = 5;
    target_language_model2.bigrams[59].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[5];

    target_language_model2.bigrams[60].first_word = 0;
    target_language_model2.bigrams[60].second_word = 6;
    target_language_model2.bigrams[60].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[61].first_word = 1;
    target_language_model2.bigrams[61].second_word = 6;
    target_language_model2.bigrams[61].probability = lambda2 * (2.0 / 4.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[62].first_word = 2;
    target_language_model2.bigrams[62].second_word = 6;
    target_language_model2.bigrams[62].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[63].first_word = 3;
    target_language_model2.bigrams[63].second_word = 6;
    target_language_model2.bigrams[63].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[64].first_word = 4;
    target_language_model2.bigrams[64].second_word = 6;
    target_language_model2.bigrams[64].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[65].first_word = 5;
    target_language_model2.bigrams[65].second_word = 6;
    target_language_model2.bigrams[65].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[66].first_word = 6;
    target_language_model2.bigrams[66].second_word = 6;
    target_language_model2.bigrams[66].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[67].first_word = 7;
    target_language_model2.bigrams[67].second_word = 6;
    target_language_model2.bigrams[67].probability = lambda2 * (1.0 / 2.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[68].first_word = 8;
    target_language_model2.bigrams[68].second_word = 6;
    target_language_model2.bigrams[68].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[69].first_word = 9;
    target_language_model2.bigrams[69].second_word = 6;
    target_language_model2.bigrams[69].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[6];

    target_language_model2.bigrams[70].first_word = 0;
    target_language_model2.bigrams[70].second_word = 7;
    target_language_model2.bigrams[70].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[71].first_word = 1;
    target_language_model2.bigrams[71].second_word = 7;
    target_language_model2.bigrams[71].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[72].first_word = 2;
    target_language_model2.bigrams[72].second_word = 7;
    target_language_model2.bigrams[72].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[73].first_word = 3;
    target_language_model2.bigrams[73].second_word = 7;
    target_language_model2.bigrams[73].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[74].first_word = 4;
    target_language_model2.bigrams[74].second_word = 7;
    target_language_model2.bigrams[74].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[75].first_word = 5;
    target_language_model2.bigrams[75].second_word = 7;
    target_language_model2.bigrams[75].probability = lambda2 * (2.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[76].first_word = 6;
    target_language_model2.bigrams[76].second_word = 7;
    target_language_model2.bigrams[76].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[77].first_word = 7;
    target_language_model2.bigrams[77].second_word = 7;
    target_language_model2.bigrams[77].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[78].first_word = 8;
    target_language_model2.bigrams[78].second_word = 7;
    target_language_model2.bigrams[78].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[79].first_word = 9;
    target_language_model2.bigrams[79].second_word = 7;
    target_language_model2.bigrams[79].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[7];

    target_language_model2.bigrams[80].first_word = 0;
    target_language_model2.bigrams[80].second_word = 8;
    target_language_model2.bigrams[80].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[81].first_word = 1;
    target_language_model2.bigrams[81].second_word = 8;
    target_language_model2.bigrams[81].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[82].first_word = 2;
    target_language_model2.bigrams[82].second_word = 8;
    target_language_model2.bigrams[82].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[83].first_word = 3;
    target_language_model2.bigrams[83].second_word = 8;
    target_language_model2.bigrams[83].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[84].first_word = 4;
    target_language_model2.bigrams[84].second_word = 8;
    target_language_model2.bigrams[84].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[85].first_word = 5;
    target_language_model2.bigrams[85].second_word = 8;
    target_language_model2.bigrams[85].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[86].first_word = 6;
    target_language_model2.bigrams[86].second_word = 8;
    target_language_model2.bigrams[86].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[87].first_word = 7;
    target_language_model2.bigrams[87].second_word = 8;
    target_language_model2.bigrams[87].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[88].first_word = 8;
    target_language_model2.bigrams[88].second_word = 8;
    target_language_model2.bigrams[88].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[89].first_word = 9;
    target_language_model2.bigrams[89].second_word = 8;
    target_language_model2.bigrams[89].probability = lambda2 * (1.0 / 5.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[8];

    target_language_model2.bigrams[90].first_word = 0;
    target_language_model2.bigrams[90].second_word = 9;
    target_language_model2.bigrams[90].probability = lambda2 * (1.0 / 3.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[91].first_word = 1;
    target_language_model2.bigrams[91].second_word = 9;
    target_language_model2.bigrams[91].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[92].first_word = 2;
    target_language_model2.bigrams[92].second_word = 9;
    target_language_model2.bigrams[92].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[93].first_word = 3;
    target_language_model2.bigrams[93].second_word = 9;
    target_language_model2.bigrams[93].probability = lambda2 * (3.0 / 3.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[94].first_word = 4;
    target_language_model2.bigrams[94].second_word = 9;
    target_language_model2.bigrams[94].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[95].first_word = 5;
    target_language_model2.bigrams[95].second_word = 9;
    target_language_model2.bigrams[95].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[96].first_word = 6;
    target_language_model2.bigrams[96].second_word = 9;
    target_language_model2.bigrams[96].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[97].first_word = 7;
    target_language_model2.bigrams[97].second_word = 9;
    target_language_model2.bigrams[97].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[98].first_word = 8;
    target_language_model2.bigrams[98].second_word = 9;
    target_language_model2.bigrams[98].probability = lambda2 * (1.0 / 2.0)
            + (1 - lambda2) * target_language_model2.unigrams_probabilities[9];

    target_language_model2.bigrams[99].first_word = 9;
    target_language_model2.bigrams[99].second_word = 9;
    target_language_model2.bigrams[99].probability
            = (1 - lambda2) * target_language_model2.unigrams_probabilities[9];
}

static void create_target_language_model3()
{
    int i, n;

    target_language_model3.unigrams_number = WORDS_NUMBER;
    target_language_model3.unigrams_probabilities
            = malloc(WORDS_NUMBER * sizeof(float));
    target_language_model3.unigrams_probabilities[0] = 3.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[1] = 4.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[2] = 5.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[3] = 3.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[4] = 3.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[5] = 5.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[6] = 5.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[7] = 2.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[8] = 2.0/(12.0+10.0+15.0);
    target_language_model3.unigrams_probabilities[9] = 5.0/(12.0+10.0+15.0);

    target_language_model3.bigrams_number = 100;
    target_language_model3.bigrams
            = malloc(target_language_model3.bigrams_number*sizeof(TWordBigram));

    target_language_model3.bigrams[0].first_word = 0;
    target_language_model3.bigrams[0].second_word = 0;
    target_language_model3.bigrams[0].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[1].first_word = 1;
    target_language_model3.bigrams[1].second_word = 0;
    target_language_model3.bigrams[1].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[2].first_word = 2;
    target_language_model3.bigrams[2].second_word = 0;
    target_language_model3.bigrams[2].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[3].first_word = 3;
    target_language_model3.bigrams[3].second_word = 0;
    target_language_model3.bigrams[3].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[4].first_word = 4;
    target_language_model3.bigrams[4].second_word = 0;
    target_language_model3.bigrams[4].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[5].first_word = 5;
    target_language_model3.bigrams[5].second_word = 0;
    target_language_model3.bigrams[5].probability = lambda3 * (2.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[6].first_word = 6;
    target_language_model3.bigrams[6].second_word = 0;
    target_language_model3.bigrams[6].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[7].first_word = 7;
    target_language_model3.bigrams[7].second_word = 0;
    target_language_model3.bigrams[7].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[8].first_word = 8;
    target_language_model3.bigrams[8].second_word = 0;
    target_language_model3.bigrams[8].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[9].first_word = 9;
    target_language_model3.bigrams[9].second_word = 0;
    target_language_model3.bigrams[9].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[0];

    target_language_model3.bigrams[10].first_word = 0;
    target_language_model3.bigrams[10].second_word = 1;
    target_language_model3.bigrams[10].probability = lambda3 * (2.0 / 3.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[11].first_word = 1;
    target_language_model3.bigrams[11].second_word = 1;
    target_language_model3.bigrams[11].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[12].first_word = 2;
    target_language_model3.bigrams[12].second_word = 1;
    target_language_model3.bigrams[12].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[13].first_word = 3;
    target_language_model3.bigrams[13].second_word = 1;
    target_language_model3.bigrams[13].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[14].first_word = 4;
    target_language_model3.bigrams[14].second_word = 1;
    target_language_model3.bigrams[14].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[15].first_word = 5;
    target_language_model3.bigrams[15].second_word = 1;
    target_language_model3.bigrams[15].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[16].first_word = 6;
    target_language_model3.bigrams[16].second_word = 1;
    target_language_model3.bigrams[16].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[17].first_word = 7;
    target_language_model3.bigrams[17].second_word = 1;
    target_language_model3.bigrams[17].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[18].first_word = 8;
    target_language_model3.bigrams[18].second_word = 1;
    target_language_model3.bigrams[18].probability = lambda3 * (1.0 / 2.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[19].first_word = 9;
    target_language_model3.bigrams[19].second_word = 1;
    target_language_model3.bigrams[19].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[1];

    target_language_model3.bigrams[20].first_word = 0;
    target_language_model3.bigrams[20].second_word = 2;
    target_language_model3.bigrams[20].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[21].first_word = 1;
    target_language_model3.bigrams[21].second_word = 2;
    target_language_model3.bigrams[21].probability = lambda3 * (2.0 / 4.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[22].first_word = 2;
    target_language_model3.bigrams[22].second_word = 2;
    target_language_model3.bigrams[22].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[23].first_word = 3;
    target_language_model3.bigrams[23].second_word = 2;
    target_language_model3.bigrams[23].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[24].first_word = 4;
    target_language_model3.bigrams[24].second_word = 2;
    target_language_model3.bigrams[24].probability = lambda3 * (2.0 / 3.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[25].first_word = 5;
    target_language_model3.bigrams[25].second_word = 2;
    target_language_model3.bigrams[25].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[26].first_word = 6;
    target_language_model3.bigrams[26].second_word = 2;
    target_language_model3.bigrams[26].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[27].first_word = 7;
    target_language_model3.bigrams[27].second_word = 2;
    target_language_model3.bigrams[27].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[28].first_word = 8;
    target_language_model3.bigrams[28].second_word = 2;
    target_language_model3.bigrams[28].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[29].first_word = 9;
    target_language_model3.bigrams[29].second_word = 2;
    target_language_model3.bigrams[29].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[2];

    target_language_model3.bigrams[30].first_word = 0;
    target_language_model3.bigrams[30].second_word = 3;
    target_language_model3.bigrams[30].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[31].first_word = 1;
    target_language_model3.bigrams[31].second_word = 3;
    target_language_model3.bigrams[31].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[32].first_word = 2;
    target_language_model3.bigrams[32].second_word = 3;
    target_language_model3.bigrams[32].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[33].first_word = 3;
    target_language_model3.bigrams[33].second_word = 3;
    target_language_model3.bigrams[33].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[34].first_word = 4;
    target_language_model3.bigrams[34].second_word = 3;
    target_language_model3.bigrams[34].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[35].first_word = 5;
    target_language_model3.bigrams[35].second_word = 3;
    target_language_model3.bigrams[35].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[36].first_word = 6;
    target_language_model3.bigrams[36].second_word = 3;
    target_language_model3.bigrams[36].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[37].first_word = 7;
    target_language_model3.bigrams[37].second_word = 3;
    target_language_model3.bigrams[37].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[38].first_word = 8;
    target_language_model3.bigrams[38].second_word = 3;
    target_language_model3.bigrams[38].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[39].first_word = 9;
    target_language_model3.bigrams[39].second_word = 3;
    target_language_model3.bigrams[39].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[3];

    target_language_model3.bigrams[40].first_word = 0;
    target_language_model3.bigrams[40].second_word = 4;
    target_language_model3.bigrams[40].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[41].first_word = 1;
    target_language_model3.bigrams[41].second_word = 4;
    target_language_model3.bigrams[41].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[42].first_word = 2;
    target_language_model3.bigrams[42].second_word = 4;
    target_language_model3.bigrams[42].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[43].first_word = 3;
    target_language_model3.bigrams[43].second_word = 4;
    target_language_model3.bigrams[43].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[44].first_word = 4;
    target_language_model3.bigrams[44].second_word = 4;
    target_language_model3.bigrams[44].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[45].first_word = 5;
    target_language_model3.bigrams[45].second_word = 4;
    target_language_model3.bigrams[45].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[46].first_word = 6;
    target_language_model3.bigrams[46].second_word = 4;
    target_language_model3.bigrams[46].probability = lambda3 * (3.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[47].first_word = 7;
    target_language_model3.bigrams[47].second_word = 4;
    target_language_model3.bigrams[47].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[48].first_word = 8;
    target_language_model3.bigrams[48].second_word = 4;
    target_language_model3.bigrams[48].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[49].first_word = 9;
    target_language_model3.bigrams[49].second_word = 4;
    target_language_model3.bigrams[49].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[4];

    target_language_model3.bigrams[50].first_word = 0;
    target_language_model3.bigrams[50].second_word = 5;
    target_language_model3.bigrams[50].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[51].first_word = 1;
    target_language_model3.bigrams[51].second_word = 5;
    target_language_model3.bigrams[51].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[52].first_word = 2;
    target_language_model3.bigrams[52].second_word = 5;
    target_language_model3.bigrams[52].probability = lambda3 * (2.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[53].first_word = 3;
    target_language_model3.bigrams[53].second_word = 5;
    target_language_model3.bigrams[53].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[54].first_word = 4;
    target_language_model3.bigrams[54].second_word = 5;
    target_language_model3.bigrams[54].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[55].first_word = 5;
    target_language_model3.bigrams[55].second_word = 5;
    target_language_model3.bigrams[55].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[56].first_word = 6;
    target_language_model3.bigrams[56].second_word = 5;
    target_language_model3.bigrams[56].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[57].first_word = 7;
    target_language_model3.bigrams[57].second_word = 5;
    target_language_model3.bigrams[57].probability = lambda3 * (1.0 / 2.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[58].first_word = 8;
    target_language_model3.bigrams[58].second_word = 5;
    target_language_model3.bigrams[58].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[59].first_word = 9;
    target_language_model3.bigrams[59].second_word = 5;
    target_language_model3.bigrams[59].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[5];

    target_language_model3.bigrams[60].first_word = 0;
    target_language_model3.bigrams[60].second_word = 6;
    target_language_model3.bigrams[60].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[61].first_word = 1;
    target_language_model3.bigrams[61].second_word = 6;
    target_language_model3.bigrams[61].probability = lambda3 * (2.0 / 4.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[62].first_word = 2;
    target_language_model3.bigrams[62].second_word = 6;
    target_language_model3.bigrams[62].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[63].first_word = 3;
    target_language_model3.bigrams[63].second_word = 6;
    target_language_model3.bigrams[63].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[64].first_word = 4;
    target_language_model3.bigrams[64].second_word = 6;
    target_language_model3.bigrams[64].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[65].first_word = 5;
    target_language_model3.bigrams[65].second_word = 6;
    target_language_model3.bigrams[65].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[66].first_word = 6;
    target_language_model3.bigrams[66].second_word = 6;
    target_language_model3.bigrams[66].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[67].first_word = 7;
    target_language_model3.bigrams[67].second_word = 6;
    target_language_model3.bigrams[67].probability = lambda3 * (1.0 / 2.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[68].first_word = 8;
    target_language_model3.bigrams[68].second_word = 6;
    target_language_model3.bigrams[68].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[69].first_word = 9;
    target_language_model3.bigrams[69].second_word = 6;
    target_language_model3.bigrams[69].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[6];

    target_language_model3.bigrams[70].first_word = 0;
    target_language_model3.bigrams[70].second_word = 7;
    target_language_model3.bigrams[70].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[71].first_word = 1;
    target_language_model3.bigrams[71].second_word = 7;
    target_language_model3.bigrams[71].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[72].first_word = 2;
    target_language_model3.bigrams[72].second_word = 7;
    target_language_model3.bigrams[72].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[73].first_word = 3;
    target_language_model3.bigrams[73].second_word = 7;
    target_language_model3.bigrams[73].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[74].first_word = 4;
    target_language_model3.bigrams[74].second_word = 7;
    target_language_model3.bigrams[74].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[75].first_word = 5;
    target_language_model3.bigrams[75].second_word = 7;
    target_language_model3.bigrams[75].probability = lambda3 * (2.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[76].first_word = 6;
    target_language_model3.bigrams[76].second_word = 7;
    target_language_model3.bigrams[76].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[77].first_word = 7;
    target_language_model3.bigrams[77].second_word = 7;
    target_language_model3.bigrams[77].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[78].first_word = 8;
    target_language_model3.bigrams[78].second_word = 7;
    target_language_model3.bigrams[78].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[79].first_word = 9;
    target_language_model3.bigrams[79].second_word = 7;
    target_language_model3.bigrams[79].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[7];

    target_language_model3.bigrams[80].first_word = 0;
    target_language_model3.bigrams[80].second_word = 8;
    target_language_model3.bigrams[80].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[81].first_word = 1;
    target_language_model3.bigrams[81].second_word = 8;
    target_language_model3.bigrams[81].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[82].first_word = 2;
    target_language_model3.bigrams[82].second_word = 8;
    target_language_model3.bigrams[82].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[83].first_word = 3;
    target_language_model3.bigrams[83].second_word = 8;
    target_language_model3.bigrams[83].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[84].first_word = 4;
    target_language_model3.bigrams[84].second_word = 8;
    target_language_model3.bigrams[84].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[85].first_word = 5;
    target_language_model3.bigrams[85].second_word = 8;
    target_language_model3.bigrams[85].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[86].first_word = 6;
    target_language_model3.bigrams[86].second_word = 8;
    target_language_model3.bigrams[86].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[87].first_word = 7;
    target_language_model3.bigrams[87].second_word = 8;
    target_language_model3.bigrams[87].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[88].first_word = 8;
    target_language_model3.bigrams[88].second_word = 8;
    target_language_model3.bigrams[88].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[89].first_word = 9;
    target_language_model3.bigrams[89].second_word = 8;
    target_language_model3.bigrams[89].probability = lambda3 * (1.0 / 5.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[8];

    target_language_model3.bigrams[90].first_word = 0;
    target_language_model3.bigrams[90].second_word = 9;
    target_language_model3.bigrams[90].probability = lambda3 * (1.0 / 3.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[91].first_word = 1;
    target_language_model3.bigrams[91].second_word = 9;
    target_language_model3.bigrams[91].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[92].first_word = 2;
    target_language_model3.bigrams[92].second_word = 9;
    target_language_model3.bigrams[92].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[93].first_word = 3;
    target_language_model3.bigrams[93].second_word = 9;
    target_language_model3.bigrams[93].probability = lambda3 * (3.0 / 3.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[94].first_word = 4;
    target_language_model3.bigrams[94].second_word = 9;
    target_language_model3.bigrams[94].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[95].first_word = 5;
    target_language_model3.bigrams[95].second_word = 9;
    target_language_model3.bigrams[95].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[96].first_word = 6;
    target_language_model3.bigrams[96].second_word = 9;
    target_language_model3.bigrams[96].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[97].first_word = 7;
    target_language_model3.bigrams[97].second_word = 9;
    target_language_model3.bigrams[97].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[98].first_word = 8;
    target_language_model3.bigrams[98].second_word = 9;
    target_language_model3.bigrams[98].probability = lambda3 * (1.0 / 2.0)
            + (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    target_language_model3.bigrams[99].first_word = 9;
    target_language_model3.bigrams[99].second_word = 9;
    target_language_model3.bigrams[99].probability
            = (1 - lambda3) * target_language_model3.unigrams_probabilities[9];

    i = 0;
    while (i < target_language_model3.bigrams_number)
    {
        if (target_language_model3.bigrams[i].probability > eps3)
        {
            i++;
        }
        else
        {
            n = target_language_model3.bigrams_number - i - 1;
            if (n > 0)
            {
                memmove(&(target_language_model3.bigrams[i]),
                        &(target_language_model3.bigrams[i+1]),
                        n * sizeof(TWordBigram));
            }
            target_language_model3.bigrams_number--;
        }
    }
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
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    calculate_language_model_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Valid partition 3",
                                    calculate_language_model_valid_test_3))
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
    create_target_language_model3();
    return 0;
}

int clean_suite_calculate_language_model()
{
    free_words_MLF_data();
    free_language_model(&target_language_model1);
    free_language_model(&target_language_model2);
    free_language_model(&target_language_model3);
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
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda1, eps1,
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
    calculated_model.bigrams_number = 0;
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda2, eps2,
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

void calculate_language_model_valid_test_3()
{
    TLanguageModel calculated_model;
    int calculated_res = 0, compare_res = 0;

    calculated_model.bigrams = NULL;
    calculated_model.bigrams_number = 0;
    calculated_model.unigrams_number = 0;
    calculated_model.unigrams_probabilities = NULL;

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda3, eps3,
                &calculated_model);
    if (calculated_res)
    {
        compare_res = compare_language_models(target_language_model3,
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
                NULL, FILES_NUMBER, WORDS_NUMBER, lambda1, eps1,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, 0, WORDS_NUMBER, lambda1, eps1,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, 0, lambda1, eps1,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, -0.5, eps1,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda1, -0.5,
                &calculated_model);
    free_language_model(&calculated_model);
    CU_ASSERT_FALSE_FATAL(calculated_res);

    calculated_res = calculate_language_model(
                words_MLF_data, FILES_NUMBER, WORDS_NUMBER, lambda1, eps1,
                NULL);
    CU_ASSERT_FALSE_FATAL(calculated_res);
}
