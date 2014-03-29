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
        if ((str[start_pos] != ' ') && (str[start_pos] > 13))
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
        if ((str[end_pos] != ' ') && (str[end_pos] > 13))
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
                        if ((filename[i] != ' ') && (filename[i] > 13))
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
                            if ((filename[i] != ' ') && (filename[i] > 13))
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

int string_to_transcription_node(char *str,
                                 char *vocabulary[], int vocabulary_size,
                                 PTranscriptionNode node)
{
    char *start_time_label = NULL, *end_time_label = NULL;
    char *node_name = NULL, *probability_str = NULL;
    int is_equal = 0;

    if ((vocabulary_size <= 0) || (vocabulary == NULL) || (str == NULL))
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

    node->node_data = find_in_vocabulary(vocabulary, vocabulary_size,
                                         node_name, &is_equal);
    if (!is_equal)
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
    bigram->second_word = find_in_vocabulary(words_vocabulary, words_number,
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
        if ((str[j] != ' ') && (str[j] > 13))
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
        if ((str[j] != ' ') && (str[j] > 13))
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

int parse_transcription_str(char *transcription_str, char **phones_vocabulary,
                            int phones_number, int phones_sequence[])
{
    char *phone_name = NULL;
    int i = 0, is_equal = 0, is_ok = 1;

    if ((transcription_str == NULL) || (phones_vocabulary == NULL)
            || (phones_number <= 0) || (phones_sequence == NULL))
    {
        return 0;
    }

    phone_name = strtok(transcription_str, " \t");
    if (phone_name == NULL)
    {
        return 0;
    }
    phones_sequence[i] = find_in_vocabulary(phones_vocabulary, phones_number,
                                            phone_name, &is_equal);
    if (!is_equal)
    {
        return 0;
    }
    i++;
    phone_name = strtok(NULL, " \t");

    while (phone_name != NULL)
    {
        phones_sequence[i]=find_in_vocabulary(phones_vocabulary, phones_number,
                                              phone_name, &is_equal);
        if (!is_equal)
        {
            is_ok = 0;
            break;
        }
        i++;
        phone_name = strtok(NULL, " \t");
    }

    if (!is_ok)
    {
        return 0;
    }
    return i;
}

int add_word_to_words_tree(int word_index, int word_phones[], int word_length,
                           PWordsTreeNode words_tree_root)
{
    int is_ok = 1, i, n, next_i;
    PWordsTreeNode next_node;

    if ((word_index < 0) || (word_phones == NULL) || (word_length <= 0)
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
        if (word_phones[i] <= 0)
        {
            is_ok = 0;
            break;
        }
        n = words_tree_root->number_of_next_nodes;
        next_i = n - 1;
        next_node = words_tree_root->next_nodes + next_i;
        while (next_i >= 0)
        {
            if (next_node->node_type != PHONE_NODE)
            {
                next_i--;
                next_node--;
            }
            else
            {
                if (next_node->node_data == word_phones[i])
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
            next_node->node_data = word_phones[i];
            next_node->node_type = PHONE_NODE;
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

int load_phones_MLF(char *mlf_name, char **phones_vocabulary,int phones_number,
                    TMLFFilePart **mlf_data)
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
            || (phones_vocabulary == NULL) || (phones_number <= 0))
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
                if (!string_to_transcription_node(buffer, phones_vocabulary,
                                                  phones_number, &new_node))
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
                new_node.node_data = find_in_vocabulary(
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
            if (fprintf(mlf_file, "%s", ".") <= 0)
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
            *phones_vocabulary = realloc(*phones_vocabulary,
                                         (vocabulary_size+1) * sizeof(char*));
            for (j = vocabulary_size + 1; j > i; j--)
            {
                *phones_vocabulary[j] = *phones_vocabulary[j-1];
            }
            *phones_vocabulary[i] = malloc((buffer_size+1) * sizeof(char));
            strcpy(*phones_vocabulary[i], buffer);
            vocabulary_size++;
        }
    }
    fclose(vocabulary_file);

    return vocabulary_size;
}

int load_words_vocabulary(char *file_name, char ***words_vocabulary)
{
    int i, j, buffer_size = 0, is_found = 0, vocabulary_size = 0;
    int is_ok = 1;
    char buffer[BUFFER_SIZE];
    char *word_name = NULL, *word_transcription = NULL;
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
        i = find_in_vocabulary(*words_vocabulary, vocabulary_size, word_name,
                               &is_found);
        if (!is_found)
        {
            *words_vocabulary = realloc(*words_vocabulary,
                                        (vocabulary_size+1) * sizeof(char*));
            for (j = vocabulary_size + 1; j > i; j--)
            {
                *words_vocabulary[j] = *words_vocabulary[j-1];
            }
            *words_vocabulary[i] = malloc((buffer_size+1) * sizeof(char));
            strcpy(*words_vocabulary[i], buffer);
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
            *bigrams = realloc(*bigrams,
                               (bigrams_number+1) * sizeof(TWordBigram));
            for (j = bigrams_number + 1; j > i; j--)
            {
                *bigrams[j] = *bigrams[j-1];
            }
            *bigrams[i] = new_bigram;
            bigrams_number++;
        }
    }
    fclose(bigrams_file);
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
    PWordsTreeNode root = NULL;
    int buffer_size = 0, vocabulary_size = 0;
    int word_index, phones_sequence_size = 0;
    int is_found = 0, is_ok = 1;
    char buffer[BUFFER_SIZE];
    int phones_sequence[BUFFER_SIZE];
    char *word_name = NULL, *word_transcription = NULL;
    FILE *vocabulary_file = NULL;

    if ((file_name==NULL) || (words_vocabulary == NULL) || (words_number <= 0)
            || (phones_vocabulary == NULL) || (phones_number <= 0))
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
        word_index = find_in_vocabulary(words_vocabulary, vocabulary_size,
                                        word_name, &is_found);
        if (!is_found)
        {
            is_ok = 0;
            break;
        }
        phones_sequence_size = parse_transcription_str(
                    word_transcription, phones_vocabulary, phones_number,
                    phones_sequence);
        if (phones_sequence_size <= 0)
        {
            is_ok = 0;
            break;
        }
        if (!add_word_to_words_tree(word_index, phones_sequence,
                                    phones_sequence_size, root))
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
        else if (cur->node_type == PHONE_NODE)
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
        PWordsTreeNode words_tree,
        TWordBigram bigrams[], int bigrams_number,
        PTranscriptionNode *recognized_words)
{
    //...
}
