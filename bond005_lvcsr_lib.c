/* bond005_lvcsr_lib.c
 *
 * This file is part of bond005_lvcsr.
 *
 * bond005_lvcsr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bond005_lvcsr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bond005_lvcsr. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:    Ivan Yu. Bondarenko.
 * Version:   0.1
 * Date:      2014
 * Copyright: GNU General Public License version 3.
 */

#include <float.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bond005_lvcsr_lib.h"

#define INTERVAL_10MSECS 100000.0
#define MAX_REPEATS_OF_PHONEME 2
#define HISTOGRAM_SIZE 20

/* Structure for representation of one cell of the matrix which is used in the
 * Viterbi Beam Search algorithm (see X.Huang, Spoken Language Processing,
 * pp.618-620). */
typedef struct _TViterbiMatrixCell {
    float cost; // total cost
    int btp;    // backtrack pointer
} TViterbiMatrixCell;

/* Structure for representation of traceback array. This array is used to
 * record the decision about the best word end hypothesis and its start time
 * for each time frame. */
typedef struct _TTracebackArrayItem {
    int start_time;
    int predecessor_word;
} TTracebackArrayItem;
typedef TTracebackArrayItem* TTracebackArray;

/* Structure for representation of whole matrix which is used in the Viterbi
 * Beam Search algorithm (see X.Huang, Spoken Language Processing,
 * pp.618-620). */
typedef struct _TViterbiMatrix {
    int times_number;           /* times number (i.e. length of recognized
                                   phones sequence) */
    int words_number;           /* number of words in vocabulary, including
                                   identical words with different variants of
                                   transcription */
    int *words_indexes;         /* indexes of words in vocabulary (some words
                                   may be identical, though they have different
                                   variants of transcription) */
    int *words_sizes;           /* lengths of words (i.e. numbers of states in
                                   each word) */
    TViterbiMatrixCell ***cells;/* 3-dimension matrix for the Viterbi Beam
                                   Search algorithm */
} TViterbiMatrix;

typedef struct _THistogram {
    int number;
    float left, right;
} THistogram;

void set_new_file_extension(char src[], char extension[])
{
    int i, n, n_extension;
    if ((src == NULL) || (extension == NULL))
    {
        return;
    }
    n_extension = strlen(extension);
    n = strlen(src);
    i = n-1;
    while (i >= 0)
    {
        if (src[i] == '.')
        {
            break;
        }
        i--;
    }
    if (i >= 0)
    {
        i++;
        memmove(&src[i], &extension[0], n_extension * sizeof(char));
        src[i+n_extension] = 0;
    }
    else
    {
        src[n] = '.';
        memmove(&src[n+1], &extension[0], n_extension * sizeof(char));
        src[n+n_extension+1] = 0;
    }
}

/* This function allocates memory for data used in the Viterbi Beam Search
 * algorithm. The data are represented by three-dimension matrix (see
 * description of TViterbiMatrix structure). First dimension is time, second
 * dimension is words lexicon, and third dimension is words states.
 *
 * The time dimension size is equal to 2 always to use sparingly the allocated
 * memory (only current and previous time points are being saved at each step).
 *
 * The words dimension size is equal to words lexicon size.
 *
 * The states dimension size is different for various words. Each word has as
 * many states as it has phonemes according to the lexicon. Besides, additional
 * state as initial pseudo-state is inserted in front of the real first word
 * state (this pseudo-state is necessary for implementation of inter-word
 * transition rules).
 */
static void create_viterbi_matrix(
        TViterbiMatrix* data, int words_number, TLinearWordsLexicon lexicon[])
{
    int t, w, s;
    if ((data == NULL) || (words_number <= 0) || (lexicon == NULL))
    {
        return;
    }
    data->times_number = 0;
    data->words_number = words_number;
    data->words_sizes = malloc(sizeof(int) * words_number);
    data->words_indexes = malloc(sizeof(int) * words_number);
    for (w = 0; w < words_number; w++)
    {
        data->words_sizes[w] = lexicon[w].phonemes_number;
        data->words_indexes[w] = lexicon[w].word_index;
    }
    data->cells = malloc(2 * sizeof(TViterbiMatrixCell**));
    for (t = 0; t < 2; t++)
    {
        data->cells[t] = malloc(words_number * sizeof(TViterbiMatrixCell*));
        for (w = 0; w < words_number; w++)
        {
            data->cells[t][w] = malloc((data->words_sizes[w]+1)
                                       * sizeof(TViterbiMatrixCell));
            for (s = 0; s <= data->words_sizes[w]; s++)
            {
                data->cells[t][w][s].btp = -1;
                data->cells[t][w][s].cost = -FLT_MAX;
            }
        }
    }
}

/* This function shifts the Viterbi matrix data from right to left along time
 * axis.
 */
static void shift_viterbi_matrix(TViterbiMatrix data)
{
    int w, s;
    for (w = 0; w < data.words_number; w++)
    {
        for (s = 0; s <= data.words_sizes[w]; s++)
        {
            data.cells[0][w][s].btp = data.cells[1][w][s].btp;
            data.cells[0][w][s].cost = data.cells[1][w][s].cost;

            data.cells[1][w][s].btp = -1;
            data.cells[1][w][s].cost = -FLT_MAX;
        }
    }
}

/* This function initializes all cells of the given Viterbi matrix as follows:
 * -FLT_MAX values will be assigned to all costs, and NULL values will be
 * assigned to all backtrack pointers (herewith memory allocated for these
 * backtrack pointers will be freed). */
static void initialize_values_of_viterbi_matrix(TViterbiMatrix data)
{
    int t, w, s;
    for (t = 0; t < 2; t++)
    {
        for (w = 0; w < data.words_number; w++)
        {
            for (s = 0; s <= data.words_sizes[w]; s++)
            {
                data.cells[t][w][s].btp = -1;
                data.cells[t][w][s].cost = -FLT_MAX;
            }
        }
    }
}

/* This function frees all memory which was allocated for the Viterbi matrix.
 */
static void delete_viterbi_matrix(TViterbiMatrix* data)
{
    int t, w;
    if (data == NULL)
    {
        return;
    }
    if ((data->words_number <= 0) || (data->times_number <= 0)
            || (data->words_sizes == NULL) || (data->words_indexes == NULL)
            || (data->cells == NULL))
    {
        return;
    }

    for (t = 0; t < 2; t++)
    {
        if (data->cells[t] == NULL)
        {
            continue;
        }
        for (w = 0; w < data->words_number; w++)
        {
            if (data->cells[t][w] == NULL)
            {
                continue;
            }
            free(data->cells[t][w]);
            data->cells[t][w] = NULL;
        }
        free(data->cells[t]);
        data->cells[t] = NULL;
    }

    free(data->words_sizes);
    data->words_sizes = NULL;
    free(data->words_indexes);
    data->words_indexes = NULL;
    free(data->cells);
    data->cells = NULL;
    data->words_number = 0;
    data->times_number = 0;
}

/* This function finds the bigram like "start_word_i->end_word_i" in the
 * language model. Starting position of search is specified by the
 * cur_bigram_i argument. As result this function returns probability of the
 * found bigram or 0 if the bigram isn't found. Also, specified starting
 * position of bigram search is being shifted from left to right in process of
 * this search, and such changing of starting position is saved. */
static float find_bigram(TLanguageModel language_model, float lambda,
                         int start_word_i, int end_word_i, int *cur_bigram_i)
{
    int i = *cur_bigram_i;
    float bigram_probability = 0.0;

    while (i < language_model.bigrams[end_word_i].begins_number)
    {
        if (language_model.bigrams[end_word_i].begins[i].word_i>=start_word_i)
        {
            break;
        }
        i++;
    }
    *cur_bigram_i = i;
    if (i < language_model.bigrams[end_word_i].begins_number)
    {
        if (language_model.bigrams[end_word_i].begins[i].word_i==start_word_i)
        {
            bigram_probability
                    = language_model.bigrams[end_word_i].begins[i].probability;
        }
    }

    bigram_probability = lambda * bigram_probability + (1.0 - lambda)
            * language_model.unigrams_probabilities[end_word_i];

    return bigram_probability;
}

static void prune_hypotheses(TViterbiMatrix data, int t, float pruning_coeff,
                             THistogram histogram[])
{
    int w, s, i;
    float max_cost, min_cost, dcost, cost_threshold;
    int number_of_all_hypotheses = 0, number_of_unpruned_hypotheses = 0;
    int max_number_of_unpruned_hypotheses;

    if (pruning_coeff <= 0)
    {
        return;
    }

    w = 0; s = 1;
    max_cost = data.cells[t][w][s].cost;
    min_cost = data.cells[t][w][s].cost;
    for (w = 0; w < data.words_number; w++)
    {
        for (s = 1; s <= data.words_sizes[w]; s++)
        {
            if (data.cells[t][w][s].cost <= (-FLT_MAX + FLT_EPSILON))
            {
                continue;
            }
            if (data.cells[t][w][s].cost > max_cost)
            {
                max_cost = data.cells[t][w][s].cost;
            }
            if (data.cells[t][w][s].cost < min_cost)
            {
                min_cost = data.cells[t][w][s].cost;
            }
        }
        number_of_all_hypotheses += data.words_sizes[w];
    }
    if ((max_cost <= (-FLT_MAX + FLT_EPSILON))
            || (min_cost <= (-FLT_MAX + FLT_EPSILON)))
    {
        return;
    }
    if ((max_cost - min_cost) <= FLT_EPSILON)
    {
        return;
    }
    max_number_of_unpruned_hypotheses = (int)floor(
                (1.0 - pruning_coeff) * number_of_all_hypotheses + 0.5);
    if (max_number_of_unpruned_hypotheses >= number_of_all_hypotheses)
    {
        return;
    }

    dcost = (max_cost - min_cost) / (HISTOGRAM_SIZE - 1);
    histogram[0].number = 0;
    histogram[0].left = max_cost - dcost;
    histogram[0].right = max_cost;
    for (i = 1; i < (HISTOGRAM_SIZE - 1); i++)
    {
        histogram[i].number = 0;
        histogram[i].right = histogram[i-1].left;
        histogram[i].left = histogram[i].right - dcost;
    }
    histogram[HISTOGRAM_SIZE-1].right = histogram[HISTOGRAM_SIZE-2].left;
    histogram[HISTOGRAM_SIZE-1].left = -FLT_MAX;
    histogram[HISTOGRAM_SIZE-1].number = 0;

    for (w = 0; w < data.words_number; w++)
    {
        for (s = 1; s <= data.words_sizes[w]; s++)
        {
            i = 0;
            while (i < HISTOGRAM_SIZE)
            {
                if ((histogram[i].left <= data.cells[t][w][s].cost)
                        && (data.cells[t][w][s].cost <= histogram[i].right))
                {
                    histogram[i].number++;
                    break;
                }
                i++;
            }
        }
    }

    i = 1;
    cost_threshold = histogram[0].left;
    number_of_unpruned_hypotheses = histogram[0].number;
    while (i < HISTOGRAM_SIZE)
    {
        if (number_of_unpruned_hypotheses > max_number_of_unpruned_hypotheses)
        {
            break;
        }
        cost_threshold = histogram[i].left;
        number_of_unpruned_hypotheses += histogram[i].number;
        i++;
    }

    for (w = 0; w < data.words_number; w++)
    {
        for (s = 1; s <= data.words_sizes[w]; s++)
        {
            if ((data.cells[t][w][s].cost <= cost_threshold)
                    && (data.cells[t][w][s].cost > (-FLT_MAX + FLT_EPSILON)))
            {
                data.cells[t][w][s].cost = -FLT_MAX;
                data.cells[t][w][s].btp = -1;
            }
        }
    }
}

static int calculate_viterbi_matrix(
        TViterbiMatrix data, TTracebackArray traceback_array,
        int src_phonemes_sequence[], float src_phonemes_weights[],
        int phonemes_vocabulary_size, float confusion_penalties[],
        TLinearWordsLexicon words_lexicon[], float pruning_coeff,
        TLanguageModel language_model, float lambda)
{
    int nwords = data.words_number;
    int is_ok = 1;
    int bigram_i, inp_phoneme_i, trg_phoneme_i;
    int t_count, t = 0, w, s, S, i, v, v_max;
    float tmp_val1, tmp_val2, tmp_d, bigram_probability;
    int *predecessors = malloc(sizeof(int) * data.words_number);
    THistogram histogram_for_pruning[HISTOGRAM_SIZE];
    //double start_time, end_time; // for debug

    t = 0; s = 1;
    inp_phoneme_i = src_phonemes_sequence[t];
    for (w = 0; w < data.words_number; w++)
    {
        trg_phoneme_i = words_lexicon[w].phonemes_indexes[s-1];
        i = trg_phoneme_i * phonemes_vocabulary_size + inp_phoneme_i;
        //i = inp_phoneme_i * phonemes_vocabulary_size + trg_phoneme_i;
        if (confusion_penalties[i] > (-FLT_MAX + FLT_EPSILON))
        {
            data.cells[t][w][s].cost = src_phonemes_weights[t]
                    + confusion_penalties[i];
        }
        else
        {
            data.cells[t][w][s].cost = -FLT_MAX;
        }
    }

    for (t_count = 1; t_count < data.times_number; t_count++)
    {
        t++;
        if (t >= 2)
        {
            t = 1;
            shift_viterbi_matrix(data);
        }
        inp_phoneme_i = src_phonemes_sequence[t_count];
        //start_time = omp_get_wtime(); // for debug
        for (w = 0; w < nwords; w++)
        {
            s = 1;
            trg_phoneme_i = words_lexicon[w].phonemes_indexes[s-1];
            tmp_val1 = data.cells[t-1][w][s].cost;
            if (tmp_val1 > (-FLT_MAX + FLT_EPSILON))
            {
                tmp_d = src_phonemes_weights[t_count];
                i = trg_phoneme_i * phonemes_vocabulary_size + inp_phoneme_i;
                //i = inp_phoneme_i * phonemes_vocabulary_size + trg_phoneme_i;
                if (confusion_penalties[i] > (-FLT_MAX + FLT_EPSILON))
                {
                    tmp_d += confusion_penalties[i];
                    tmp_val1 += tmp_d;
                }
                else
                {
                    tmp_val1 = -FLT_MAX;
                }
            }
            data.cells[t][w][s].cost = tmp_val1;
            data.cells[t][w][s].btp = data.cells[t-1][w][s].btp;

            for (s = 2; s < data.words_sizes[w]; s++)
            {
                trg_phoneme_i = words_lexicon[w].phonemes_indexes[s-1];
                tmp_d = src_phonemes_weights[t_count];
                i = trg_phoneme_i * phonemes_vocabulary_size + inp_phoneme_i;
                //i = inp_phoneme_i * phonemes_vocabulary_size + trg_phoneme_i;
                if (confusion_penalties[i] > (-FLT_MAX + FLT_EPSILON))
                {
                    tmp_d += confusion_penalties[i];
                    tmp_val1 = data.cells[t-1][w][s-1].cost;
                    if (tmp_val1 > (-FLT_MAX + FLT_EPSILON))
                    {
                        tmp_val1 += tmp_d;
                    }
                    tmp_val2 = data.cells[t-1][w][s].cost;
                    if (tmp_val2 > (-FLT_MAX + FLT_EPSILON))
                    {
                        tmp_val2 += tmp_d;
                    }
                }
                else
                {
                    tmp_val1 = -FLT_MAX;
                    tmp_val2 = -FLT_MAX;
                }
                if (tmp_val1 >= tmp_val2)
                {
                    data.cells[t][w][s].cost = tmp_val1;
                    data.cells[t][w][s].btp = data.cells[t-1][w][s-1].btp;
                }
                else
                {
                    data.cells[t][w][s].cost = tmp_val2;
                    data.cells[t][w][s].btp = data.cells[t-1][w][s].btp;
                }
            }

            s = data.words_sizes[w];
            tmp_val1 = data.cells[t][w][s-1].cost;
            tmp_val2 = data.cells[t-1][w][s].cost;
            if (tmp_val2 > (-FLT_MAX + FLT_EPSILON))
            {
                trg_phoneme_i = words_lexicon[w].phonemes_indexes[s-1];
                tmp_d = src_phonemes_weights[t_count];
                i = trg_phoneme_i * phonemes_vocabulary_size + inp_phoneme_i;
                //i = inp_phoneme_i * phonemes_vocabulary_size + trg_phoneme_i;
                if (confusion_penalties[i] > (-FLT_MAX + FLT_EPSILON))
                {
                    tmp_val2 += (tmp_d + confusion_penalties[i]);
                }
                else
                {
                    tmp_val2 = -FLT_MAX;
                }
            }
            if (tmp_val1 >= tmp_val2)
            {
                data.cells[t][w][s].cost = tmp_val1;
                data.cells[t][w][s].btp = data.cells[t][w][s-1].btp;
            }
            else
            {
                data.cells[t][w][s].cost = tmp_val2;
                data.cells[t][w][s].btp = data.cells[t-1][w][s].btp;
            }
        }
        //end_time = omp_get_wtime(); // for debug
        //printf("%.4f\t", end_time - start_time); // for debug

        //start_time = omp_get_wtime(); // for debug
        prune_hypotheses(data, t, pruning_coeff, histogram_for_pruning);
        //end_time = omp_get_wtime(); // for debug
        //printf("%.4f\t", end_time - start_time); // for debug

        //start_time = omp_get_wtime(); // for debug
        #pragma omp parallel for private(v,v_max,S,bigram_i,\
                                         bigram_probability,tmp_val1,tmp_val2)
        for (w = 0; w < data.words_number; w++)
        {
            bigram_i = 0;
            v_max = 0;
            S = data.words_sizes[v_max];
            if (data.cells[t][v_max][S].cost > (-FLT_MAX + FLT_EPSILON))
            {
                bigram_probability = find_bigram(
                            language_model, lambda, data.words_indexes[v_max],
                            data.words_indexes[w], &bigram_i);
                if (bigram_probability > 0.0)
                {
                    tmp_val1 = log10(bigram_probability)
                            + data.cells[t][v_max][S].cost;
                }
                else
                {
                    tmp_val1 = -FLT_MAX;
                }
            }
            else
            {
                tmp_val1 = -FLT_MAX;
            }
            for (v = 1; v < data.words_number; v++)
            {
                S = data.words_sizes[v];
                if (data.cells[t][v][S].cost > (-FLT_MAX + FLT_EPSILON))
                {
                    bigram_probability = find_bigram(
                                language_model, lambda, data.words_indexes[v],
                                data.words_indexes[w], &bigram_i);
                    if (bigram_probability > FLT_EPSILON)
                    {
                        tmp_val2 = log10(bigram_probability)
                                + data.cells[t][v][S].cost;
                        if (tmp_val2 > tmp_val1)
                        {
                            v_max = v;
                            tmp_val1 = tmp_val2;
                        }
                    }
                }
            }

            data.cells[t][w][0].cost = tmp_val1;
            data.cells[t][w][0].btp = t_count-1;

            if (data.cells[t][w][0].cost > data.cells[t][w][1].cost)
            {
                data.cells[t][w][1].cost = data.cells[t][w][0].cost;
                data.cells[t][w][1].btp = data.cells[t][w][0].btp;
            }

            predecessors[w] = v_max;
        }
        //end_time = omp_get_wtime(); // for debug
        //printf("%.4f\n", end_time - start_time); // for debug

        v_max = 0;
        for (v = 1; v < data.words_number; v++)
        {
            if (data.cells[t][v][data.words_sizes[v]].cost
                    > data.cells[t][v_max][data.words_sizes[v_max]].cost)
            {
                v_max = v;
            }
        }
        if (data.cells[t][v_max][data.words_sizes[v_max]].cost
                <= (-FLT_MAX + FLT_EPSILON))
        {
            is_ok = 0;
            break;
        }
        traceback_array[t_count-1].predecessor_word
                = data.words_indexes[predecessors[v_max]];
        traceback_array[t_count-1].start_time
                = data.cells[t][v_max][data.words_sizes[v_max]].btp;
    }
    if (is_ok)
    {
        v_max = 0;
        for (v = 1; v < data.words_number; v++)
        {
            if (data.cells[t][v][data.words_sizes[v]].cost
                    > data.cells[t][v_max][data.words_sizes[v_max]].cost)
            {
                v_max = v;
            }
        }
        if (data.cells[t][v_max][data.words_sizes[v_max]].cost
                <= (-FLT_MAX + FLT_EPSILON))
        {
            is_ok = 0;
        }
        else
        {
            traceback_array[data.times_number-1].predecessor_word
                    = data.words_indexes[v_max];
            traceback_array[data.times_number-1].start_time
                    = data.cells[t][v_max][data.words_sizes[v_max]].btp;
        }
    }

    free(predecessors);

    return is_ok;
}

static int get_words_sequence_by_traceback_array(
        TTracebackArray traceback_array, int traceback_array_length,
        int recognized_words_sequence[])
{
    int tmp, i = traceback_array_length - 1, n = 0;

    while (i >= 0)
    {
        if (traceback_array[i].start_time < 0)
        {
            break;
        }
        recognized_words_sequence[n++] = traceback_array[i].predecessor_word;
        i = traceback_array[i].start_time - 1;
    }

    for (i = 0; i < (n / 2); i++)
    {
        tmp = recognized_words_sequence[i];
        recognized_words_sequence[i] = recognized_words_sequence[n-i-1];
        recognized_words_sequence[n-i-1] = tmp;
    }

    return n;
}

static int create_phonemes_sequence_by_transcription(
        TTranscriptionNode transcription[], int transcription_length,
        int phonemes_sequence[], float phonemes_weights[])
{
    int is_ok = 1;
    int start_ind = -1, end_ind = -1, i, j, k;
    int number_of_steps_10ms, phonemes_sequence_length = 0;
    float cur_phoneme_weight;

    for (i = 0; i < transcription_length; i++)
    {
        if (transcription[i].probability <= 0.0)
        {
            is_ok = 0;
            break;
        }
        if (start_ind < 0)
        {
            if (transcription[i].node_data > 0)
            {
                start_ind = i;
            }
        }
        else
        {
            if (transcription[i].node_data > 0)
            {
                end_ind = -1;
            }
            else
            {
                end_ind = i-1;
            }
        }
    }
    if (!is_ok)
    {
        return -1;
    }
    if (start_ind < 0)
    {
        return 0;
    }
    if (end_ind < 0)
    {
        end_ind = transcription_length - 1;
    }

    for (i = start_ind; i <= end_ind; i++)
    {
        number_of_steps_10ms = (int)floor((transcription[i].end_time
                                           - transcription[i].start_time)
                                          / INTERVAL_10MSECS + 0.5);
        if (number_of_steps_10ms > MAX_REPEATS_OF_PHONEME)
        {
            number_of_steps_10ms = MAX_REPEATS_OF_PHONEME;
        }
        else
        {
            if (number_of_steps_10ms < 1)
            {
                number_of_steps_10ms = 1;
            }
        }
        phonemes_sequence_length += number_of_steps_10ms;
    }
    if (phonemes_sequence == NULL)
    {
        return phonemes_sequence_length;
    }

    j = 0;
    for (i = start_ind; i <= end_ind; i++)
    {
        number_of_steps_10ms = (int)floor((transcription[i].end_time
                                           - transcription[i].start_time)
                                          / INTERVAL_10MSECS + 0.5);
        if (number_of_steps_10ms > MAX_REPEATS_OF_PHONEME)
        {
            number_of_steps_10ms = MAX_REPEATS_OF_PHONEME;
        }
        else
        {
            if (number_of_steps_10ms < 1)
            {
                number_of_steps_10ms = 1;
            }
        }
        cur_phoneme_weight = log10(transcription[i].probability);
        for (k = 0; k < number_of_steps_10ms; k++)
        {
            phonemes_sequence[j+k] = transcription[i].node_data;
            phonemes_weights[j+k] = cur_phoneme_weight;
        }
        j += number_of_steps_10ms;
    }

    return phonemes_sequence_length;
}

static int compare_begins_of_bigrams(const void *ptr1, const void *ptr2)
{
    TWordBigramBegin *item1 = (TWordBigramBegin*)ptr1;
    TWordBigramBegin *item2 = (TWordBigramBegin*)ptr2;

    return (item1->word_i - item2->word_i);
}

static int compare_items_of_words_lexicon(const void *ptr1, const void *ptr2)
{
    TLinearWordsLexicon *item1 = (TLinearWordsLexicon*)ptr1;
    TLinearWordsLexicon *item2 = (TLinearWordsLexicon*)ptr2;

    return (item1->word_index - item2->word_index);
}

int read_string(FILE *read_file, char *str)
{
    int start_pos = 0, end_pos = 0;

    if ((read_file == NULL) || (str == NULL))
    {
        return 0;
    }

    memset(str, 0, BUFFER_SIZE);
    if (fgets(str, BUFFER_SIZE, read_file) == NULL)
    {
        return 0;
    }

    while (str[start_pos] != 0)
    {
        if ((str[start_pos] != ' ') && (str[start_pos] != '\t')
                && (str[start_pos] != '\n') && (str[start_pos] != '\r'))
        {
            break;
        }
        start_pos++;
    }
    if (str[start_pos] == 0)
    {
        memset(str, 0, BUFFER_SIZE);
        return 0;
    }

    end_pos = strlen(str) - 1;
    while (end_pos > start_pos)
    {
        if ((str[end_pos] != ' ') && (str[end_pos] != '\t')
                && (str[end_pos] != '\n') && (str[end_pos] != '\r'))
        {
            break;
        }
        end_pos--;
    }
    str[end_pos+1] = 0;

    if (start_pos > 0)
    {
        memmove(&str[0], &str[start_pos], end_pos-start_pos+2);
    }

    return (end_pos - start_pos + 1);
}

int prepare_filename(char *filename)
{
    int i, n;

    if (filename == NULL)
    {
        return 0;
    }

    n = strlen(filename);
    if (n <= 0)
    {
        return 0;
    }
    if (filename[0] == '"')
    {
        if (n > 1)
        {
            if (filename[n-1] == '"')
            {
                if (n > 2)
                {
                    filename[n-1] = 0;
                    memmove(&filename[0], &filename[1], (n-1) * sizeof(char));
                    n -= 2;

                    i = 0;
                    while (i < n)
                    {
                        if ((filename[i]!=' ') && (filename[i]!='\t')
                                && (filename[i]!='\n')  && (filename[i]!='\r'))
                        {
                            break;
                        }
                        i++;
                    }
                    if (i < n)
                    {
                        memmove(&filename[0], &filename[i], n-i+1);
                        n = n-i;

                        i = n-1;
                        while (i > 0)
                        {
                            if ((filename[i]!=' ')&&(filename[i]!='\t')
                                    &&(filename[i]!='\r')&&(filename[i]!='\n'))
                            {
                                break;
                            }
                            i--;
                        }
                        filename[i+1] = 0;
                        n = i+1;
                    }
                    else
                    {
                        filename[0] = 0;
                        n = 0;
                    }
                }
                else
                {
                    n = 0;
                }
            }
            else
            {
                n = 0;
            }
        }
        else
        {
            n = 0;
        }
    }
    else
    {
        if (filename[n-1] == '"')
        {
            n = 0;
        }
    }

    return n;
}

int find_in_vocabulary(char *vocabulary[], int vocabulary_size,
                       char *found_name, int *is_equal)
{
    if (is_equal != NULL)
    {
        *is_equal = 0;
    }
    if ((found_name == NULL) || (vocabulary_size <= 0) || (vocabulary == NULL))
    {
        return -1;
    }

    int compare_res, first_pos = 0, last_pos = vocabulary_size, middle_pos;

    compare_res = strcmp(vocabulary[0], found_name);
    if (compare_res >= 0)
    {
        if ((compare_res == 0) && (is_equal != NULL))
        {
            *is_equal = 1;
        }
        return 0;
    }

    compare_res = strcmp(vocabulary[last_pos-1], found_name);
    if (compare_res <= 0)
    {
        if (compare_res == 0)
        {
            if (is_equal != NULL)
            {
                *is_equal = 1;
            }
        }
        return last_pos-1;
    }

    while (first_pos < last_pos)
    {
        middle_pos = first_pos + (last_pos - first_pos) / 2;
        compare_res = strcmp(found_name, vocabulary[middle_pos]);
        if (compare_res == 0)
        {
            if (is_equal != NULL)
            {
                *is_equal = 1;
            }
            last_pos = middle_pos;
            break;
        }
        else if (compare_res < 0)
        {
            last_pos = middle_pos;
        }
        else {
            first_pos = middle_pos + 1;
        }
    }

    return last_pos;
}

int find_in_unsorted_vocabulary(char *vocabulary[], int vocabulary_size,
                                char *found_name)
{
    int i, res = -1;

    if ((found_name == NULL) || (vocabulary_size <= 0) || (vocabulary == NULL))
    {
        return -1;
    }

    for (i = 0; i < vocabulary_size; i++)
    {
        if (vocabulary[i] != NULL)
        {
            if (strcmp(vocabulary[i], found_name) == 0)
            {
                res = i;
                break;
            }
        }
    }

    return res;
}

int string_to_transcription_node(char *str, char *phonemes_vocabulary[],
                                 int phonemes_number, PTranscriptionNode node)
{
    char *start_time_label = NULL, *end_time_label = NULL;
    char *node_name = NULL, *probability_str = NULL;

    if ((phonemes_number <= 0) || (phonemes_vocabulary == NULL)
            || (str == NULL))
    {
        return 0;
    }

    start_time_label = strtok(str, " \t");
    if (start_time_label == NULL)
    {
        return 0;
    }
    end_time_label = strtok(NULL, " \t");
    if (end_time_label == NULL)
    {
        return 0;
    }
    node_name = strtok(NULL, " \t");
    if (node_name == NULL)
    {
        return 0;
    }
    probability_str = strtok(NULL, " \t");
    if (probability_str != NULL)
    {
        if (strtok(NULL, " \t") != NULL)
        {
            return 0;
        }
    }

    if (sscanf(start_time_label, "%lu", &(node->start_time)) != 1)
    {
        return 0;
    }

    if (sscanf(end_time_label, "%lu", &(node->end_time)) != 1)
    {
        return 0;
    }
    if (node->end_time <= node->start_time)
    {
        return 0;
    }

    node->node_data = find_in_unsorted_vocabulary(
                phonemes_vocabulary, phonemes_number, node_name);
    if (node->node_data < 0)
    {
        return 0;
    }

    if (probability_str == NULL)
    {
        node->probability = 1.0;
    }
    else
    {
        if (sscanf(probability_str, "%f", &(node->probability)) != 1)
        {
            return 0;
        }
        if ((node->probability < 0.0) || (node->probability > 1.0))
        {
            return 0;
        }
    }

    return 1;
}

int select_word_and_transcription(char *str, char **word_substr,
                                  char **transcription_substr)
{
    int i, j;

    if ((str == NULL) || (word_substr == NULL)
            || (transcription_substr == NULL))
    {
        return 0;
    }

    *word_substr = NULL;
    *transcription_substr = NULL;

    i = 0;
    while (str[i] != 0)
    {
        if (str[i] == '=')
        {
            break;
        }
        i++;
    }
    if (str[i] != '=')
    {
        return 0;
    }

    j = i - 1;
    while (j >= 0)
    {
        if ((str[j] != ' ') && (str[j] != '\t') && (str[j] != '\n')
                && (str[j] != '\r'))
        {
            break;
        }
        j--;
    }
    j++;
    if (j <= 0)
    {
        return 0;
    }
    str[j] = 0;
    *word_substr = str;

    j = i+1;
    while (str[j] != 0)
    {
        if ((str[j] != ' ') && (str[j] != '\t') && (str[j] != '\n')
                && (str[j] != '\r'))
        {
            break;
        }
        j++;
    }
    if (str[j] == 0)
    {
        *word_substr = NULL;
        return 0;
    }
    *transcription_substr = &str[j];
    return 1;
}

int parse_transcription_str(char *transcription_str,char **phonemes_vocabulary,
                            int phonemes_number, int phonemes_sequence[])
{
    char *phoneme_name = NULL;
    int i = 0, is_ok = 1;

    if ((transcription_str == NULL) || (phonemes_vocabulary == NULL)
            || (phonemes_number <= 0) || (phonemes_sequence == NULL))
    {
        return 0;
    }

    phoneme_name = strtok(transcription_str, " \t");
    if (phoneme_name == NULL)
    {
        return 0;
    }
    phonemes_sequence[i] = find_in_unsorted_vocabulary(
                phonemes_vocabulary, phonemes_number, phoneme_name);
    if (phonemes_sequence[i] < 0)
    {
        return 0;
    }
    i++;
    phoneme_name = strtok(NULL, " \t");

    while (phoneme_name != NULL)
    {
        phonemes_sequence[i] = find_in_unsorted_vocabulary(
                    phonemes_vocabulary, phonemes_number, phoneme_name);
        if (phonemes_sequence[i] < 0)
        {
            is_ok = 0;
            break;
        }
        i++;
        phoneme_name = strtok(NULL, " \t");
    }

    if (!is_ok)
    {
        return 0;
    }
    return i;
}

int add_word_to_words_tree(int word_index, int word_phonemes[],int word_length,
                           PWordsTreeNode words_tree_root)
{
    int is_ok = 1, i, n, next_i;
    PWordsTreeNode next_node;

    if ((word_index < 0) || (word_phonemes == NULL) || (word_length <= 0)
            || (words_tree_root == NULL))
    {
        return 0;
    }
    if (words_tree_root->node_type != INIT_NODE)
    {
        return 0;
    }

    for (i = 0; i < word_length; i++)
    {
        if (word_phonemes[i] <= 0)
        {
            is_ok = 0;
            break;
        }
        n = words_tree_root->number_of_next_nodes;
        next_i = n - 1;
        next_node = words_tree_root->next_nodes + next_i;
        while (next_i >= 0)
        {
            if (next_node->node_type != PHONEME_NODE)
            {
                next_i--;
                next_node--;
            }
            else
            {
                if (next_node->node_data == word_phonemes[i])
                {
                    break;
                }
                next_i--;
                next_node--;
            }
        }
        if (next_i < 0)
        {
            words_tree_root->next_nodes = realloc(
                        words_tree_root->next_nodes,
                        (n+1) * sizeof(TWordsTreeNode));
            next_node = words_tree_root->next_nodes + n;
            words_tree_root->number_of_next_nodes++;
            next_node->node_data = word_phonemes[i];
            next_node->node_type = PHONEME_NODE;
            next_node->number_of_next_nodes = 0;
            next_node->next_nodes = NULL;
        }
        words_tree_root = next_node;
    }
    if (!is_ok)
    {
        return 0;
    }

    n = words_tree_root->number_of_next_nodes;
    next_i = n - 1;
    next_node = words_tree_root->next_nodes + next_i;
    while (next_i >= 0)
    {
        if (next_node->node_type == WORD_NODE)
        {
            break;
        }
        next_i--;
        next_node--;
    }
    if (next_i >= 0)
    {
        if (next_node->node_data != word_index)
        {
            return 0;
        }
        return 1;
    }

    words_tree_root->next_nodes = realloc(
                words_tree_root->next_nodes, (n+1) * sizeof(TWordsTreeNode));
    next_node = words_tree_root->next_nodes + n;
    words_tree_root->number_of_next_nodes++;
    next_node->node_data = word_index;
    next_node->node_type = WORD_NODE;
    next_node->number_of_next_nodes = 0;
    next_node->next_nodes = NULL;
    return 1;
}

void free_words_tree_node(PWordsTreeNode deleted_node)
{
    PWordsTreeNode next_node = NULL;
    int i;

    if (deleted_node == NULL)
    {
        return;
    }

    if ((deleted_node->number_of_next_nodes > 0) && (deleted_node->next_nodes != NULL))
    {
        for (i = 0; i < deleted_node->number_of_next_nodes; i++)
        {
            next_node = deleted_node->next_nodes + i;
            free_words_tree_node(next_node);
        }
        free(deleted_node->next_nodes);
        deleted_node->next_nodes = NULL;
    }
}

int load_phonemes_MLF(char *mlf_name, char **phonemes_vocabulary,
                      int phonemes_number, TMLFFilePart **mlf_data)
{
    int buffer_size = 0, n_files = 0, n_transcription = 0;
    int reading_state = HEADER_EXPECTATION_STATE;
    int is_ok = 1;
    char buffer[BUFFER_SIZE];
    FILE *mlf_file = NULL;
    TMLFFilePart *cur_mlf_part = NULL;
    TTranscriptionNode new_node;
    PTranscriptionNode last_node_ptr = NULL;

    if ((mlf_data == NULL) || (mlf_name == NULL)
            || (phonemes_vocabulary == NULL) || (phonemes_number <= 0))
    {
        return 0;
    }

    *mlf_data = NULL;
    new_node.start_time = 0;
    new_node.end_time = 0;
    new_node.probability = 1.0;
    new_node.node_data = -1;

    mlf_file = fopen(mlf_name, "r");
    if (mlf_file == NULL)
    {
        return 0;
    }
    while (!feof(mlf_file))
    {
        buffer_size = read_string(mlf_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        switch (reading_state)
        {
        case FILENAME_READING_STATE:
            buffer_size = prepare_filename(buffer);
            if (buffer_size <= 0)
            {
                is_ok = 0;
                break;
            }
            n_files++;
            *mlf_data = realloc(*mlf_data, n_files * sizeof(TMLFFilePart));
            cur_mlf_part = *mlf_data + n_files - 1;
            cur_mlf_part->name = malloc((buffer_size+1) * sizeof(char));
            memset(cur_mlf_part->name, 0, buffer_size+1);
            strcpy(cur_mlf_part->name, buffer);
            cur_mlf_part->transcription = NULL;
            cur_mlf_part->transcription_size = 0;
            last_node_ptr = NULL;
            n_transcription = 0;
            reading_state = EVENT_READING_STATE;
            break;
        case EVENT_READING_STATE:
            if (strcmp(buffer, ".") == 0)
            {
                if (n_transcription <= 0)
                {
                    is_ok = 0;
                }
                else
                {
                    reading_state = FILENAME_READING_STATE;
                }
            }
            else
            {
                if (!string_to_transcription_node(buffer, phonemes_vocabulary,
                                                  phonemes_number, &new_node))
                {
                    is_ok = 0;
                }
                else
                {
                    if (last_node_ptr != NULL)
                    {
                        if (last_node_ptr->end_time > new_node.start_time)
                        {
                            is_ok = 0;
                        }
                    }
                }
                if (is_ok)
                {
                    n_transcription++;
                    cur_mlf_part->transcription_size++;
                    cur_mlf_part->transcription = realloc(
                                cur_mlf_part->transcription,
                                n_transcription * sizeof(TTranscriptionNode));
                    cur_mlf_part->transcription[n_transcription-1] = new_node;
                    last_node_ptr = cur_mlf_part->transcription
                            + n_transcription - 1;
                }
            }
            break;
        default:
            if (strcmp(buffer, MLF_HEADER) != 0)
            {
                is_ok = 0;
            }
            else
            {
                reading_state = FILENAME_READING_STATE;
            }
        }
        if (!is_ok)
        {
            break;
        }
    }
    fclose(mlf_file);
    if (is_ok)
    {
        if (reading_state != FILENAME_READING_STATE)
        {
            is_ok = 0;
        }
    }

    if (!is_ok)
    {
        free_MLF(mlf_data, n_files);
        n_files = 0;
    }
    return n_files;
}

int load_words_MLF(char *mlf_name, char **words_vocabulary, int words_number,
                   TMLFFilePart **mlf_data)
{
    int buffer_size = 0, n_files = 0, n_transcription = 0;
    int reading_state = HEADER_EXPECTATION_STATE;
    int is_ok = 1;
    char buffer[BUFFER_SIZE];
    FILE *mlf_file = NULL;
    TMLFFilePart *cur_mlf_part = NULL;
    TTranscriptionNode new_node;

    if ((mlf_data == NULL) || (mlf_name == NULL) || (words_vocabulary == NULL)
            || (words_number <= 0))
    {
        return 0;
    }

    *mlf_data = NULL;
    new_node.start_time = 0;
    new_node.end_time = 0;
    new_node.probability = 1.0;
    new_node.node_data = -1;

    mlf_file = fopen(mlf_name, "r");
    if (mlf_file == NULL)
    {
        return 0;
    }
    while (!feof(mlf_file))
    {
        buffer_size = read_string(mlf_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        switch (reading_state)
        {
        case FILENAME_READING_STATE:
            buffer_size = prepare_filename(buffer);
            if (buffer_size <= 0)
            {
                is_ok = 0;
                break;
            }
            n_files++;
            *mlf_data = realloc(*mlf_data, n_files * sizeof(TMLFFilePart));
            cur_mlf_part = *mlf_data + n_files - 1;
            cur_mlf_part->name = malloc((buffer_size+1) * sizeof(char));
            memset(cur_mlf_part->name, 0, buffer_size+1);
            strcpy(cur_mlf_part->name, buffer);
            cur_mlf_part->transcription = NULL;
            cur_mlf_part->transcription_size = 0;
            n_transcription = 0;
            reading_state = EVENT_READING_STATE;
            break;
        case EVENT_READING_STATE:
            if (strcmp(buffer, ".") == 0)
            {
                if (n_transcription <= 0)
                {
                    is_ok = 0;
                }
                else
                {
                    reading_state = FILENAME_READING_STATE;
                }
            }
            else
            {
                new_node.node_data = find_in_unsorted_vocabulary(
                            words_vocabulary, words_number, buffer);
                if (new_node.node_data >= 0)
                {
                    n_transcription++;
                    cur_mlf_part->transcription_size++;
                    cur_mlf_part->transcription = realloc(
                                cur_mlf_part->transcription,
                                n_transcription * sizeof(TTranscriptionNode));
                    cur_mlf_part->transcription[n_transcription-1] = new_node;
                }
                /*if (new_node.node_data < 0)
                {
                    is_ok = 0;
                }
                else
                {
                    n_transcription++;
                    cur_mlf_part->transcription_size++;
                    cur_mlf_part->transcription = realloc(
                                cur_mlf_part->transcription,
                                n_transcription * sizeof(TTranscriptionNode));
                    cur_mlf_part->transcription[n_transcription-1] = new_node;
                }*/
                /*new_node.node_data = find_in_vocabulary(
                            words_vocabulary, words_number, buffer, &is_equal);
                if (!is_equal)
                {
                    is_ok = 0;
                }
                else
                {
                    n_transcription++;
                    cur_mlf_part->transcription_size++;
                    cur_mlf_part->transcription = realloc(
                                cur_mlf_part->transcription,
                                n_transcription * sizeof(TTranscriptionNode));
                    cur_mlf_part->transcription[n_transcription-1] = new_node;
                }*/
            }
            break;
        default:
            if (strcmp(buffer, MLF_HEADER) != 0)
            {
                is_ok = 0;
                break;
            }
            reading_state = FILENAME_READING_STATE;
            break;
        }
        if (!is_ok)
        {
            break;
        }
    }
    fclose(mlf_file);
    if (is_ok)
    {
        if (reading_state != FILENAME_READING_STATE)
        {
            is_ok = 0;
        }
    }

    if (!is_ok)
    {
        free_MLF(mlf_data, n_files);
        n_files = 0;
    }
    return n_files;
}

int save_words_MLF(char *mlf_name, char **words_vocabulary, int words_number,
                   TMLFFilePart *mlf_data, int files_number)
{
    int i, j, ret = files_number;
    FILE *mlf_file = NULL;
    PTranscriptionNode node_ptr;
    char *word_name;

    if ((mlf_name == NULL) || (words_vocabulary == NULL) || (words_number <= 0)
            || (mlf_data == NULL) || (files_number <= 0))
    {
        return 0;
    }

    mlf_file = fopen(mlf_name, "w");
    if (mlf_file == NULL)
    {
        return 0;
    }

    if (fprintf(mlf_file, "%s\n", MLF_HEADER) <= 0)
    {
        ret = 0;
    }
    else
    {
        for (i = 0; i < files_number; i++)
        {
            if ((mlf_data->name == NULL) || (mlf_data->transcription == NULL)
                    || (mlf_data->transcription_size <= 0))
            {
                ret = 0;
                break;
            }
            if (fprintf(mlf_file, "\"%s\"\n", mlf_data->name) <= 0)
            {
                ret = 0;
                break;
            }
            node_ptr = mlf_data->transcription;
            for (j = 0; j < mlf_data->transcription_size; j++)
            {
                if ((node_ptr->node_data < 0)
                        || (node_ptr->node_data >= words_number))
                {
                    ret = 0;
                    break;
                }
                word_name = words_vocabulary[node_ptr->node_data];
                if (word_name == NULL)
                {
                    ret = 0;
                    break;
                }
                if (fprintf(mlf_file, "%s\n", word_name) <= 0)
                {
                    ret = 0;
                    break;
                }
                node_ptr++;
            }
            if (!ret)
            {
                break;
            }
            if (fprintf(mlf_file, "%c\n", '.') <= 0)
            {
                ret = 0;
                break;
            }
            mlf_data++;
        }
    }
    fclose(mlf_file);

    return ret;
}

int load_phonemes_vocabulary(char *file_name, char ***phonemes_vocabulary)
{
    int i, buffer_size = 0, vocabulary_size = 0;
    char buffer[BUFFER_SIZE];
    char **temp_vocabulary;
    FILE *vocabulary_file = NULL;

    if (phonemes_vocabulary == NULL)
    {
        return 0;
    }
    *phonemes_vocabulary = NULL;

    vocabulary_file = fopen(file_name, "r");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    while (!feof(vocabulary_file))
    {
        buffer_size = read_string(vocabulary_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        i = find_in_unsorted_vocabulary(*phonemes_vocabulary, vocabulary_size,
                                        buffer);
        if (i < 0)
        {
            *phonemes_vocabulary = realloc(*phonemes_vocabulary,
                                           (vocabulary_size+1)*sizeof(char*));
            temp_vocabulary = *phonemes_vocabulary;
            i = vocabulary_size;
            temp_vocabulary[i] = malloc((buffer_size+1) * sizeof(char));
            memset(temp_vocabulary[i], 0, (buffer_size+1) * sizeof(char));
            strcpy(temp_vocabulary[i], buffer);
            vocabulary_size++;
        }
    }
    fclose(vocabulary_file);

    return vocabulary_size;
}

int calculate_confusion_penalties_matrix(
        char *confusion_matrix_name, int phonemes_number,
        float confusion_penalties_matrix[])
{
    FILE *matrix_file = NULL;
    int *confusion_matrix = NULL;
    char buffer[BUFFER_SIZE];
    char *value_str;
    int i, j, index, buffer_size, value = 0, is_ok = 1;
    float sum_value = 0.0;

    if ((confusion_matrix_name == NULL) || (phonemes_number <= 0)
            || (confusion_penalties_matrix == NULL))
    {
        return 0;
    }

    matrix_file = fopen(confusion_matrix_name, "r");
    if (matrix_file == NULL)
    {
        return 0;
    }
    i = 0;
    confusion_matrix = malloc((phonemes_number+1) * (phonemes_number+1)
                              * sizeof(int));
    memset(confusion_matrix, 0, (phonemes_number+1) * (phonemes_number+1)
           * sizeof(int));
    while (!feof(matrix_file))
    {
        buffer_size = read_string(matrix_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        if (i > phonemes_number)
        {
            is_ok = 0;
            break;
        }
        j = 0;
        value_str = strtok(buffer, " \t");
        if (value_str == NULL)
        {
            is_ok = 0;
            break;
        }
        if (sscanf(value_str, "%d", &value) != 1)
        {
            is_ok = 0;
            break;
        }
        if (value < 0)
        {
            is_ok = 0;
            break;
        }
        confusion_matrix[i*(phonemes_number+1)+j] = value;
        j++;
        value_str = strtok(NULL, " \t");
        while (value_str != NULL)
        {
            if (j > phonemes_number)
            {
                is_ok = 0;
                break;
            }
            if (sscanf(value_str, "%d", &value) != 1)
            {
                is_ok = 0;
                break;
            }
            if (value < 0)
            {
                is_ok = 0;
                break;
            }
            confusion_matrix[i*(phonemes_number+1)+j] = value;
            j++;
            value_str = strtok(NULL, " \t");
        }
        if (is_ok)
        {
            if (j != (phonemes_number+1))
            {
                is_ok = 0;
            }
        }
        if (!is_ok)
        {
            break;
        }
        i++;
    }
    if (is_ok)
    {
        if (i != (phonemes_number+1))
        {
            is_ok = 0;
        }
    }
    fclose(matrix_file);
    if (!is_ok)
    {
        free(confusion_matrix);
        return 0;
    }
    for (i = 0; i < phonemes_number; i++)
    {
        sum_value = 0;
        for (j = 0; j < phonemes_number; j++)
        {
            sum_value += confusion_matrix[i*(phonemes_number+1)+j];
        }
        if (sum_value > 0)
        {
            for (j = 0; j < phonemes_number; j++)
            {
                confusion_penalties_matrix[i*phonemes_number+j]
                        = confusion_matrix[i*(phonemes_number+1)+j]/sum_value;
            }
        }
        else
        {
            is_ok = 0;
            break;
        }
    }
    free(confusion_matrix);
    if (!is_ok)
    {
        return 0;
    }
    for (i = 0; i < phonemes_number; i++)
    {
        for (j = 0; j < phonemes_number; j++)
        {
            index = i * phonemes_number + j;
            if (confusion_penalties_matrix[index] > 0.0)
            {
                confusion_penalties_matrix[index]
                        = log10(confusion_penalties_matrix[index]);
            }
            else
            {
                confusion_penalties_matrix[index] = -FLT_MAX;
            }
        }
    }
    return 1;
}

int load_words_vocabulary(char *file_name, char ***words_vocabulary)
{
    int i, n, buffer_size = 0, vocabulary_size = 0;
    int is_ok = 1;
    char buffer[BUFFER_SIZE];
    char *word_name = NULL, *word_transcription = NULL;
    char **temp_vocabulary;
    FILE *vocabulary_file = NULL;

    if (words_vocabulary == NULL)
    {
        return 0;
    }
    *words_vocabulary = NULL;

    vocabulary_file = fopen(file_name, "r");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    while (!feof(vocabulary_file))
    {
        buffer_size = read_string(vocabulary_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        if (!select_word_and_transcription(buffer, &word_name,
                                           &word_transcription))
        {
            is_ok = 0;
            break;
        }
        if (find_in_unsorted_vocabulary(*words_vocabulary, vocabulary_size,
                                        word_name) < 0)
        {
            *words_vocabulary = realloc(*words_vocabulary,
                                        (vocabulary_size+1) * sizeof(char*));
            temp_vocabulary = *words_vocabulary;
            i = vocabulary_size;
            n = strlen(word_name);
            temp_vocabulary[i] = malloc((n+1) * sizeof(char));
            memset(temp_vocabulary[i], 0, (n+1) * sizeof(char));
            strcpy(temp_vocabulary[i], word_name);
            vocabulary_size++;
        }
    }
    fclose(vocabulary_file);
    if (!is_ok)
    {
        free_string_array(words_vocabulary, vocabulary_size);
        vocabulary_size = 0;
    }

    return vocabulary_size;
}

int load_language_model(char *file_name,  int words_number,
                        TLanguageModel *language_model)
{
    int i, j, n = 0, is_ok = 1;
    FILE *h_file = NULL;

    if (language_model == NULL)
    {
        return 0;
    }
    language_model->unigrams_number = 0;
    language_model->unigrams_probabilities = NULL;
    language_model->bigrams = NULL;

    h_file = fopen(file_name, "rb");
    if (h_file == NULL)
    {
        return 0;
    }

    if (fread(&n, sizeof(int), 1, h_file) != 1)
    {
        fclose(h_file);
        return 0;
    }
    if (n != words_number)
    {
        fclose(h_file);
        return 0;
    }
    language_model->unigrams_probabilities = malloc(n * sizeof(float));
    language_model->unigrams_number = n;
    language_model->bigrams = malloc(n * sizeof(TWordBigram));
    for (i = 0; i < n; i++)
    {
        language_model->bigrams[i].begins_number = 0;
        language_model->bigrams[i].begins = NULL;
    }
    if (fread(language_model->unigrams_probabilities, sizeof(float), n, h_file)
            != n)
    {
        free_language_model(language_model);
        fclose(h_file);
        return 0;
    }
    for (i = 0; i < n; i++)
    {
        if ((language_model->unigrams_probabilities[i] < 0.0)
                || (language_model->unigrams_probabilities[i] > 1.0))
        {
            is_ok = 0;
            break;
        }
    }
    if (!is_ok)
    {
        free_language_model(language_model);
        fclose(h_file);
        return 0;
    }

    for (i = 0; i < language_model->unigrams_number; i++)
    {
        if (fread(&n, sizeof(int), 1, h_file) != 1)
        {
            fclose(h_file);
            return 0;
        }
        if (n < 0)
        {
            is_ok = 0;
            break;
        }
        if (n > 0)
        {
            language_model->bigrams[i].begins_number = n;
            language_model->bigrams[i].begins
                    = malloc(n * sizeof(TWordBigramBegin));
            if (fread(language_model->bigrams[i].begins,
                      sizeof(TWordBigramBegin), n, h_file) != n)
            {
                is_ok = 0;
                break;
            }
            for (j = 0; j < n; j++)
            {
                if ((language_model->bigrams[i].begins[j].word_i < 0)
                        || (language_model->bigrams[i].begins[j].word_i
                            >= language_model->unigrams_number))
                {
                    is_ok = 0;
                    break;
                }
                if ((language_model->bigrams[i].begins[j].probability < 0.0)
                        ||(language_model->bigrams[i].begins[j].probability
                           > 1.0))
                {
                    is_ok = 0;
                    break;
                }
            }
            if (!is_ok)
            {
                break;
            }
            qsort(language_model->bigrams[i].begins,
                  language_model->bigrams[i].begins_number,
                  sizeof(TWordBigramBegin), compare_begins_of_bigrams);
        }
    }
    if (!is_ok)
    {
        free_language_model(language_model);
        return 0;
    }
    return 1;
}

int save_language_model(char *file_name, TLanguageModel language_model)
{
    int i, n, is_ok = 1;
    FILE *h_file = NULL;

    if ((file_name == NULL) || (language_model.unigrams_number <= 0)
            || (language_model.bigrams == NULL)
            || (language_model.unigrams_probabilities == NULL))
    {
        return 0;
    }

    h_file = fopen(file_name, "wb");
    if (h_file == NULL)
    {
        return 0;
    }

    n = language_model.unigrams_number;
    if (fwrite(&n, sizeof(int), 1, h_file) != 1)
    {
        fclose(h_file);
        return 0;
    }
    if (fwrite(language_model.unigrams_probabilities, sizeof(float), n, h_file)
            != n)
    {
        fclose(h_file);
        return 0;
    }

    for (i = 0; i < language_model.unigrams_number; i++)
    {
        n = language_model.bigrams[i].begins_number;
        if (fwrite(&n, sizeof(int), 1, h_file) != 1)
        {
            is_ok = 0;
            break;
        }
        if (n > 0)
        {
            if (fwrite(language_model.bigrams[i].begins,
                       sizeof(TWordBigramBegin), n, h_file) != n)
            {
                is_ok = 0;
                break;
            }
        }
    }

    fclose(h_file);
    return is_ok;
}

int calculate_language_model(TMLFFilePart *words_mlf_data, int files_number,
                             int words_number, float eps,
                             TLanguageModel *language_model)
{
    int i, j, word_i, bigram_i, is_ok = 1, total_words_count = 0;
    int first_i, second_i, nwords, nbigrams;
    int *words_frequencies = NULL;

    if ((words_mlf_data == NULL) || (files_number <= 0) || (words_number <= 0)
            || (eps < 0.0) || (eps >= 1.0) || (language_model == NULL))
    {
        return 0;
    }

    language_model->unigrams_number = words_number;
    language_model->unigrams_probabilities = malloc(words_number
                                                    * sizeof(float));
    words_frequencies = malloc(words_number * sizeof(int));
    memset(words_frequencies, 0, words_number * sizeof(int));
    language_model->bigrams = malloc(words_number * sizeof(TWordBigram));

    for (i = 0; i < words_number; i++)
    {
        language_model->unigrams_probabilities[i] = 0.0;
        language_model->bigrams[i].begins_number = 0;
        language_model->bigrams[i].begins = NULL;
    }

    for (i = 0; i < files_number; i++)
    {
        for (j = 0; j < words_mlf_data[i].transcription_size; j++)
        {
            word_i = words_mlf_data[i].transcription[j].node_data;
            if ((word_i >= words_number) || (word_i < 0))
            {
                is_ok = 0;
                break;
            }
            words_frequencies[word_i]++;
            total_words_count++;
        }
    }
    if (!is_ok)
    {
        free_language_model(language_model);
        free(words_frequencies);
        return 0;
    }
    for (i = 0; i < words_number; i++)
    {
        if (words_frequencies[i] < 1)
        {
            words_frequencies[i] = 1;
            total_words_count++;
        }
    }
    for (i = 0; i < words_number; i++)
    {
        language_model->unigrams_probabilities[i]
                = (float)words_frequencies[i] / (float)total_words_count;
    }
    if (!is_ok)
    {
        free_language_model(language_model);
        free(words_frequencies);
        return 0;
    }

    for (i = 0; i < files_number; i++)
    {
        for (j = 1; j < words_mlf_data[i].transcription_size; j++)
        {
            first_i = words_mlf_data[i].transcription[j-1].node_data;
            second_i = words_mlf_data[i].transcription[j].node_data;
            bigram_i = 0;
            nwords = language_model->bigrams[second_i].begins_number;
            while (bigram_i < nwords)
            {
                if (language_model->bigrams[second_i].begins[bigram_i].word_i
                        == first_i)
                {
                    break;
                }
                bigram_i++;
            }
            if (bigram_i < nwords)
            {
                language_model->bigrams[second_i].begins[
                        bigram_i].probability += 1.0;
            }
            else
            {
                language_model->bigrams[second_i].begins_number++;
                if (nwords > 0)
                {
                    language_model->bigrams[second_i].begins = realloc(
                                language_model->bigrams[second_i].begins,
                                sizeof(TWordBigramBegin) * (nwords + 1));
                }
                else
                {
                    language_model->bigrams[second_i].begins = malloc(
                                sizeof(TWordBigramBegin) * (nwords + 1));
                }
                language_model->bigrams[second_i].begins[nwords].word_i
                        = first_i;
                language_model->bigrams[second_i].begins[nwords].probability
                        = 1.0;
            }
        }
    }

    nbigrams = 0;
    for (i = 0; i < language_model->unigrams_number; i++)
    {
        j = 0;
        while (j < language_model->bigrams[i].begins_number)
        {
            first_i = language_model->bigrams[i].begins[j].word_i;
            language_model->bigrams[i].begins[j].probability
                    /= (float)words_frequencies[first_i];
            if (language_model->bigrams[i].begins[j].probability >= eps)
            {
                j++;
            }
            else
            {
                nwords = language_model->bigrams[i].begins_number;
                memmove(language_model->bigrams[i].begins+j,
                        language_model->bigrams[i].begins+j+1,
                        sizeof(TWordBigramBegin) * (nwords - j - 1));
                language_model->bigrams[i].begins_number--;
            }
        }
        nwords = language_model->bigrams[i].begins_number;
        if (nwords > 1)
        {
            qsort(language_model->bigrams[i].begins, nwords,
                  sizeof(TWordBigramBegin), compare_begins_of_bigrams);
        }
        nbigrams += nwords;
    }

    free(words_frequencies);
    if (nbigrams <= 0)
    {
        free_language_model(language_model);
        return 0;
    }

    return 1;
}

PWordsTreeNode create_words_vocabulary_tree(
        char *file_name, char **phonemes_vocabulary, int phonemes_number,
        char **words_vocabulary, int words_number)
{
    PWordsTreeNode root = NULL;
    int buffer_size = 0;
    int word_index, phonemes_sequence_size = 0;
    int is_ok = 1;
    char buffer[BUFFER_SIZE];
    int phonemes_sequence[BUFFER_SIZE];
    char *word_name = NULL, *word_transcription = NULL;
    FILE *vocabulary_file = NULL;

    if ((file_name==NULL) || (words_vocabulary == NULL) || (words_number <= 0)
            || (phonemes_vocabulary == NULL) || (phonemes_number <= 0))
    {
        return NULL;
    }

    vocabulary_file = fopen(file_name, "r");
    if (vocabulary_file == NULL)
    {
        return NULL;
    }
    root = malloc(sizeof(TWordsTreeNode));
    root->node_data = -1;
    root->node_type = INIT_NODE;
    root->next_nodes = NULL;
    root->number_of_next_nodes = 0;
    while (!feof(vocabulary_file))
    {
        buffer_size = read_string(vocabulary_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        if (!select_word_and_transcription(buffer, &word_name,
                                           &word_transcription))
        {
            is_ok = 0;
            break;
        }
        word_index = find_in_unsorted_vocabulary(
                    words_vocabulary, words_number, word_name);
        if (word_index < 0)
        {
            is_ok = 0;
            break;
        }
        phonemes_sequence_size = parse_transcription_str(
                    word_transcription, phonemes_vocabulary, phonemes_number,
                    phonemes_sequence);
        if (phonemes_sequence_size <= 0)
        {
            is_ok = 0;
            break;
        }
        if (!add_word_to_words_tree(word_index, phonemes_sequence,
                                    phonemes_sequence_size, root))
        {
            is_ok = 0;
            break;
        }
    }
    fclose(vocabulary_file);
    if (is_ok)
    {
        if (root->number_of_next_nodes <= 0)
        {
            free_words_tree(&root);
        }
    }
    else
    {
        free_words_tree(&root);
    }

    return root;
}

int word_exists_in_words_tree(int word_index, PWordsTreeNode words_tree_root)
{
    int i, res = 0;
    PWordsTreeNode cur;
    if ((word_index < 0) || (words_tree_root == NULL))
    {
        return 0;
    }
    cur = words_tree_root->next_nodes;
    for (i = 0; i < words_tree_root->number_of_next_nodes; i++)
    {
        if (cur->node_type == WORD_NODE)
        {
            if (cur->node_data == word_index)
            {
                res = 1;
                break;
            }
        }
        else if (cur->node_type == PHONEME_NODE)
        {
            if (word_exists_in_words_tree(word_index, cur))
            {
                res = 1;
                break;
            }
        }
        cur++;
    }
    return res;
}

int create_linear_words_lexicon(
        char *file_name, char **phonemes_vocabulary, int phonemes_number,
        char **words_vocabulary, int words_number,
        TLinearWordsLexicon **linear_words_lexicon)
{
    int words_lexicon_size = 0;
    int buffer_size = 0;
    int word_index, phonemes_sequence_size = 0, n;
    int is_ok = 1;
    char buffer[BUFFER_SIZE];
    int phonemes_sequence[BUFFER_SIZE];
    char *word_name = NULL, *word_transcription = NULL;
    FILE *vocabulary_file = NULL;
    TLinearWordsLexicon *tmp_array;

    if ((file_name==NULL) || (words_vocabulary == NULL) || (words_number <= 0)
            || (phonemes_vocabulary == NULL) || (phonemes_number <= 0)
            || (linear_words_lexicon == NULL))
    {
        return 0;
    }

    vocabulary_file = fopen(file_name, "r");
    if (vocabulary_file == NULL)
    {
        return 0;
    }
    while (!feof(vocabulary_file))
    {
        buffer_size = read_string(vocabulary_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        if (!select_word_and_transcription(buffer, &word_name,
                                           &word_transcription))
        {
            is_ok = 0;
            break;
        }
        word_index = find_in_unsorted_vocabulary(
                    words_vocabulary, words_number, word_name);
        if (word_index < 0)
        {
            is_ok = 0;
            break;
        }
        phonemes_sequence_size = parse_transcription_str(
                    word_transcription, phonemes_vocabulary, phonemes_number,
                    phonemes_sequence);
        if (phonemes_sequence_size <= 0)
        {
            is_ok = 0;
            break;
        }
        words_lexicon_size++;
        *linear_words_lexicon = realloc(
                    *linear_words_lexicon,
                    words_lexicon_size * sizeof(TLinearWordsLexicon));
        tmp_array = *linear_words_lexicon;
        tmp_array[words_lexicon_size-1].word_index = word_index;
        n = phonemes_sequence_size + 1;
        tmp_array[words_lexicon_size-1].phonemes_number = n;
        tmp_array[words_lexicon_size-1].phonemes_indexes
                = malloc(sizeof(int) * n);
        memmove(tmp_array[words_lexicon_size-1].phonemes_indexes,
                phonemes_sequence, phonemes_sequence_size * sizeof(int));
        tmp_array[words_lexicon_size-1].phonemes_indexes[n-1] = 0;
    }
    fclose(vocabulary_file);
    if (!is_ok)
    {
        free_linear_words_lexicon(linear_words_lexicon, words_lexicon_size);
        words_lexicon_size = 0;
    }
    qsort(*linear_words_lexicon,words_lexicon_size,sizeof(TLinearWordsLexicon),
          compare_items_of_words_lexicon);

    return words_lexicon_size;
}

void free_MLF(TMLFFilePart **mlf_data, int number_of_MLF_parts)
{
    int i;
    TMLFFilePart *cur_MLF_part;

    if ((mlf_data == NULL) || (number_of_MLF_parts <= 0))
    {
        return;
    }
    if (*mlf_data == NULL)
    {
        return;
    }

    cur_MLF_part = *mlf_data;
    for (i = 0; i < number_of_MLF_parts; i++)
    {
        if (cur_MLF_part->name != NULL)
        {
            free(cur_MLF_part->name);
            cur_MLF_part->name = NULL;
        }
        if (cur_MLF_part->transcription != NULL)
        {
            free(cur_MLF_part->transcription);
            cur_MLF_part->transcription = NULL;
        }
        cur_MLF_part->transcription_size = 0;
        cur_MLF_part++;
    }
    free(*mlf_data);
    *mlf_data = NULL;
}

void free_words_tree(PWordsTreeNode* root_node)
{
    PWordsTreeNode deleted_node = *root_node, next_node = NULL;
    int i;

    if (root_node == NULL)
    {
        return;
    }
    if (deleted_node == NULL)
    {
        return;
    }

    if ((deleted_node->number_of_next_nodes > 0) && (deleted_node->next_nodes != NULL))
    {
        for (i = 0; i < deleted_node->number_of_next_nodes; i++)
        {
            next_node = deleted_node->next_nodes + i;
            free_words_tree_node(next_node);
        }
        free(deleted_node->next_nodes);
        deleted_node->next_nodes = NULL;
    }
    free(deleted_node);
    *root_node = NULL;
}

void free_linear_words_lexicon(TLinearWordsLexicon **words_lexicon,
                               int lexicon_size)
{
    int i;
    TLinearWordsLexicon *tmp;

    if ((*words_lexicon == NULL) || (lexicon_size <= 0))
    {
        return;
    }
    tmp = *words_lexicon;
    if (tmp == NULL)
    {
        return;
    }

    for (i = 0; i < lexicon_size; i++)
    {
        if (tmp[i].phonemes_indexes != NULL)
        {
            free(tmp[i].phonemes_indexes);
        }
    }
    free(tmp);
    *words_lexicon = NULL;
}

void free_string_array(char ***string_array, int array_size)
{
    char **tmp;
    register int i;

    if ((string_array == NULL) || (array_size <= 0))
    {
        return;
    }
    if (*string_array == NULL)
    {
        return;
    }
    tmp = *string_array;

    for (i = 0; i < array_size; i++)
    {
        if (tmp[i] != NULL)
        {
            free(tmp[i]);
            tmp[i] = NULL;
        }
    }
    free(tmp);
    *string_array = NULL;
}

void free_language_model(TLanguageModel *language_model)
{
    int i;
    if (language_model == NULL)
    {
        return;
    }
    if (language_model->unigrams_probabilities != NULL)
    {
        free(language_model->unigrams_probabilities);
        language_model->unigrams_probabilities = NULL;
    }
    if (language_model->bigrams != NULL)
    {
        for (i = 0; i < language_model->unigrams_number; i++)
        {
            if (language_model->bigrams[i].begins != NULL)
            {
                free(language_model->bigrams[i].begins);
                language_model->bigrams[i].begins = NULL;
            }
        }
        free(language_model->bigrams);
        language_model->bigrams = NULL;
    }
    language_model->unigrams_number = 0;
}

float get_bigram_probability(TLanguageModel language_model, int start_word_ind,
                             int end_word_ind)
{
    int first, last, middle, cmp_res;
    TWordBigram *item = NULL;

    if ((language_model.unigrams_probabilities == NULL)
            || (language_model.unigrams_number <= 0)
            || (language_model.bigrams == NULL))
    {
        return 0.0;
    }
    if ((start_word_ind < 0)
            || (start_word_ind >= language_model.unigrams_number))
    {
        return 0.0;
    }
    if ((end_word_ind < 0) || (end_word_ind >= language_model.unigrams_number))
    {
        return 0.0;
    }

    item = language_model.bigrams + end_word_ind;

    if (item->begins_number <= 0)
    {
        return 0.0;
    }

    first = 0; last = item->begins_number;

    cmp_res = item->begins[0].word_i - start_word_ind;
    if (cmp_res > 0)
    {
        return 0.0;
    }
    if (cmp_res == 0)
    {
        return item->begins[0].probability;
    }

    cmp_res = item->begins[last-1].word_i - start_word_ind;
    if (cmp_res < 0)
    {
        return 0.0;
    }
    if (cmp_res == 0)
    {
        return item->begins[last-1].probability;
    }

    while (first < last)
    {
        middle = first + (last - first) / 2;

        cmp_res = start_word_ind - item->begins[middle].word_i;

        if (cmp_res <= 0)
        {
            last = middle;
            if (cmp_res == 0)
            {
                break;
            }
        }
        else
        {
            first = middle + 1;
        }
    }

    cmp_res = item->begins[last].word_i - start_word_ind;
    if (cmp_res == 0)
    {
        return item->begins[last].probability;
    }
    return 0.0;
}

int recognize_words(
        TMLFFilePart *source_phonemes_MLF, int number_of_MLF_files,
        int phonemes_vocabulary_size, float confusion_penalties_matrix[],
        TLinearWordsLexicon words_lexicon[], int words_lexicon_size,
        float pruning_coeff, TLanguageModel language_model, float lambda,
        TMLFFilePart **result_words_MLF)
{
    int is_ok = 1;
    int i, j, n;
    int phonemes_sequence_length, phonemes_sequence_capacity;
    int words_sequence_length, max_words_sequence_length = 0;
    int *words_sequence = NULL;
    TMLFFilePart *cur_src, *cur_result;
    TViterbiMatrix data;
    TTracebackArray traceback_array = NULL;
    int *src_phonemes_sequence = NULL;
    float *src_phonemes_weights = NULL;

    if ((source_phonemes_MLF == NULL) || (number_of_MLF_files <= 0)
            || (phonemes_vocabulary_size <= 0)
            || (confusion_penalties_matrix == NULL)
            || (words_lexicon_size <= 0) || (words_lexicon == NULL)
            || (pruning_coeff < 0.0) || (pruning_coeff > 1.0)
            || (language_model.unigrams_number <= 0)
            || (language_model.unigrams_probabilities == NULL)
            || (language_model.bigrams == NULL) || (result_words_MLF == NULL)
            || (lambda < 0.0) || (lambda > 1.0))
    {
        return 0;
    }

    *result_words_MLF = malloc(sizeof(TMLFFilePart) * number_of_MLF_files);
    cur_src = source_phonemes_MLF;
    cur_result = *result_words_MLF;
    for (i = 0; i < number_of_MLF_files; i++)
    {
        cur_result->transcription_size = 0;
        cur_result->transcription = NULL;
        n = strlen(cur_src->name);
        cur_result->name = malloc((n+5)*sizeof(char));
        memset(cur_result->name, 0, (n+5)*sizeof(char));
        strcpy(cur_result->name, cur_src->name);
        set_new_file_extension(cur_result->name, "rec");
        cur_result++;
        cur_src++;
    }

    data.cells = NULL;
    data.words_sizes = NULL;
    data.times_number = 0;
    data.words_number = 0;
    create_viterbi_matrix(&data, words_lexicon_size, words_lexicon);

    cur_src = source_phonemes_MLF;
    cur_result = *result_words_MLF;
    max_words_sequence_length = cur_src->transcription_size;
    words_sequence = malloc(max_words_sequence_length * sizeof(int));
    phonemes_sequence_capacity = create_phonemes_sequence_by_transcription(
                cur_src->transcription, cur_src->transcription_size,
                NULL, NULL);
    if (phonemes_sequence_capacity > 0)
    {
        if (phonemes_sequence_capacity > 1)
        {
            traceback_array = malloc(phonemes_sequence_capacity
                                     * sizeof(TTracebackArrayItem));
        }
        src_phonemes_sequence = malloc(
                    phonemes_sequence_capacity * sizeof(int));
        src_phonemes_weights = malloc(
                    phonemes_sequence_capacity * sizeof(float));
        phonemes_sequence_capacity = create_phonemes_sequence_by_transcription(
                    cur_src->transcription, cur_src->transcription_size,
                    src_phonemes_sequence, src_phonemes_weights);

        data.times_number = phonemes_sequence_capacity;
        initialize_values_of_viterbi_matrix(data);
        if (!calculate_viterbi_matrix(
                    data, traceback_array,
                    src_phonemes_sequence, src_phonemes_weights,
                    phonemes_vocabulary_size, confusion_penalties_matrix,
                    words_lexicon, pruning_coeff, language_model, lambda))
        {
            is_ok = 0;
        }
        else
        {
            words_sequence_length = get_words_sequence_by_traceback_array(
                        traceback_array, phonemes_sequence_capacity,
                        words_sequence);
            if (words_sequence_length > 0)
            {
                cur_result->transcription_size = words_sequence_length;
                cur_result->transcription = malloc(words_sequence_length
                                                   * sizeof(TTranscriptionNode));
                for (j = 0; j < words_sequence_length; j++)
                {
                    cur_result->transcription[j].node_data = words_sequence[j];
                    cur_result->transcription[j].start_time = 0;
                    cur_result->transcription[j].end_time = 0;
                    cur_result->transcription[j].probability = 1.0;
                }
            }
        }
    }
    else if (phonemes_sequence_capacity < 0)
    {
        is_ok = 0;
    }
    cur_src++;
    cur_result++;
    if (!is_ok)
    {
        if (traceback_array != NULL)
        {
            free(traceback_array);
            traceback_array = NULL;
        }
        delete_viterbi_matrix(&data);
        free(words_sequence);
        free(src_phonemes_sequence);
        free(src_phonemes_weights);
        free_MLF(result_words_MLF, number_of_MLF_files);
        return 0;
    }

    for (i = 1; i < number_of_MLF_files; i++)
    {
        if (cur_src->transcription_size > max_words_sequence_length)
        {
            max_words_sequence_length = cur_src->transcription_size;
            words_sequence = realloc(words_sequence, max_words_sequence_length
                                     * sizeof(int));
        }
        phonemes_sequence_length = create_phonemes_sequence_by_transcription(
                    cur_src->transcription, cur_src->transcription_size,
                    NULL, NULL);
        if (phonemes_sequence_length > phonemes_sequence_capacity)
        {
            phonemes_sequence_capacity = phonemes_sequence_length;
            if (phonemes_sequence_capacity > 1)
            {
                if (traceback_array == NULL)
                {
                    traceback_array = malloc(phonemes_sequence_capacity
                                             * sizeof(TTracebackArrayItem));
                }
                else
                {
                    traceback_array = realloc(traceback_array,
                                              phonemes_sequence_capacity
                                              * sizeof(TTracebackArrayItem));
                }
            }
            src_phonemes_sequence = realloc(
                        src_phonemes_sequence, phonemes_sequence_capacity
                        * sizeof(int));
            src_phonemes_weights = realloc(
                        src_phonemes_weights, phonemes_sequence_capacity
                        * sizeof(float));
        }
        if (phonemes_sequence_length > 0)
        {
            phonemes_sequence_length=create_phonemes_sequence_by_transcription(
                        cur_src->transcription, cur_src->transcription_size,
                        src_phonemes_sequence, src_phonemes_weights);
            data.times_number = phonemes_sequence_length;
            initialize_values_of_viterbi_matrix(data);
            if (!calculate_viterbi_matrix(
                        data, traceback_array,
                        src_phonemes_sequence, src_phonemes_weights,
                        phonemes_vocabulary_size, confusion_penalties_matrix,
                        words_lexicon, pruning_coeff, language_model, lambda))
            {
                is_ok = 0;
                break;
            }
            words_sequence_length = get_words_sequence_by_traceback_array(
                        traceback_array, phonemes_sequence_length,
                        words_sequence);
            if (words_sequence_length > 0)
            {
                cur_result->transcription_size = words_sequence_length;
                cur_result->transcription = malloc(words_sequence_length
                                                   * sizeof(TTranscriptionNode));
                for (j = 0; j < words_sequence_length; j++)
                {
                    cur_result->transcription[j].node_data = words_sequence[j];
                    cur_result->transcription[j].start_time = 0;
                    cur_result->transcription[j].end_time = 0;
                    cur_result->transcription[j].probability = 1.0;
                }
            }
        }
        else if (phonemes_sequence_length < 0)
        {
            is_ok = 0;
            break;
        }
        cur_src++;
        cur_result++;
    }
    if (traceback_array != NULL)
    {
        free(traceback_array);
        traceback_array = NULL;
    }
    delete_viterbi_matrix(&data);
    free(words_sequence);
    free(src_phonemes_sequence);
    free(src_phonemes_weights);
    if (!is_ok)
    {
        free_MLF(result_words_MLF, number_of_MLF_files);
    }

    return is_ok;
}

float estimate_error_rate(
        TMLFFilePart recognized_MLF[], TMLFFilePart correct_MLF[],
        int files_number, int *insertions, int *deletions, int *substitutions)
{
    int i, j, k, n = 0, m = 0, is_ok = 1;
    int number_of_insertions = 0;
    int number_of_deletions = 0;
    int number_of_substitutions = 0;
    int correct_sequences_sizes_sum = 0;
    int **R = NULL, **B = NULL;
    float result;

    if ((recognized_MLF == NULL) || (correct_MLF == NULL)
            || (files_number <= 0))
    {
        return -1.0;
    }

    for (k = 0; k < files_number; k++)
    {
        if ((correct_MLF[k].transcription_size <= 0)
                || (correct_MLF[k].transcription == NULL))
        {
            is_ok = 0;
            break;
        }
        correct_sequences_sizes_sum += correct_MLF[k].transcription_size;
        if (recognized_MLF[k].transcription_size <= 0)
        {
            number_of_deletions += correct_MLF[k].transcription_size;
        }
        else
        {
            if (recognized_MLF[k].transcription == NULL)
            {
                is_ok = 0;
                break;
            }
            n = correct_MLF[k].transcription_size;
            m = recognized_MLF[k].transcription_size;
            R = malloc((n+1) * sizeof(int*));
            B = malloc((n+1) * sizeof(int*));
            memset(R, 0, (n+1) * sizeof(int*));
            memset(B, 0, (n+1) * sizeof(int*));
            for (i = 0; i <= n; i++)
            {
                R[i] = malloc((m+1) * sizeof(int));
                B[i] = malloc((m+1) * sizeof(int));
            }

            R[0][0] = 0;
            B[0][0] = 0;

            j = 0;
            for (i = 1; i <= n; i++)
            {
                R[i][j] = i;
                B[i][j] = 1;
            }
            i = 0;
            for (j = 1; j <= m; j++)
            {
                R[i][j] = j;
                B[i][j] = 2;
            }
            for (i = 1; i <= n; i++)
            {
                for (j = 1; j <= m; j++)
                {
                    if (correct_MLF[k].transcription[i-1].node_data
                            == recognized_MLF[k].transcription[j-1].node_data)
                    {
                        R[i][j] = R[i-1][j-1];
                        B[i][j] = 3;
                    }
                    else
                    {
                        if ((R[i-1][j] + 1) < (R[i-1][j-1] + 1))
                        {
                            if ((R[i-1][j] + 1) < (R[i][j-1] + 1))
                            {
                                R[i][j] = R[i-1][j] + 1;
                                B[i][j] = 1;
                            }
                            else
                            {
                                R[i][j] = R[i][j-1] + 1;
                                B[i][j] = 2;
                            }
                        }
                        else
                        {
                            if ((R[i-1][j-1] + 1) < (R[i][j-1] + 1))
                            {
                                R[i][j] = R[i-1][j-1] + 1;
                                B[i][j] = 4;
                            }
                            else
                            {
                                R[i][j] = R[i][j-1] + 1;
                                B[i][j] = 2;
                            }
                        }
                    }
                }
            }
            i = n;
            j = m;
            while ((i > 0) && (j > 0))
            {
                switch (B[i][j])
                {
                case 1:
                    number_of_deletions++;
                    i--;
                    break;
                case 2:
                    number_of_insertions++;
                    j--;
                    break;
                case 4:
                    number_of_substitutions++;
                    i--; j--;
                    break;
                default:
                    i--; j--;
                }
            }
            for (i = 0; i <= n; i++)
            {
                free(R[i]);
                free(B[i]);
            }
            free(R);
            free(B);
        }
    }
    if (!is_ok)
    {
        return -1;
    }
    if (insertions != NULL)
    {
        *insertions = number_of_insertions;
    }
    if (deletions != NULL)
    {
        *deletions = number_of_deletions;
    }
    if (substitutions != NULL)
    {
        *substitutions = number_of_substitutions;
    }
    result = 100.0 * (float)(number_of_substitutions + number_of_insertions
                             + number_of_deletions)
            / (float)correct_sequences_sizes_sum;
    if (result > 100.0)
    {
        result = 100.0;
    }
    return result;
}
