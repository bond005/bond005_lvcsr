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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bond005_lvcsr_lib.h"

/* There are states of the MLF file reading.
 */
enum {
    HEADER_EXPECTATION_STATE,/* The initial state for reading of all MLF file
                                (such MLF file begins with special header). */
    FILENAME_READING_STATE,  /* The initial state for reading of one
                                transcription in the MLF file (such
                                transcription always begins with name of file
                                containing the source acoustical data). */
    EVENT_READING_STATE      /* The state for reading of acoustic events, i.e.
                                transcription nodes. */
};

int read_string(FILE *read_file, char *str)
{
    int start_pos = 0, end_pos = 0;

    if (fgets(str, BUFFER_SIZE, read_file) == NULL)
    {
        return 0;
    }

    while (str[start_pos] != NULL)
    {
        if ((str[start_pos] != ' ') && (str[start_pos] > 13))
        {
            break;
        }
        start_pos++;
    }
    if (str[start_pos] == NULL)
    {
        memset(str, NULL, BUFFER_SIZE);
        return 0;
    }

    end_pos = strlen(str) - 1;
    while (end_pos > start_pos)
    {
        if ((str[end_pos] != ' ') && (str[end_pos] > 13))
        {
            break;
        }
        end_pos--;
    }
    str[end_pos+1] = NULL;

    if (start_pos > 0)
    {
        memmove(&str[0], &str[start_pos], end_pos-start_pos+1);
    }

    return (end_pos - start_pos + 1);
}

int find_in_vocabulary(char *vocabulary[], int vocabulary_size,
                       char *found_name, int *is_equal)
{
    if (is_equal != NULL)
    {
        *is_equal = 0;
    }
    if ((vocabulary_size < 0) || ((vocabulary_size > 0)&&(vocabulary == NULL)))
    {
        return -1;
    }
    if (vocabulary_size == 0)
    {
        return 0;
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
        if ((compare_res == 0) && (is_equal != NULL))
        {
            *is_equal = 1;
        }
        return last_pos;
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

int find_in_bigrams_list(TWordBigram bigrams[], int bigrams_number,
                         TWordBigram found_bigram, int *is_equal)
{
    if (is_equal != NULL)
    {
        *is_equal = 0;
    }
    if ((bigrams_number < 0) || ((bigrams_number > 0) && (bigrams == NULL)))
    {
        return -1;
    }
    if (bigrams_number == 0)
    {
        return 0;
    }

    int compare_res, first_pos = 0, last_pos = bigrams_number, middle_pos;

    compare_res = bigrams[0].first_word-found_bigram.first_word;
    if (compare_res == 0)
    {
        compare_res = bigrams[0].second_word-found_bigram.second_word;
    }
    if (compare_res >= 0)
    {
        if ((compare_res == 0) && (is_equal != NULL))
        {
            *is_equal = 1;
        }
        return 0;
    }

    compare_res = bigrams[last_pos-1].first_word-found_bigram.first_word;
    if (compare_res == 0)
    {
        compare_res = bigrams[last_pos-1].second_word-found_bigram.second_word;
    }
    if (compare_res <= 0)
    {
        if ((compare_res == 0) && (is_equal != NULL))
        {
            *is_equal = 1;
        }
        return last_pos;
    }

    while (first_pos < last_pos)
    {
        middle_pos = first_pos + (last_pos - first_pos) / 2;
        compare_res = found_bigram.first_word-bigrams[middle_pos].first_word;
        if (compare_res == 0)
        {
            compare_res
                    = found_bigram.second_word-bigrams[middle_pos].second_word;
        }
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

int string_to_bigram(char *str, char *words_vocabulary[], int words_number,
                     TWordBigram *bigram)
{
    char *first_word = NULL, *second_word = NULL, *probability_str = NULL;
    int is_equal = 0;

    if ((words_number <= 0) || (words_vocabulary == NULL) || (str == NULL))
    {
        return 0;
    }

    first_word = strtok(str, " \t");
    if (first_word == NULL)
    {
        return 0;
    }
    second_word = strtok(NULL, " \t");
    if (second_word == NULL)
    {
        return 0;
    }
    probability_str = strtok(NULL, " \t");
    if (probability_str == NULL)
    {
        return 0;
    }
    if (strtok(NULL, " \t") != NULL)
    {
        return 0;
    }

    bigram->first_word = find_in_vocabulary(words_vocabulary, words_number,
                                            first_word, &is_equal);
    if ((!is_equal) || (bigram->first_word < 0))
    {
        return 0;
    }
    bigram->fsecond_word = find_in_vocabulary(words_vocabulary, words_number,
                                              second_word, &is_equal);
    if ((!is_equal) || (bigram->second_word < 0))
    {
        return 0;
    }
    if (sscanf(probability_str, "%f", &(bigram->probability)) != 1)
    {
        return 0;
    }
    if ((bigram->probability < 0.0) || (bigram->probability > 1.0))
    {
        return 0;
    }

    return 1;
}

int load_phones_transcriptions(char *mlf_name,
                               char **phones_vocabulary, int phones_number,
                               PTranscriptionNode **transcriptions_of_files,
                               char ***names_of_files)
{
    //...
}

int load_words_transcriptions(char *mlf_name,
                              char **words_vocabulary, int words_number,
                              PTranscriptionNode **transcriptions_of_files,
                              char ***names_of_files)
{
    int buffer_size = 0, files_number = 0, nodes_number = 0;
    int reading_state = HEADER_EXPECTATION_STATE;
    int is_ok = 1;
    int is_equal = 0;
    char buffer[BUFFER_SIZE];
    FILE *mlf_file = NULL;
    TTranscriptionNode new_node;

    if ((transcriptions_of_files == NULL) || (names_of_files == NULL)
            || (mlf_name == NULL) || (words_vocabulary == NULL)
            || (words_number <= 0))
    {
        return 0;
    }

    *transcriptions_of_files = NULL;
    *names_of_files = NULL;
    memset(buffer, NULL, BUFFER_SIZE);
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
        buffer_size = read_string(vocabulary_file, buffer);
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
            files_number++;
            *names_of_file = realloc(
                        *names_of_file, files_number * sizeof(char));
            *transcriptions_of_files = realloc(
                        *transcriptions_of_files,
                        files_number * sizeof(PTranscriptionNode));
            *names_of_files[files_number-1] = malloc(
                        (buffer_size+1) * sizeof(char));
            memset(*names_of_files[files_number-1], NULL, buffer_size+1);
            strcpy(*names_of_files[files_number-1], buffer);
            *transcriptions_of_files[files_number-1] = NULL;
            reading_state = EVENT_READING_STATE;
            nodes_number = 0;
            break;
        case EVENT_READING_STATE:
            if (strcmp(buffer, ".") == 0)
            {
                if (transcription_size <= 0)
                {
                    is_ok = 0;
                }
            }
            else
            {
                new_node.node_data = find_in_vocabulary(
                            words_vocabulary, words_number, buffer, &is_equal);
                if (!is_equal)
                {
                    is_ok = 0;
                }
                else
                {
                    *transcriptions_of_files[files_number-1] = realloc(
                                *transcriptions_of_files[files_number-1], nodes_number+1);
                    *transcriptions_of_files[files_number-1][nodes_number] = new_node;
                    nodes_number++;
                }
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
    if (is_ok)
    {
        if (reading_state != FILENAME_READING_STATE)
        {
            is_ok = 0;
        }
    }
    //...
}

int save_words_transcriptions(char *mlf_name, char **words_vocabulary,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number)
{
    //...
}

int load_phones_vocabulary(char *file_name, char ***phones_vocabulary)
{
    int i, j, buffer_size = 0, is_found = 0, vocabulary_size = 0;
    char buffer[BUFFER_SIZE];
    FILE *vocabulary_file = NULL;

    if (phones_vocabulary == NULL)
    {
        return 0;
    }
    *phones_vocabulary = NULL;
    memset(buffer, NULL, BUFFER_SIZE);

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
        i = find_in_vocabulary(*phones_vocabulary, vocabulary_size, buffer,
                               &is_found);
        if (!is_found)
        {
            realloc(*phones_vocabulary, (vocabulary_size+1) * sizeof(char*));
            for (j = vocabulary_size + 1; j > i; j--)
            {
                *phones_vocabulary[j] = *phones_vocabulary[j-1];
            }
            *phones_vocabulary[i] = malloc((buffer_size+1) * sizeof(char));
            strcpy(*phones_vocabulary[i], buffer);
            vocabulary_size++;
        }
    }

    return vocabulary_size;
}

int load_words_vocabulary(char *file_name, char ***words_vocabulary)
{
    //...
}

int load_words_bigrams(char *file_name, char **words_vocabulary,
                       int words_number, TWordBigram *bigrams[])
{
    int i, j, buffer_size = 0, is_found = 0, bigrams_number = 0;
    char buffer[BUFFER_SIZE];
    FILE *bigrams_file = NULL;
    TWordBigram new_bigram;

    if (bigrams == NULL)
    {
        return 0;
    }
    *bigrams = NULL;
    memset(buffer, NULL, BUFFER_SIZE);

    bigrams_file = fopen(file_name, "r");
    if (bigrams_file == NULL)
    {
        return 0;
    }
    while (!feof(bigrams_file))
    {
        buffer_size = read_string(bigrams_file, buffer);
        if (buffer_size <= 0)
        {
            continue;
        }
        if (!string_to_bigram(buffer, words_vocabulary, words_number,
                              &new_bigram))
        {
            bigrams_number = 0;
            break;
        }
        i = find_in_bigrams_list(*bigrams, bigrams_number, new_bigram,
                                 &is_found);
        if (!is_found)
        {
            realloc(*bigrams, (bigrams_number+1) * sizeof(TWordBigram));
            for (j = bigrams_number + 1; j > i; j--)
            {
                *bigrams[j] = *bigrams[j-1];
            }
            *bigrams[i] = new_bigram;
            bigrams_number++;
        }
    }
    if ((bigrams_number <= 0) && (*bigrams != NULL))
    {
        free(*bigrams);
        *bigrams = NULL;
    }

    return bigrams_number;
}

PWordsTreeNode create_words_vocabulary_tree(
        char *file_name, char **phones_vocabulary, int phones_number,
        char **words_vocabulary, int words_number)
{
    //...
}

void free_vocabulary_tree(PWordsTreeNode* root_node)
{
    PWordsTreeNode deleted_node = *root_node;
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
            free_vocabulary_tree(&(deleted_node->next_nodes[i]));
        }
        free(deleted_node->next_nodes);
        deleted_node->next_nodes = NULL;
    }
    free(deleted_node);
    *root_node = NULL;
}

void free_string_array(char ***string_array, int array_size)
{
    register int i;

    if ((string_array == NULL) || (array_size <= 0))
    {
        return;
    }
    if (*string_array == NULL)
    {
        return;
    }

    for (i = 0; i < array_size; i++)
    {
        if (*string_array[i] != NULL)
        {
            free(*string_array[i]);
            *string_array[i] = NULL;
        }
    }
    free(*string_array);
    *string_array = NULL;
}

int recognize_words(
        PTranscriptionNode source_phones_transcription,
        char *phones_vocabulary[], int phones_number,
        char *words_vocabulary[], int words_number,
        PWordsTreeNode words_tree,
        TWordBigram bigrams[], int bigrams_number,
        PTranscriptionNode *recognized_words)
{
    //...
}
