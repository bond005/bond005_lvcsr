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

#include <stdlib.h>
#include <string.h>
#include "bond005_lvcsr_lib.h"

#define BUFFER_SIZE 1001

/* This function reads one line from the given text file and trims this line
 * (trimming is deletion of initial and final spaces, tabs and nonprinted
 * characters).
 *
 * This function returns length of line which has been read. In case of error
 * this function returns zero.
 *
 * Arguments:
 * 1. read_file Handle of text file.
 * 2. str Line which will be read.
 */
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
    if (vocabulary_size <= 0)
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

int load_phones_transcriptions(char *mlf_name, char **phones_vocabulary,
                               PTranscriptionNode *transcriptions_of_files,
                               char **names_of_files, int files_number)
{
    //...
}

int load_words_transcriptions(char *mlf_name, char **words_vocabulary,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number)
{
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
                       int words_number, TWordBigram bigrams[],
                       int bigrams_number)
{
    //...
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

void free_vocabulary(char ***vocabulary, int vocabulary_size)
{
    register int i;

    if ((vocabulary == NULL) || (vocabulary_size <= 0))
    {
        return;
    }
    if (*vocabulary == NULL)
    {
        return;
    }

    for (i = 0; i < vocabulary_size; i++)
    {
        if (*vocabulary[i] != NULL)
        {
            free(*vocabulary[i]);
            *vocabulary[i] = NULL;
        }
    }
    free(*vocabulary);
    *vocabulary = NULL;
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
