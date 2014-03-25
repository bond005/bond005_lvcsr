/** \file bond005_lvcsr_lib.h
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
 * \author Ivan Yu. Bondarenko.
 * \version 0.1
 * \date 2014
 * \copyright GNU General Public License version 3.
 */

#ifndef BOND005_LVCSR_LIB_H
#define BOND005_LVCSR_LIB_H

#include <stdlib.h>

/*! \def BUFFER_SIZE
 * \brief This macro defines maximal size of string buffer which is used at
 * reading of text files.
 */
#define BUFFER_SIZE 1001

/*! \def MLF_HEADER
 * \brief This macro defines header string of each MLF file.
 */
#define MLF_HEADER "#!MLF!#"

/*! \struct TWordsTreeNode
 * \brief Structure for representation of a words tree node. */
typedef struct _TWordsTreeNode {
    int node_data; /**< Data of node: index of word of phone. */
    int node_type; /**< Type of node: 0 - initial node, 1 - phone node,
                     * 2 - word node (final node). */
    int number_of_next_nodes;          /**< Number of next nodes. */
    struct _TWordsTreeNode *next_nodes;/**< Array of pointers to next nodes. */
} TWordsTreeNode;

/*! \typedef PWordsTreeNode
 * \brief Pointer to TWordsTreeNode data.
 * \sa TWordsTreeNode
 */
typedef TWordsTreeNode* PWordsTreeNode;

/*! \struct TTranscriptionNode
 * \brief Structure for representation of one transcription node.
 *
 * The transcription node describes some acoustic event associated with
 * appearance of word or phone in speech. This description involves:
 * - name of acoustic event;
 * - start time of this acoustic event;
 * - stop time of this acoustic event;
 * - event probability.
 */
typedef struct {
    int node_data;    /**< Data of node: index of word or phone. */
    int start_time;   /**< Start time in 100ns units. */
    int end_time;     /**< End time in 100ns units. */
    float probability;/**< Probability of this acoustic event. */
} TTranscriptionNode;

/*! \typedef PTranscriptionNode
 * \brief Pointer to TTranscriptionNode data.
 * \sa TTranscriptionNode
 */
typedef TTranscriptionNode* PTranscriptionNode;

/*! \struct TMLFFileNode
 * \brief Structure for representation of
 */
typedef struct {
    char *name;
    PTranscriptionNode transcription;
    int transcription_size;
} TMLFFileNode;

/*! \struct TWordBigram
 * \brief Structure for representation of one words bigram.
 */
typedef struct {
    int first_word;   /**< Index of first word in bigram. */
    int second_word;  /**< Index of second word in bigram. */
    float probability;/**< Probability of bigram. */
} TWordBigram;

/*! \fn int load_phones_transcriptions(
 *         char *mlf_name, char **phones_vocabulary, int phones_number,
 *         PTranscriptionNode **transcriptions_of_files,
 *         char ***names_of_files);
 *
 * \brief This function loads list of phones transcriptions and names of their
 * files from the given MLF file. This MLF file must contain not only labels of
 * acoustical events (i.e. names of phones) but also start and end times of these
 * events. Besides, probabilities of these acoustical events can be present at
 * the MLF file.
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param phones_vocabulary The string array which represents phones vocabulary.
 *
 * \param phones_number The size of phones vocabulary.
 *
 * \param transcriptions_of_files Pointer to the array of transcriptions into
 * which phones transcriptions will be written. Memory for this array will be
 * allocated automatically in this function.
 *
 * \param names_of_files Pointer to the string array into which names of
 * transcriptions files will be written. Memory for this array will be
 * allocated automatically in this function.
 *
 * \return If the loading has been completed successfully then this function
 * will return number of loaded phones transcriptions and corresponding files
 * names, else this function will return zero.
 */
int load_phones_transcriptions(char *mlf_name,
                               char **phones_vocabulary, int phones_number,
                               PTranscriptionNode **transcriptions_of_files,
                               char ***names_of_files);

/*! \fn int load_words_transcriptions(
 *         char *mlf_name, char **words_vocabulary, int words_number,
 *         PTranscriptionNode **transcriptions_of_files,
 *         char ***names_of_files);
 *
 * \brief This function loads list of words transcriptions and names of their
 * files from the given MLF file. This MLF file must contain only labels of
 * acoustical events (i.e. names of words).
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param words_vocabulary The string array which represents words vocabulary.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param transcriptions_of_files Pointer to the array of transcriptions into
 * which words transcriptions will be written. Memory for this array will be
 * allocated automatically in this function.
 *
 * \param names_of_files Pointer to the string array into which names of
 * transcriptions files will be written. Memory for this array will be
 * allocated automatically in this function.
 *
 * \return If the loading has been completed successfully then this function
 * will return number of loaded words transcriptions and corresponding files
 * names, else this function will return zero.
 */
int load_words_transcriptions(char *mlf_name,
                              char **words_vocabulary, int words_number,
                              PTranscriptionNode **transcriptions_of_files,
                              char ***names_of_files);

/*! \fn int save_words_transcriptions(
 *         char *mlf_name, char **words_vocabulary, int words_number,
 *         PTranscriptionNode *transcriptions_of_files,
 *         char **names_of_files, int files_number)
 *
 * \brief This function saves list of words transcriptions and names of their
 * files into the given MLF file. Only labels of acoustical labels (i.e. names
 * of words) without their start and end times will be written into this MLF
 * file.
 *
 * \param mlf_name The name of MLF file into which list of words transcriptions
 * must be written.
 *
 * \param words_vocabulary The string array which represents words vocabulary.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param transcriptions_of_files The array of words transcriptions which will
 * be saved.
 *
 * \param names_of_files The string array with names of transcriptions files
 * will be saved.
 *
 * \param files_number The number of words transcriptions and corresponding
 * files names which will be saved into MLF file.
 *
 * \return If the saving has been completed successfully then this function
 * will return number of saved words transcriptions and corresponding files
 * names, else this function will return zero.
 */
int save_words_transcriptions(char *mlf_name,
                              char **words_vocabulary, int words_number,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number);

/*! \fn int load_phones_vocabulary(char *file_name, char ***phones_vocabulary)
 *
 * \brief This function loads phones vocabulary (i.e. list of phones) from the
 * given text file into the string array. The phones vocabulary is being sorted
 * automatically at loading.
 *
 * \param file_name The name of text file with phones vocabulary which will be
 * loaded.
 *
 * \param phones_vocabulary Pointer to string array in which the phones
 * vocabulary will be written. Memory for this string array will be allocated
 * automatically in this function.
 *
 * \return This function returns number of phones in case of successful
 * loading, and it returns zero in case of loading error.
 */
int load_phones_vocabulary(char *file_name, char ***phones_vocabulary);

/*! \fn int load_words_vocabulary(char *file_name, char ***words_vocabulary)
 *
 * \brief This function loads words vocabulary (i.e. list of words) from the
 * given text file into the string array. The words vocabulary is being sorted
 * automatically at loading.
 *
 * \param file_name The name of text file with words vocabulary which will be
 * loaded (this file describes vocabulary words and their transcriptions).
 *
 * \param words_vocabulary Pointer to string array in which the words
 * vocabulary will be written. Memory for this string array will be allocated
 * automatically in this function.
 *
 * \return This function returns number of words in case of successful loading,
 * and it returns zero in case of loading error.
 */
int load_words_vocabulary(char *file_name, char ***words_vocabulary);

/*! \fn int load_words_bigrams(
 *         char *file_name, char **words_vocabulary, int words_number,
 *         TWordBigram bigrams[], int bigrams_number)
 *
 * \brief This function loads a bigrams list from the given text file into
 * the TWordBigram array. The bigrams list is being sorted automatically at
 * loading.
 *
 * \param file_name The name of text file with bigrams list. Each line of this
 * file describes one bigram in the following way: name of first word, name of
 * second word and bigram probability by way of spaces.
 *
 * \param words_vocabulary The string array which represents words vocabulary.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param bigrams Pointer to TWordBigram array in which the loaded bigrams list
 * will be written. Memory for this array will be allocated automatically in
 * this function.
 *
 * \return This function returns number of bigrams in case of successful
 * loading, and it returns zero in case of loading error.
 */
int load_words_bigrams(char *file_name, char **words_vocabulary,
                       int words_number, TWordBigram *bigrams[]);

/*! \fn int create_words_vocabulary_tree(
 *         char *file_name, char **phones_vocabulary, int phones_number,
 *         char **words_vocabulary, int words_number,
 *         PWordsTreeNode* root_node)
 *
 * \brief This function loads information about words and their transcriptions
 * from the given text file and creates a new words tree on basis on this
 * information.
 *
 * \param file_name The name of text file with words vocabulary which will be
 * loaded (this file describes vocabulary words and their transcriptions).
 *
 * \param phones_vocabulary The sorted string array which contains names of
 * recognized phones.
 *
 * \param phones_number The size of phones vocabulary.
 *
 * \param words_vocabulary The sorted string array which contains names of
 * recognized words.
 *
 * \param words_number The size of words vocabulary.
 *
 * \return This function returns root of created words tree (pointer to the
 * TWordsTreeNode structure) in case of successful creating, or it returns
 * NULL in case of error.
 */
PWordsTreeNode create_words_vocabulary_tree(
        char *file_name, char **phones_vocabulary, int phones_number,
        char **words_vocabulary, int words_number);

/*! \fn void free_vocabulary_tree(PWordsTreeNode* root_node)
 *
 * \brief Free memory which was allocated for the given words tree.
 *
 * \param Pointer to root of the deletable words tree.
 */
void free_vocabulary_tree(PWordsTreeNode* root_node);

/*! \fn void free_string_array(char ***string_array, int array_size)
 *
 * \brief This function frees memory which was allocated for the given string
 * array.
 *
 * \param string_array Pointer to the deletable string array.
 *
 * \param array_size Size of the deletable string array.
 */
void free_string_array(char ***string_array, int array_size);

/*! \fn int recognize_words(
 *         PTranscriptionNode source_phones_transcription,
 *         char *phones_vocabulary[], int phones_number,
 *         char *words_vocabulary[], int words_number,
 *         PWordsTreeNode words_tree, TWordBigram bigrams[],int bigrams_number,
 *         PTranscriptionNode *recognized_words)
 *
 * \brief This function recognizes all words in the source phones sequence
 * using vocabularies of phones and words, the bigrams list and the words tree.
 *
 * \param source_phones_transcription The source phones transcription which will
 * be recognized. As result of this recognition the words transcription will be
 * generated.
 *
 * \param phones_vocabulary The sorted string array which represents phones
 * vocabulary.
 *
 * \param phones_number The size of phones vocabulary.
 *
 * \param words_vocabulary The sorted string array which represents words
 * vocabulary.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param words_tree The root of words tree which describes standard phonetic
 * transcriptions of all words.
 *
 * \param bigrams The sorted TWordBigram array which represents bigram model of
 * language.
 *
 * \param bigrams_number The number of bigrams.
 *
 * \param recognized_words The pointer to words transcription which will be
 * created as result of recognition. The memory for this words transcription
 * will be allocated automatically in this function.
 *
 * \return This function returns size of recognized words transcription in case
 * of successful recognizing, or it returns -1 in case of error.
 */
int recognize_words(
        PTranscriptionNode source_phones_transcription,
        char *phones_vocabulary[], int phones_number,
        char *words_vocabulary[], int words_number,
        PWordsTreeNode words_tree, TWordBigram bigrams[], int bigrams_number,
        PTranscriptionNode *recognized_words);

/*! \fn int read_string(FILE *read_file, char *str)
 *
 * \brief This function reads one line from the given text file and trims this
 * line (trimming is deletion of initial and final spaces, tabs and nonprinted
 * characters).
 *
 * \param read_file Handle of text file.
 *
 * \param str Line which will be read. Maximal length of line is 1000
 * characters.
 *
 * \return This function returns length of line which has been read. In case of
 * error this function returns zero.
 */
int read_string(FILE *read_file, char *str);

/*! \fn int find_in_vocabulary(
 *         char *vocabulary[], int vocabulary_size, char *found_name,
 *         int *is_equal)
 *
 * \brief This function finds the specified node (word or phone) in sorted
 * vocabulary. If sought node isn't contained in vocabulary, then function
 * finds the most similar node.
 *
 * \param vocabulary Sorted string array which represents the vocabulary in
 * which search will be realized.
 *
 * \param vocabulary_size Size of sorted string array.
 *
 * \param found_name Vocabulary node which must be found.
 *
 * \param is_equal Pointer to integer flag which defines the search result
 * (1 - exact coincidence, 0 - the most similar).
 *
 * \result This function returns index of found node in vocabulary in case of
 * successful completion of search, or it returns -1 in case of error.
 */
int find_in_vocabulary(char *vocabulary[], int vocabulary_size,
                       char *found_name, int *is_equal);

/*! \fn int string_to_bigram(
 *         char *str, char *words_vocabulary[], int words_number,
 *         TWordBigram *bigram)
 *
 * \brief This function parses source string and converts it to the TWordBigram
 * structure.
 *
 * \param str The source string.
 *
 * \param words_vocabulary The words vocabulary which is sorted by words names.
 *
 * \param words_number Size of words vocabulary.
 *
 * \param Pointer to the TWordBigram structure in which information about
 * bigram will be written.
 *
 * \return If source string correctly describes the bigram, then this function
 * returns 1. Else, this function returns 0.
 */
int string_to_bigram(char *str, char *words_vocabulary[], int words_number,
                     TWordBigram *bigram);

/*! \fn int find_in_bigrams_list(
 *         TWordBigram bigrams[], int bigrams_number, TWordBigram found_bigram,
 *         int *is_equal)
 *
 * \brief This function finds the specified bigram in sorted bigrams list.
 * If sought bigram isn't contained in vocabulary, then function finds the most
 * similar node.
 *
 * \param bigrams Sorted TWordBigram array which represents the bigrams list in
 * which search will be realized.
 *
 * \param bigrams_number Size of the sorted TWordBigram array.
 *
 * \param found_bigram The bigram which must be found.
 *
 * \param is_equal Pointer to integer flag which defines the search result
 * (1 - exact coincidence, 0 - the most similar).
 *
 * \result This function returns index of bigram in the bigrams list in case of
 * successful completion of search, or it returns -1 in case of error.
 */
int find_in_bigrams_list(TWordBigram bigrams[], int bigrams_number,
                         TWordBigram found_bigram, int *is_equal);

#endif // BOND005_LVCSR_LIB_H
