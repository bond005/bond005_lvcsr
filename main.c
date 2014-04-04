/* main.c
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

enum TExecutionMode { emUNKNOWN, emTRAINING, emTESTING };

static int get_execution_mode(int argc, char *argv[]);
static int get_parameters_of_training(
        int argc, char *argv[], char **mlf_file_name,
        char **words_vocabulary, float *lambda, float *eps,
        char **language_model_name);
static int get_parameters_of_testing(
        int argc,char *argv[], char **source_file_name,char **result_file_name,
        char **phonemes_vocabulary, char **confusion_matrix_name,
        char **words_vocabulary, char **language_model_name);
static int train_language_model_by_mlf_file(int argc, char *argv[]);
static int recognize_speech_by_mlf_file(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int res = EXIT_SUCCESS;
    int mode = get_execution_mode(argc, argv);
    if (mode == emUNKNOWN)
    {
        fprintf(stderr, "There are incorrect parameters of command prompt "\
                "(execution mode of this program cannot be defined).\n");
        return EXIT_FAILURE;
    }
    if (mode == emTRAINING)
    {
        if(!train_language_model_by_mlf_file(argc, argv))
        {
            res = EXIT_FAILURE;
        }
    }
    else
    {
        if(!recognize_speech_by_mlf_file(argc, argv))
        {
            res = EXIT_FAILURE;
        }
    }
    return res;
}

int get_execution_mode(int argc, char *argv[])
{
    int i;
    int res = emUNKNOWN;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-test") == 0)
        {
            res = emTESTING;
            break;
        }
        if (strcmp(argv[i], "-train") == 0)
        {
            res = emTRAINING;
            break;
        }
    }
    return res;
}

int get_parameters_of_training(
        int argc, char *argv[], char **mlf_file_name,
        char **words_vocabulary, float *lambda, float *eps,
        char **language_model_name)
{
    int i, n = 0, is_ok = 0;

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-src") == 0)
        {
            is_ok = 1;
            *mlf_file_name = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-words") == 0)
        {
            is_ok = 1;
            *words_vocabulary = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-lang") == 0)
        {
            is_ok = 1;
            *language_model_name = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-lambda") == 0)
        {
            if (sscanf(argv[i+1], "%f", lambda) != 1)
            {
                break;
            }
            if ((*lambda < 0.0) || (*lambda > 1.0))
            {
                break;
            }
            is_ok = 1;
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-eps") == 0)
        {
            if (sscanf(argv[i+1], "%f", eps) != 1)
            {
                break;
            }
            if ((*eps < 0.0) || (*eps >= 1.0))
            {
                break;
            }
            is_ok = 1;
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    return ((n * 2) == (argc-2));
}

int get_parameters_of_testing(
        int argc,char *argv[], char **source_file_name,char **result_file_name,
        char **phonemes_vocabulary, char **confusion_matrix_name,
        char **words_vocabulary, char **language_model_name)
{
    int i, n = 0, is_ok = 0;

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-src") == 0)
        {
            is_ok = 1;
            *source_file_name = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-res") == 0)
        {
            is_ok = 1;
            *result_file_name = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-phonemes") == 0)
        {
            is_ok = 1;
            *phonemes_vocabulary = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-words") == 0)
        {
            is_ok = 1;
            *words_vocabulary = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-conf") == 0)
        {
            is_ok = 1;
            *confusion_matrix_name = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    for (i = 0; i < (argc-1); i++)
    {
        if (strcmp(argv[i], "-lang") == 0)
        {
            is_ok = 1;
            *language_model_name = argv[i+1];
            n++;
            break;
        }
    }
    if (!is_ok)
    {
        return 0;
    }

    return ((n * 2) == (argc-2));
}

int train_language_model_by_mlf_file(int argc, char *argv[])
{
    char *mlf_file_name = NULL;
    char *words_vocabulary_name = NULL;
    char *language_model_name = NULL;
    float lambda = 1.0, eps = 0.0;
    TMLFFilePart *data = NULL;
    int files_number_in_MLF;
    char **words_vocabulary = NULL;
    int words_number;
    TLanguageModel model;

    if (!get_parameters_of_training(
                argc,argv, &mlf_file_name, &words_vocabulary_name, &lambda,
                &eps, &language_model_name))
    {
        fprintf(stderr, "Parameters of command prompt are incorrect.\n");
        return 0;
    }
    words_number = load_words_vocabulary(words_vocabulary_name,
                                         &words_vocabulary);
    if (words_number <= 0)
    {
        fprintf(stderr, "The given words vocabulary cannot be loaded.\n");
        return 0;
    }
    files_number_in_MLF = load_words_MLF(mlf_file_name, words_vocabulary,
                                         words_number, &data);
    if (files_number_in_MLF <= 0)
    {
        free_string_array(&words_vocabulary, words_number);
        fprintf(stderr, "The given MLF file cannot be loaded.\n");
        return 0;
    }
    if (!calculate_language_model(data, files_number_in_MLF, words_number,
                                  lambda, eps, &model))
    {
        free_string_array(&words_vocabulary, words_number);
        free_MLF(&data, files_number_in_MLF);
        fprintf(stderr, "The language model cannot be calculated (probably, "\
                "input data is incorrect).\n");
        return 0;
    }
    free_string_array(&words_vocabulary, words_number);
    free_MLF(&data, files_number_in_MLF);
    if (!save_language_model(language_model_name, model))
    {
        free_language_model(&model);
        fprintf(stderr, "The language model cannot be saved into the given "\
                "file.\n");
        return 0;
    }
    free_language_model(&model);
    return 1;
}

int recognize_speech_by_mlf_file(int argc, char *argv[])
{
    char *source_file_name = NULL;
    char *result_file_name = NULL;
    char *phonemes_vocabulary_name = NULL;
    char *confusion_matrix_name = NULL;
    char *words_vocabulary_name = NULL;
    char *language_model_name = NULL;

    TMLFFilePart *src_data = NULL, *res_data = NULL;
    int files_in_MLF = 0;
    char **phonemes_vocabulary, **words_vocabulary;
    int phonemes_number, words_number;
    TLinearWordsLexicon *words_lexicon = NULL;
    TLanguageModel language_model;
    float *phonemes_probabilities_matrix = NULL;

    if (!get_parameters_of_testing(
                argc, argv, &source_file_name, &result_file_name,
                &phonemes_vocabulary_name, &confusion_matrix_name,
                &words_vocabulary_name, &language_model_name))
    {
        fprintf(stderr, "Parameters of command prompt are incorrect.\n");
        return 0;
    }
    phonemes_number = load_phonemes_vocabulary(phonemes_vocabulary_name,
                                               &phonemes_vocabulary);
    if (phonemes_number <= 0)
    {
        fprintf(stderr, "The given phonemes vocabulary cannot be loaded.\n");
        return 0;
    }
    words_number = load_words_vocabulary(words_vocabulary_name,
                                         &words_vocabulary);
    if (words_number <= 0)
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        fprintf(stderr, "The given words vocabulary cannot be loaded.\n");
        return 0;
    }
    phonemes_probabilities_matrix = malloc(sizeof(float) * phonemes_number
                                           * phonemes_number);
    if (!calculate_phonemes_probabilities(
                confusion_matrix_name, phonemes_number,
                phonemes_probabilities_matrix))
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        free_string_array(&words_vocabulary, words_number);
        free(phonemes_probabilities_matrix);
        fprintf(stderr, "The phonemes probabilities matrix cannot be "\
                "calculated (probably, source confusion matrix is incorrect, "\
                "or it cannot be loaded from the given file).\n");
        return 0;
    }
    if (!create_linear_words_lexicon(
                words_vocabulary_name, phonemes_vocabulary, phonemes_number,
                words_vocabulary, words_number, &words_lexicon))
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        free_string_array(&words_vocabulary, words_number);
        free(phonemes_probabilities_matrix);
        fprintf(stderr, "The words lexicon cannot be created on basis of "\
                "given phonemes and words vocabularies.\n");
        return 0;
    }
    if (!load_language_model(language_model_name, words_number,
                             &language_model))
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        free_string_array(&words_vocabulary, words_number);
        free(phonemes_probabilities_matrix);
        free_linear_words_lexicon(&words_lexicon, words_number);
        fprintf(stderr, "The language model cannot be loaded from the given "\
                "file.\n");
        return 0;
    }

    files_in_MLF = load_phonemes_MLF(
                source_file_name, phonemes_vocabulary, phonemes_number,
                &src_data);
    if (files_in_MLF <= 0)
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        free_string_array(&words_vocabulary, words_number);
        free(phonemes_probabilities_matrix);
        free_linear_words_lexicon(&words_lexicon, words_number);
        free_language_model(&language_model);
        fprintf(stderr, "The source data (phonemes transcriptions in the MLF "\
                "file) cannot be loaded from the given file.\n");
        return 0;
    }
    if (!recognize_words(
                src_data, files_in_MLF, phonemes_number,
                phonemes_probabilities_matrix, words_number, words_lexicon,
                language_model, &res_data))
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        free_string_array(&words_vocabulary, words_number);
        free(phonemes_probabilities_matrix);
        free_linear_words_lexicon(&words_lexicon, words_number);
        free_language_model(&language_model);
        free_MLF(&src_data, files_in_MLF);
        fprintf(stderr, "The input data cannot be recognized (probably, this "\
                "data are not valid, or recognition parameters are "\
                "incorrect).\n");
        return 0;
    }
    if (!save_words_MLF(result_file_name, words_vocabulary, words_number, res_data, files_in_MLF))
    {
        free_string_array(&phonemes_vocabulary, phonemes_number);
        free_string_array(&words_vocabulary, words_number);
        free(phonemes_probabilities_matrix);
        free_linear_words_lexicon(&words_lexicon, words_number);
        free_language_model(&language_model);
        free_MLF(&src_data, files_in_MLF);
        free_MLF(&res_data, files_in_MLF);
        fprintf(stderr, "The recognition results cannot be saved into the "\
                "given file.\n");
        return 0;
    }

    free_string_array(&phonemes_vocabulary, phonemes_number);
    free_string_array(&words_vocabulary, words_number);
    free(phonemes_probabilities_matrix);
    free_linear_words_lexicon(&words_lexicon, words_number);
    free_language_model(&language_model);
    free_MLF(&src_data, files_in_MLF);
    free_MLF(&res_data, files_in_MLF);
    return 1;
}
