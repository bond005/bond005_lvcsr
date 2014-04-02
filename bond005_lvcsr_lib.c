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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bond005_lvcsr_lib.h"

/* Structure for representation of backtrack pointer. The backtrack pointer is
 * stack of word-time pairs which defines best words sequence (see X.Huang,
 * Spoken Language Processing, pp.618-620). */
typedef struct _TBackTrackPointer {
    int word; // word index in the word lexicon
    int time; // time moment
    struct _TBackTrackPointer *next;
} TBackTrackPointer;
typedef TBackTrackPointer* PBackTrackPointer;

/* Structure for representation of one cell of the matrix which is used in the
 * Viterbi Beam Search algorithm (see X.Huang, Spoken Language Processing,
 * pp.618-620). */
typedef struct _TViterbiMatrixCell {
    float cost;           // total cost
    PBackTrackPointer btp;// stack of backtrack pointers
} TViterbiMatrixCell;

/* Structure for representation of whole matrix which is used in the Viterbi
 * Beam Search algorithm (see X.Huang, Spoken Language Processing,
 * pp.618-620). */
typedef struct _TViterbiMatrix {
    int times_number;           /* times number (i.e. length of recognized
                                   phones sequence) */
    int words_number;           /* words number (i.e. vocabulary size) */
    int *words_sizes;           /* lengths of words (i.e. numbers of states in
                                   each word) */
    TViterbiMatrixCell ***cells;/* 3-dimension matrix for the Viterbi Beam
                                   Search algorithm */
} TViterbiMatrix;

/* This function adds new word-time pair into head of the backtrack pointers
 * stack. */
static void add_to_backtrack_pointer(PBackTrackPointer* bt, int word, int time)
{
    PBackTrackPointer new_bt;
    if (bt == NULL)
    {
        return;
    }
    new_bt = malloc(sizeof(TBackTrackPointer));
    new_bt->word = word;
    new_bt->time = time;
    new_bt->next = *bt;
    *bt = new_bt;
}

/* This function deletes all nodes of the specified stack of backtrack
 * pointers. */
static void delete_backtrack_pointer(PBackTrackPointer* bt)
{
    PBackTrackPointer deleted_bt;
    if (bt == NULL)
    {
        return;
    }
    deleted_bt = *bt;
    while (deleted_bt != NULL)
    {
        *bt = deleted_bt->next;
        free(deleted_bt);
        deleted_bt = *bt;
    }
}

/* This function copies one backtrack pointers stack into another one. */
static void copy_backtrack_pointer(PBackTrackPointer* dst_bt,
                                   PBackTrackPointer src_bt)
{
    PBackTrackPointer new_bt, last_bt;
    if (dst_bt == NULL)
    {
        return;
    }
    delete_backtrack_pointer(dst_bt);
    if (src_bt == NULL)
    {
        return;
    }
    new_bt = malloc(sizeof(TBackTrackPointer));
    new_bt->word = src_bt->word;
    new_bt->time = src_bt->time;
    new_bt->next = NULL;
    *dst_bt = new_bt;
    last_bt = new_bt;
    src_bt = src_bt->next;
    while (src_bt != NULL)
    {
        new_bt = malloc(sizeof(TBackTrackPointer));
        new_bt->word = src_bt->word;
        new_bt->time = src_bt->time;
        new_bt->next = NULL;
        last_bt->next = new_bt;
        last_bt = new_bt;
        src_bt = src_bt->next;
    }
}

static void create_viterbi_matrix(
        TViterbiMatrix* data, int times_number, int words_number,
        TLinearWordsLexicon lexicon[])
{
    int t, w, s;
    if ((data == NULL) || (times_number <= 0) || (words_number <= 0)
            || (lexicon == NULL))
    {
        return;
    }
    data->times_number = times_number;
    data->words_number = words_number;
    data->words_sizes = malloc(sizeof(int) * words_number);
    for (w = 0; w < words_number; w++)
    {
        data->words_sizes[w] = lexicon[w].phonemes_number;
    }
    data->cells = malloc(times_number * sizeof(TViterbiMatrixCell**));
    for (t = 0; t < times_number; t++)
    {
        data->cells[t] = malloc(words_number * sizeof(TViterbiMatrixCell*));
        for (w = 0; w < words_number; w++)
        {
            data->cells[t][w] = malloc((data->words_sizes[w]+1)
                                       * sizeof(TViterbiMatrixCell));
            for (s = 0; s <= data->words_sizes[w]; s++)
            {
                data->cells[t][w][s].btp = NULL;
                data->cells[t][w][s].cost = FLT_MAX;
            }
        }
    }
}

static void resize_viterbi_matrix(TViterbiMatrix* data, int new_times_number)
{
    int t, w, s;
    if ((data == NULL) || (new_times_number <= 0))
    {
        return;
    }
    if (data->times_number >= new_times_number)
    {
        return;
    }
    data->cells = realloc(data->cells, new_times_number
                          * sizeof(TViterbiMatrixCell**));
    for (t = data->times_number; t < new_times_number; t++)
    {
        data->cells[t] = malloc(data->words_number
                                * sizeof(TViterbiMatrixCell*));
        for (w = 0; w < data->words_number; w++)
        {
            data->cells[t][w] = malloc((data->words_sizes[w]+1)
                                       * sizeof(TViterbiMatrixCell));
            for (s = 0; s <= data->words_sizes[w]; s++)
            {
                data->cells[t][w][s].btp = NULL;
                data->cells[t][w][s].cost = FLT_MAX;
            }
        }
    }
    data->times_number = new_times_number;
}

static void initialize_values_of_viterbi_matrix(TViterbiMatrix data)
{
    int t, w, s;
    for (t = 0; t < data.times_number; t++)
    {
        for (w = 0; w < data.words_number; w++)
        {
            for (s = 0; s <= data.words_sizes[w]; s++)
            {
                if (data.cells[t][w][s].btp != NULL)
                {
                    delete_backtrack_pointer(&(data.cells[t][w][s].btp));
                }
                data.cells[t][w][s].cost = FLT_MAX;
            }
        }
    }
}

static void delete_viterbi_matrix(TViterbiMatrix* data)
{
    int t, w, s;
    if (data == NULL)
    {
        return;
    }
    if ((data->words_number <= 0) || (data->times_number <= 0)
            || (data->words_sizes == NULL) || (data->cells == NULL))
    {
        return;
    }

    for (t = 0; t < data->times_number; t++)
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
            for (s = 0; s <= data->words_sizes[w]; s++)
            {
                delete_backtrack_pointer(&(data->cells[t][w][s].btp));
            }
            free(data->cells[t][w]);
            data->cells[t][w] = NULL;
        }
        free(data->cells[t]);
        data->cells[t] = NULL;
    }

    free(data->words_sizes);
    data->words_sizes = NULL;
    free(data->cells);
    data->cells = NULL;
    data->words_number = 0;
    data->times_number = 0;
}

static void calculate_viterbi_matrix(
        TViterbiMatrix data, TTranscriptionNode src_phonemes_sequence[],
        int phonemes_vocabulary_size, float phonemes_probabilities[],
        TLinearWordsLexicon words_lexicon[], TLanguageModel language_model)
{
    int t, w, s, S, i, v, v_min;
    float tmp_val1, tmp_val2, tmp_d, bigram_probability;

    t = 0;
    for (w = 0; w < data.words_number; w++)
    {
        i = words_lexicon[w].phonemes_indexes[0] * phonemes_vocabulary_size
                + src_phonemes_sequence[t].node_data;
        if (phonemes_probabilities[i] > 0.0)
        {
            data.cells[t][w][1].cost =
                    -log(src_phonemes_sequence[t].probability)
                    - log(phonemes_probabilities[i]);
        }
        else
        {
            data.cells[t][w][1].cost = FLT_MAX;
        }
    }

    for (t = 1; t < data.times_number; t++)
    {
        for (w = 0; w < data.words_number; w++)
        {
            s = 1;
            tmp_val1 = data.cells[t-1][w][s].cost;
            if (tmp_val1 < (FLT_MAX - FLT_EPSILON))
            {
                tmp_d = -log(src_phonemes_sequence[t].probability);
                i = words_lexicon[w].phonemes_indexes[s-1]
                        * phonemes_vocabulary_size
                        + src_phonemes_sequence[t].node_data;
                if (phonemes_probabilities[i] > 0.0)
                {
                    tmp_d -= log(phonemes_probabilities[i]);
                    tmp_val1 += tmp_d;
                }
                else
                {
                    tmp_val1 = FLT_MAX;
                }
            }
            data.cells[t][w][s].cost = tmp_val1;
            copy_backtrack_pointer(&(data.cells[t][w][s].btp),
                    data.cells[t-1][w][s].btp);

            for (s = 2; s <= data.words_sizes[w]; s++)
            {
                tmp_val1 = data.cells[t-1][w][s].cost;
                if (tmp_val1 < (FLT_MAX - FLT_EPSILON))
                {
                    tmp_d = -log(src_phonemes_sequence[t].probability);
                    i = words_lexicon[w].phonemes_indexes[s-1]
                            * phonemes_vocabulary_size
                            + src_phonemes_sequence[t].node_data;
                    if (phonemes_probabilities[i] > 0.0)
                    {
                        tmp_d -= log(phonemes_probabilities[i]);
                        tmp_val1 += tmp_d;
                    }
                    else
                    {
                        tmp_val1 = FLT_MAX;
                    }
                }
                tmp_val2 = data.cells[t-1][w][s-1].cost;
                if (tmp_val2 < (FLT_MAX - FLT_EPSILON))
                {
                    tmp_d = -log(src_phonemes_sequence[t].probability);
                    i = words_lexicon[w].phonemes_indexes[s-1]
                            * phonemes_vocabulary_size
                            + src_phonemes_sequence[t].node_data;
                    if (phonemes_probabilities[i] > 0.0)
                    {
                        tmp_d -= log(phonemes_probabilities[i]);
                        tmp_val2 += tmp_d;
                    }
                    else
                    {
                        tmp_val2 = FLT_MAX;
                    }
                }
                if (tmp_val1 <= tmp_val2)
                {
                    data.cells[t][w][s].cost = tmp_val1;
                    copy_backtrack_pointer(&(data.cells[t][w][s].btp),
                            data.cells[t-1][w][s].btp);
                }
                else
                {
                    data.cells[t][w][s].cost = tmp_val2;
                    copy_backtrack_pointer(&(data.cells[t][w][s].btp),
                            data.cells[t-1][w][s-1].btp);
                }
            }
        }

        for (w = 0; w < data.words_number; w++)
        {
            v_min = 0;
            S = data.words_sizes[v_min];
            bigram_probability = get_bigram_probability(language_model,
                                                        v_min, w);
            if (bigram_probability > 0.0)
            {
                tmp_val1 = -log(bigram_probability);
                if (data.cells[t][v_min][data.words_sizes[v_min]].cost
                        < (FLT_MAX - FLT_EPSILON))
                {
                    tmp_val1 += data.cells[t][v_min][S].cost;
                }
                else
                {
                    tmp_val1 = FLT_MAX;
                }
            }
            else
            {
                tmp_val1 = FLT_MAX;
            }
            for (v = 1; v < data.words_number; v++)
            {
                S = data.words_sizes[v];
                bigram_probability = get_bigram_probability(language_model,
                                                            v, w);
                if (bigram_probability > 0.0)
                {
                    tmp_val2 = -log(bigram_probability);
                    if (data.cells[t][v][data.words_sizes[v]].cost
                            < (FLT_MAX - FLT_EPSILON))
                    {
                        tmp_val2 += data.cells[t][v][S].cost;
                    }
                    else
                    {
                        tmp_val2 = FLT_MAX;
                    }
                }
                else
                {
                    tmp_val2 = FLT_MAX;
                }
                if (tmp_val2 < tmp_val1)
                {
                    v_min = v;
                    tmp_val1 = tmp_val2;
                }
            }
            data.cells[t][w][0].cost = tmp_val1;
            copy_backtrack_pointer(&(data.cells[t][w][0].btp),
                    data.cells[t][v_min][data.words_sizes[v_min]].btp);
            add_to_backtrack_pointer(&(data.cells[t][w][0].btp), v_min, t);

            if (data.cells[t][w][0].cost < data.cells[t][w][1].cost)
            {
                data.cells[t][w][1].cost = data.cells[t][w][0].cost;
                copy_backtrack_pointer(&(data.cells[t][w][1].btp),
                        data.cells[t][w][0].btp);
            }
        }
    }
}

static int calculate_words_sequence_by_viterbi_matrix(
        TViterbiMatrix data, int recognized_words_sequence[])
{
    int t, w, w_min, i, n = 0;
    PBackTrackPointer cur;

    t = data.times_number - 1;
    w_min = 0;
    for (w = 1; w < data.words_number; w++)
    {
        if (data.cells[t][w][0].cost < data.cells[t][w_min][0].cost)
        {
            w_min = w;
        }
    }

    cur = data.cells[t][w_min][0].btp;
    while (cur != NULL)
    {
        for (i = n; i > 0; i--)
        {
            recognized_words_sequence[i] = recognized_words_sequence[i-1];
        }
        recognized_words_sequence[0] = cur->word;
        n++;
        cur = cur->next;
    }

    return n;
}

static int create_phonemes_sequence_by_transcription(
        TTranscriptionNode transcription[], int transcription_length,
        TTranscriptionNode **phonemes_sequence)
{
    TTranscriptionNode *tmp_sequence;
    int i, j, k, number_of_windows_10ms, phonemes_sequence_length = 0;

    for (i = 0; i < transcription_length; i++)
    {
        number_of_windows_10ms = (transcription[i].end_time
                                  - transcription[i].start_time) / 100000;
        if (number_of_windows_10ms < 1)
        {
            number_of_windows_10ms = 1;
        }
        phonemes_sequence_length += number_of_windows_10ms;
    }

    *phonemes_sequence = malloc(phonemes_sequence_length
                                * sizeof(TTranscriptionNode));
    tmp_sequence = *phonemes_sequence;
    j = 0;
    for (i = 0; i < transcription_length; i++)
    {
        number_of_windows_10ms = (transcription[i].end_time
                                  - transcription[i].start_time) / 100000;
        if (number_of_windows_10ms < 1)
        {
            number_of_windows_10ms = 1;
        }
        for (k = 0; k < number_of_windows_10ms; k++)
        {
            tmp_sequence[j+k].node_data = transcription[i].node_data;
            tmp_sequence[j+k].probability = transcription[i].probability;
            tmp_sequence[j+k].start_time = -1;
            tmp_sequence[j+k].end_time = -1;
        }
        j += number_of_windows_10ms;
    }

    return phonemes_sequence_length;
}

static int compare_bigrams(const void *ptr1, const void *ptr2)
{
    int res;
    TWordBigram *bigram1 = (TWordBigram*)ptr1;
    TWordBigram *bigram2 = (TWordBigram*)ptr2;

    res = bigram1->first_word - bigram2->first_word;
    if (res == 0)
    {
        res = bigram1->second_word - bigram2->second_word;
    }

    return res;
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

    if (sscanf(start_time_label, "%d", &(node->start_time)) != 1)
    {
        return 0;
    }
    if (node->start_time < 0)
    {
        return 0;
    }

    if (sscanf(end_time_label, "%d", &(node->end_time)) != 1)
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
    new_node.start_time = -1;
    new_node.end_time = -1;
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
    int is_equal = 0;
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
    new_node.start_time = -1;
    new_node.end_time = -1;
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

int calculate_phonemes_probabilities(
        char *confusion_matrix_name, int phonemes_number,
        float phonemes_probabilities_matrix[])
{
    FILE *matrix_file = NULL;
    int *confusion_matrix = NULL;
    char buffer[BUFFER_SIZE];
    char *value_str;
    int i, j, buffer_size, value = 0, is_ok = 1;
    float sum_value = 0.0;

    if ((confusion_matrix_name == NULL) || (phonemes_number <= 0)
            || (phonemes_probabilities_matrix == NULL))
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
        if (value > 0)
        {
            for (j = 0; j < phonemes_number; j++)
            {
                phonemes_probabilities_matrix[i*phonemes_number+j]
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
    return is_ok;
}

int load_words_vocabulary(char *file_name, char ***words_vocabulary)
{
    int i, j, n, buffer_size = 0, is_found = 0, vocabulary_size = 0;
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
        /*i = find_in_vocabulary(*words_vocabulary, vocabulary_size, word_name,
                               &is_found);
        if (!is_found)
        {
            *words_vocabulary = realloc(*words_vocabulary,
                                        (vocabulary_size+1) * sizeof(char*));
            temp_vocabulary = *words_vocabulary;
            if (i >= 0)
            {
                for (j = vocabulary_size; j > i; j--)
                {
                    temp_vocabulary[j] = temp_vocabulary[j-1];
                }
            }
            else
            {
                i = vocabulary_size;
            }
            n = strlen(word_name);
            temp_vocabulary[i] = malloc((n+1) * sizeof(char));
            memset(temp_vocabulary[i], 0, (n+1) * sizeof(char));
            strcpy(temp_vocabulary[i], word_name);
            vocabulary_size++;
        }*/
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
    int i, n = 0, is_ok = 1;
    FILE *h_file = NULL;

    if (language_model == NULL)
    {
        return 0;
    }
    language_model->unigrams_number = 0;
    language_model->unigrams_probabilities = NULL;
    language_model->bigrams_number = 0;
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

    if (fread(&n, sizeof(int), 1, h_file) != 1)
    {
        fclose(h_file);
        return 0;
    }
    if (n <= 0)
    {
        free_language_model(language_model);
        fclose(h_file);
        return 0;
    }
    language_model->bigrams = malloc(sizeof(TWordBigram) * n);
    language_model->bigrams_number = n;
    if (fread(language_model->bigrams, sizeof(TWordBigram), n, h_file) != n)
    {
        free_language_model(language_model);
        fclose(h_file);
        return 0;
    }
    fclose(h_file);
    for (i = 0; i < n; i++)
    {
        if ((language_model->bigrams[i].first_word < 0)
                || (language_model->bigrams[i].first_word >= words_number))
        {
            is_ok = 0;
            break;
        }
        if ((language_model->bigrams[i].second_word < 0)
                || (language_model->bigrams[i].second_word >= words_number))
        {
            is_ok = 0;
            break;
        }
        if ((language_model->bigrams[i].probability < 0.0)
                || (language_model->bigrams[i].probability > 1.0))
        {
            is_ok = 0;
            break;
        }
    }
    if (!is_ok)
    {
        free_language_model(language_model);
        return 0;
    }
    qsort(language_model->bigrams, n, sizeof(TWordBigram), compare_bigrams);
    return 1;
}

int save_language_model(char *file_name, TLanguageModel language_model)
{
    int n;
    FILE *h_file = NULL;

    if ((file_name == NULL) || (language_model.bigrams_number <= 0)
            || (language_model.unigrams_number <= 0)
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

    n = language_model.bigrams_number;
    if (fwrite(&n, sizeof(int), 1, h_file) != 1)
    {
        fclose(h_file);
        return 0;
    }
    if (fwrite(language_model.bigrams, sizeof(TWordBigram), n, h_file) != n)
    {
        fclose(h_file);
        return 0;
    }

    fclose(h_file);
    return 1;
}

int calculate_language_model(TMLFFilePart *words_mlf_data, int files_number,
                             int words_number, float lambda, float eps,
                             TLanguageModel *language_model)
{
    int i, j, word_index, is_ok = 1, total_words_count = 0;
    int bigram_frequency, start_word_ind, end_word_ind;
    int *words_frequencies = NULL;
    float bigram_probability;
    TWordBigram *cur_bigram;

    if ((words_mlf_data == NULL) || (files_number <= 0) || (words_number <= 0)
            || (lambda < 0.0) || (lambda > 1.0) || (eps < 0.0) || (eps >= 1.0)
            || (language_model == NULL))
    {
        return 0;
    }

    language_model->unigrams_number = words_number;
    language_model->unigrams_probabilities = malloc(words_number
                                                    * sizeof(float));
    words_frequencies = malloc(words_number * sizeof(int));
    memset(words_frequencies, 0, words_number * sizeof(int));
    language_model->bigrams_number = 0;
    language_model->bigrams = NULL;

    for (i = 0; i < words_number; i++)
    {
        language_model->unigrams_probabilities[i] = 0.0;
    }
    for (i = 0; i < files_number; i++)
    {
        for (j = 0; j < words_mlf_data[i].transcription_size; j++)
        {
            word_index = words_mlf_data[i].transcription[j].node_data;
            if ((word_index >= words_number) || (word_index < 0))
            {
                is_ok = 0;
                break;
            }
            words_frequencies[word_index]++;
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
        /*if (words_frequencies[i] < 1)
        {
            is_ok = 0;
            break;
        }*/
        language_model->unigrams_probabilities[i]
                = (float)words_frequencies[i] / (float)total_words_count;
    }
    if (!is_ok)
    {
        free_language_model(language_model);
        free(words_frequencies);
        return 0;
    }

    for (start_word_ind = 0; start_word_ind < words_number; start_word_ind++)
    {
        for (end_word_ind = 0; end_word_ind < words_number; end_word_ind++)
        {
            bigram_frequency = 0;
            for (i = 0; i < files_number; i++)
            {
                for (j = 1; j < words_mlf_data[i].transcription_size; j++)
                {
                    if (words_mlf_data[i].transcription[j-1].node_data
                            != start_word_ind)
                    {
                        continue;
                    }
                    if (words_mlf_data[i].transcription[j].node_data != end_word_ind)
                    {
                        continue;
                    }
                    bigram_frequency++;
                }
            }
            bigram_probability = (float)bigram_frequency
                    / (float)words_frequencies[start_word_ind];
            bigram_probability = lambda * bigram_probability + (1.0 - lambda)
                    * language_model->unigrams_probabilities[end_word_ind];
            if (bigram_probability <= eps)
            {
                continue;
            }
            language_model->bigrams_number++;
            language_model->bigrams = realloc(
                        language_model->bigrams,
                        language_model->bigrams_number * sizeof(TWordBigram));
            cur_bigram = language_model->bigrams
                    + language_model->bigrams_number - 1;
            cur_bigram->first_word = start_word_ind;
            cur_bigram->second_word = end_word_ind;
            cur_bigram->probability = bigram_probability;
        }
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
    int is_found = 0, is_ok = 1;
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
        /*word_index = find_in_vocabulary(words_vocabulary, words_number,
                                        word_name, &is_found);
        if (!is_found)
        {
            is_ok = 0;
            break;
        }*/
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
    int word_index, phonemes_sequence_size = 0;
    int is_found = 0, is_ok = 1;
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
        /*word_index = find_in_vocabulary(words_vocabulary, words_number,
                                        word_name, &is_found);
        if (!is_found)
        {
            is_ok = 0;
            break;
        }*/
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
        tmp_array[words_lexicon_size-1].phonemes_number
                = phonemes_sequence_size;
        tmp_array[words_lexicon_size-1].phonemes_indexes
                = malloc(sizeof(int) * phonemes_sequence_size);
        memmove(tmp_array[words_lexicon_size-1].phonemes_indexes,
                phonemes_sequence, phonemes_sequence_size * sizeof(int));
    }
    fclose(vocabulary_file);
    if (!is_ok)
    {
        free_linear_words_lexicon(linear_words_lexicon, words_lexicon_size);
        words_lexicon_size = 0;
    }

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
        free(language_model->bigrams);
        language_model->bigrams = NULL;
    }
    language_model->unigrams_number = 0;
    language_model->bigrams_number = 0;
}

float get_bigram_probability(TLanguageModel language_model, int start_word_ind,
                             int end_word_ind)
{
    int first, last, middle, cmp_res;

    if ((language_model.unigrams_probabilities == NULL)
            || (language_model.unigrams_number <= 0)
            || (language_model.bigrams == NULL)
            || (language_model.bigrams_number <= 0))
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

    first = 0; last = language_model.bigrams_number;

    cmp_res = language_model.bigrams[0].first_word - start_word_ind;
    if (cmp_res == 0)
    {
        cmp_res = language_model.bigrams[0].second_word - end_word_ind;
    }
    if (cmp_res > 0)
    {
        return 0.0;
    }
    if (cmp_res == 0)
    {
        return language_model.bigrams[0].probability;
    }

    cmp_res = language_model.bigrams[last-1].first_word - start_word_ind;
    if (cmp_res == 0)
    {
        cmp_res = language_model.bigrams[last-1].second_word - end_word_ind;
    }
    if (cmp_res < 0)
    {
        return 0.0;
    }
    if (cmp_res == 0)
    {
        return language_model.bigrams[last-1].probability;
    }

    while (first < last)
    {
        middle = first + (last - first) / 2;

        cmp_res = start_word_ind - language_model.bigrams[middle].first_word;
        if (cmp_res == 0)
        {
            cmp_res = end_word_ind -language_model.bigrams[middle].second_word;
        }

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

    cmp_res = language_model.bigrams[last].first_word-start_word_ind;
    if (cmp_res == 0)
    {
        cmp_res = language_model.bigrams[last].second_word-end_word_ind;
    }
    if (cmp_res == 0)
    {
        return language_model.bigrams[last].probability;
    }
    return 0.0;
}

int recognize_words(
        TMLFFilePart *source_phonemes_MLF, int number_of_MLF_files,
        int phonemes_number, float phonemes_probabilities[],
        int words_number, TLinearWordsLexicon words_lexicon[],
        TLanguageModel language_model, TMLFFilePart **result_words_MLF)
{
    int is_ok = 1;
    int i, j, n;
    int phonemes_sequence_length;
    int words_sequence_length, max_words_sequence_length = 0;
    int *words_sequence = NULL;
    TMLFFilePart *cur_src, *cur_result;
    TViterbiMatrix data;
    TTranscriptionNode *recognized_phonemes_sequence = NULL;

    if ((source_phonemes_MLF == NULL) || (number_of_MLF_files <= 0)
            || (phonemes_number <= 0) || (phonemes_probabilities == NULL)
            || (words_number <= 0) || (words_lexicon == NULL)
            || (language_model.unigrams_number != words_number)
            || (language_model.unigrams_probabilities == NULL)
            || (language_model.bigrams_number <= 0)
            || (language_model.bigrams == NULL) || (result_words_MLF == NULL))
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
        cur_result->name = malloc((n+1)*sizeof(char));
        memset(cur_result->name, 0, (n+1)*sizeof(char));
        strcpy(cur_result->name, cur_src->name);
        cur_result++;
        cur_src++;
    }

    data.cells = NULL;
    data.words_sizes = NULL;
    data.times_number = 0;
    data.words_number = 0;

    cur_src = source_phonemes_MLF;
    cur_result = *result_words_MLF;
    phonemes_sequence_length = create_phonemes_sequence_by_transcription(
                cur_src->transcription, cur_src->transcription_size,
                &recognized_phonemes_sequence);
    max_words_sequence_length = cur_src->transcription_size;
    words_sequence = realloc(words_sequence, max_words_sequence_length
                             * sizeof(int));
    create_viterbi_matrix(&data, phonemes_sequence_length, words_number,
                          words_lexicon);
    initialize_values_of_viterbi_matrix(data);
    calculate_viterbi_matrix(
                data, recognized_phonemes_sequence, phonemes_number,
                phonemes_probabilities, words_lexicon, language_model);
    free(recognized_phonemes_sequence);
    recognized_phonemes_sequence = NULL;
    words_sequence_length = calculate_words_sequence_by_viterbi_matrix(
                data, words_sequence);
    if (words_sequence_length > 0)
    {
        cur_result->transcription_size = words_sequence_length;
        cur_result->transcription = malloc(words_sequence_length
                                           * sizeof(TTranscriptionNode));
        for (j = 0; j < words_sequence_length; j++)
        {
            cur_result->transcription[j].node_data = words_sequence[j];
            cur_result->transcription[j].start_time = -1;
            cur_result->transcription[j].end_time = -1;
            cur_result->transcription[j].probability = 1.0;
        }
    }
    cur_src++;
    cur_result++;

    for (i = 1; i < number_of_MLF_files; i++)
    {
        phonemes_sequence_length = create_phonemes_sequence_by_transcription(
                    cur_src->transcription, cur_src->transcription_size,
                    &recognized_phonemes_sequence);
        if (cur_src->transcription_size > max_words_sequence_length)
        {
            max_words_sequence_length = cur_src->transcription_size;
            words_sequence = realloc(words_sequence, max_words_sequence_length
                                     * sizeof(int));
        }
        resize_viterbi_matrix(&data, phonemes_sequence_length);
        initialize_values_of_viterbi_matrix(data);
        calculate_viterbi_matrix(
                    data, recognized_phonemes_sequence, phonemes_number,
                    phonemes_probabilities, words_lexicon, language_model);
        free(recognized_phonemes_sequence);
        recognized_phonemes_sequence = NULL;
        words_sequence_length = calculate_words_sequence_by_viterbi_matrix(
                    data, words_sequence);
        if (words_sequence_length > 0)
        {
            cur_result->transcription_size = words_sequence_length;
            cur_result->transcription = malloc(words_sequence_length
                                               * sizeof(TTranscriptionNode));
            for (j = 0; j < words_sequence_length; j++)
            {
                cur_result->transcription[j].node_data = words_sequence[j];
                cur_result->transcription[j].start_time = -1;
                cur_result->transcription[j].end_time = -1;
                cur_result->transcription[j].probability = 1.0;
            }
        }
        cur_src++;
        cur_result++;
    }
    delete_viterbi_matrix(&data);
    free(words_sequence);
    if (!is_ok)
    {
        free_MLF(result_words_MLF, number_of_MLF_files);
    }

    return is_ok;
}
