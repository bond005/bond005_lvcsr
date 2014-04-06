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

#include <stdint.h>
#include <stdio.h>
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

/*! \enum TMLFParsingState
 * \brief There are states of the MLF file reading.
 */
enum TMLFParsingState {
    HEADER_EXPECTATION_STATE,/**< The initial state for reading of all MLF file
                                  (such MLF file begins with special header).*/
    FILENAME_READING_STATE,  /**< The initial state for reading of one
                                  transcription in the MLF file (such
                                  transcription always begins with name of file
                                  containing the source acoustical data). */
    EVENT_READING_STATE      /**< The state for reading of acoustic events,
                                  i.e. transcription nodes. */
};

/*! \enum TWordsTreeNodeType
 * \brief There are types of the words tree's nodes.
 */
typedef enum _TWordsTreeNodeType {
    INIT_NODE,   /**< The initial node (root node) of the words tree. */
    PHONEME_NODE,/**< The phoneme node of the words tree. */
    WORD_NODE    /**< The word node (final node) of the words tree. */
} TWordsTreeNodeType;

/*! \struct TWordsTreeNode
 * \brief Structure for representation of a words tree node. */
typedef struct _TWordsTreeNode {
    int node_data;                /**< Data of node: index of word of
                                       phoneme. */
    TWordsTreeNodeType node_type; /**< Type of node^ initial node, phoneme node
                                       or word node (final node). */
    int number_of_next_nodes;          /**< Number of next nodes. */
    struct _TWordsTreeNode *next_nodes;/**< Array of pointers to next nodes. */
} TWordsTreeNode;

/*! \typedef PWordsTreeNode
 * \brief Pointer to TWordsTreeNode data.
 * \sa TWordsTreeNode
 */
typedef TWordsTreeNode* PWordsTreeNode;

/*! \struct TLinearWordsLexicon
 * \brief Structure for representation of one unit of the linear words lexicon.
 * Such unit consists of the word's index and the sequence of phonemes indexes
 * which describes transcription of the given word. The word's index is
 * position of this word in the words vocabulary. The phonemes indexes define
 * positions of corresponding phonemes in the phonemes vocabulary.
 */
typedef struct _TLinearWordsLexicon {
    int word_index;
    int phonemes_number;
    int *phonemes_indexes;
} TLinearWordsLexicon;

/*! \struct TTranscriptionNode
 * \brief Structure for representation of one transcription node.
 *
 * The transcription node describes some acoustic event associated with
 * appearance of word or phoneme in speech. This description involves:
 * - name of acoustic event;
 * - start time of this acoustic event;
 * - stop time of this acoustic event;
 * - event probability.
 */
typedef struct _TTranscriptionNode {
    int node_data;           /**< Data of node: index of word or phoneme. */
    long unsigned start_time;/**< Start time in 100ns units. */
    long unsigned end_time;  /**< End time in 100ns units. */
    float probability;       /**< Probability of this acoustic event. */
} TTranscriptionNode;

/*! \typedef PTranscriptionNode
 * \brief Pointer to TTranscriptionNode data.
 * \sa TTranscriptionNode
 */
typedef TTranscriptionNode* PTranscriptionNode;

/*! \struct TMLFFilePart
 * \brief Structure for representation of one part of the MLF file body. Any
 * MLF file consists of header and body. By-turn, body combines some label
 * files describing speech transcriptions. Thus, one part of MLF file body
 * involves name and contents of one label file.
 */
typedef struct {
    char *name;                      /**< Name of the label file. */
    PTranscriptionNode transcription;/**< Contents of the label file, i.e.
                                          speech transcription. */
    int transcription_size;          /**< Length of speech transcription, i.e.
                                          number of transcribed acoustical
                                          events. */
} TMLFFilePart;

/*! \struct TWordBigram
 * \brief Structure for representation of one words bigram.
 */
typedef struct _TWordBigram {
    int first_word;   /**< Index of first word in bigram. */
    int second_word;  /**< Index of second word in bigram. */
    float probability;/**< Probability of bigram. */
} TWordBigram;

/*! \struct TLanguageModel
 * \brief Structure for representation of the language model.
 */
typedef struct _TLanguageModel {
    int unigrams_number;          /**< Number of unigrams. */
    float *unigrams_probabilities;/**< Probabilities of unigrams. */
    int bigrams_number;           /**< Number of existable bigrams. */
    TWordBigram *bigrams;         /**< Array of existable bigrams. */
} TLanguageModel;

/*! \fn int load_phonemes_MLF(
 *         char *mlf_name, char **phonemes_vocabulary, int phonemes_number,
 *         TMLFFilePart **mlf_data);
 *
 * \brief This function loads MLF file describing phonemes transcriptions of
 * some speech signals. This MLF file must contain not only labels of
 * acoustical events (i.e. names of phonemes) but also start and end times of
 * these events. Besides, probabilities of these acoustical events can be
 * present at the MLF file.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as prepare_filename(), read_string() and
 * string_to_transcription_node().
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param phonemes_vocabulary The string array which represents phonemes
 * vocabulary.
 *
 * \param phonemes_number The size of phonemes vocabulary.
 *
 * \param mlf_data Pointer to array of MLF file's parts (one part of MLF file
 * involves name of the some label file and phonemes transcription containing
 * in this label file). Memory for this array will be allocated automatically
 * in this function.
 *
 * \return If the loading has been completed successfully then this function
 * will return number of loaded parts of MLF file (i.e. size of loaded MLF data
 * array), else this function will return zero.
 *
 * \sa prepare_filename(), read_string(), string_to_transcription_node().
 */
int load_phonemes_MLF(char *mlf_name, char **phonemes_vocabulary,
                      int phonemes_number, TMLFFilePart **mlf_data);

/*! \fn int load_words_MLF(
 *         char *mlf_name, char **words_vocabulary, int words_number,
 *         TMLFFilePart **mlf_data);
 *
 * \brief This function loads MLF file describing words transcriptions of some
 * speech signals. This MLF file must contain only labels of acoustical events
 * (i.e. names of words).
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as find_in_vocabulary(), prepare_filename(),
 * read_string().
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param words_vocabulary The string array which represents words vocabulary.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param mlf_data Pointer to array of MLF file's parts (one part of MLF file
 * involves name of the some label file and words transcription containing in
 * this label file). Memory for this array will be allocated automatically in
 * this function.
 *
 * \return If the loading has been completed successfully then this function
 * will return number of loaded parts of MLF file (i.e. size of loaded MLF data
 * array), else this function will return zero.
 *
 * \sa find_in_vocabulary(), prepare_filename(), read_string().
 */
int load_words_MLF(char *mlf_name, char **words_vocabulary, int words_number,
                   TMLFFilePart **mlf_data);

/*! \fn int save_words_MLF(
 *         char *mlf_name, char **words_vocabulary, int words_number,
 *         TMLFFilePart *mlf_data, int files_number)
 *
 * \brief This function saves the MLF data (names and transcriptions of all
 * label files) into the given MLF file. Only labels of acoustical labels (i.e.
 * names of words) without their start and end times will be written into this
 * MLF file.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param mlf_name The name of MLF file into which list of words transcriptions
 * must be written.
 *
 * \param words_vocabulary The string array which represents words vocabulary.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param mlf_data The TMLFFilePart array representing the MLF data which will
 * be saved. Each item of this array describes the corresponding labels file
 * involving the name and the words transcription.
 *
 * \param files_number The size of TMLFFilePart array, i.e. number of labels
 * files of which the saved MLF file will consist.
 *
 * \return If the saving has been completed successfully, then this function
 * will return number of labels files of which the saved MLF file consists. In
 * case of error this function will return zero.
 */
int save_words_MLF(char *mlf_name, char **words_vocabulary, int words_number,
                   TMLFFilePart *mlf_data, int files_number);

/*! \fn int load_phonemes_vocabulary(char *file_name,
 *         char ***phonemes_vocabulary)
 *
 * \brief This function loads phonemes vocabulary (i.e. list of phonemes) from
 * the given text file into the string array. The phonemes vocabulary isn't
 * being sorted at loading. It will be loaded "as is".
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as find_in_vocabulary() and read_string().
 *
 * \param file_name The name of text file with phonemes vocabulary which will
 * be loaded.
 *
 * \param phonemes_vocabulary Pointer to string array in which the phonemes
 * vocabulary will be written. Memory for this string array will be allocated
 * automatically in this function.
 *
 * \return This function returns number of phonemes in case of successful
 * loading, and it returns zero in case of loading error.
 *
 * \sa read_string().
 */
int load_phonemes_vocabulary(char *file_name, char ***phonemes_vocabulary);

/*! \fn int calculate_confusion_penalties_matrix(
 *         char *confusion_matrix_name, int phonemes_number,
 *         float confusion_penalties_matrix[])
 *
 * \brief This function loads the phonemes confusion matrix from the given file
 * and calculates the matrix of penalties for phonemes confusion on basis of
 * loaded confusion matrix. The confusion matrix using as source data in this
 * function is calculated usually by some phonemes recognizer.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional functions of this library.
 *
 * \param confusion_matrix_name The name of text file containing the phonemes
 * confusion matrix. Loaded confusion matrix must be quadratic matrix of
 * (phonemes_number+1) on (phonemes_number+1) in size. Each row describes how
 * often the corresponding phoneme was confused with other phonemes. Last item
 * of the confusion matrix row describes how often the corresponding phoneme
 * was mistakenly deleted. Last matrix row describes false insertions of
 * phonemes.
 *
 * \param phonemes_number Number of phonemes, i.e size of the phonemes
 * vocabulary.
 *
 * \param confusion_penalties_matrix It is quadratic matrix of phonemes_number
 * on phonemes_number in size. This matrix is unfolded by rows. Each matrix
 * line describes penalties for mixing up the corresponding phoneme with other
 * phonemes.
 *
 * \return This function returns 1 in case of successful work completion, or
 * it returns 0 in case of error.
 */
int calculate_confusion_penalties_matrix(
        char *confusion_matrix_name, int phonemes_number,
        float confusion_penalties_matrix[]);

/*! \fn int load_words_vocabulary(char *file_name, char ***words_vocabulary)
 *
 * \brief This function loads words vocabulary (i.e. list of words) from the
 * given text file into the string array. The words vocabulary is being sorted
 * automatically at loading.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as find_in_vocabulary() and read_string().
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
 *
 * \sa find_in_vocabulary(), read_string().
 */
int load_words_vocabulary(char *file_name, char ***words_vocabulary);

/*! \fn int load_language_model(
 *         char *file_name, int words_number, TLanguageModel *language_model)
 *
 * \brief This function loads a language model from the given binary file into
 * the TLanguageModel struct. Loaded language model consists of unigrams array
 * and bigrams array. The bigrams array is being sorted automatically at
 * loading.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param file_name The name of binary file with language model. First 4 bytes
 * represent integer value which determines number of unigrams, i.e. words.
 * Each next 4 bytes represent float value which determines probability of
 * corresponding unigram. After ending of unigrams array the next 4 bytes
 * represent number of bigrams. Each next 12 bytes represent TWordBigram
 * structure which describes corresponding bigram in the following way:
 * vocabulary index of first word, vocabulary index of second word and bigram
 * probability.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param language_model Pointer to TLanguageModel structure in which the
 * loaded unigrams and bigrams arrays will be written. Memory for these arrays
 * will be allocated automatically in this function.
 *
 * \return This function returns 1 in case of successful loading, and it
 * returns 0 in case of loading error.
 */
int load_language_model(char *file_name, int words_number,
                        TLanguageModel *language_model);

/*! \fn int save_language_model(
 *         char *file_name, TLanguageModel language_model)
 *
 * \brief This function saves a language model into the given binary file.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param file_name The name of binary file with language model. First 4 bytes
 * represent integer value which determines number of unigrams, i.e. words.
 * Each next 4 bytes represent float value which determines probability of
 * corresponding unigram. After ending of unigrams array the next 4 bytes
 * represent number of bigrams. Each next 12 bytes represent TWordBigram
 * structure which describes corresponding bigram in the following way:
 * vocabulary index of first word, vocabulary index of second word and bigram
 * probability.
 *
 * \param language_model The TLanguageModel structure with the saved unigrams
 * and bigrams arrays.
 *
 * \return This function returns 1 in case of successful saving, and it
 * returns 0 in case of saving error.
 */
int save_language_model(char *file_name, TLanguageModel language_model);

/*! \fn int calculate_language_model(
 *         TMLFFilePart *words_mlf_data, int files_number, int words_number,
 *         float eps, TLanguageModel *language_model)
 *
 * \brief This function calculates language model (unigrams and bigrams) on
 * basis of the given MLF data containing words transcriptions. Each words
 * transcription is considered as the separate text. The collection of such
 * texts is used for creation of language model.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * For bigrams smoothing the Deleted Interpolation Smoothing algorithm will be
 * used.
 *
 * \param mlf_data The TMLFFilePart array representing the MLF data which will
 * be used for calculation of language model. Each item of this array describes
 * the corresponding labels file involving the name and the words
 * transcription.
 *
 * \param files_number The size of TMLFFilePart array, i.e. number of labels
 * files of which the MLF file consists.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param eps The bottom threshold of bigram probability. All bigrams whose
 * probabilities is more or less than this threshold will not take part in
 * language model. Value of this threshold must be more or equal 0, and less 1.
 *
 * \param language_model Pointer to the TLanguageModel structure representing
 * the language model which consists of unigrams array and bigrams array. These
 * arrays will be calculated. Memory for these arrays will be allocated
 * automatically in this function.
 *
 * \return This function returns 1 in case of successful canglculati, and it
 * returns 0 in case of error.
 */
int calculate_language_model(TMLFFilePart *words_mlf_data, int files_number,
                             int words_number, float eps,
                             TLanguageModel *language_model);

/*! \fn int create_words_vocabulary_tree(
 *         char *file_name, char **phonemes_vocabulary, int phonemes_number,
 *         char **words_vocabulary, int words_number,
 *         PWordsTreeNode* root_node)
 *
 * \brief This function loads information about words and their transcriptions
 * from the given text file and creates a new words tree on basis on this
 * information.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as add_word_to_words_tree(),
 * find_in_vocabulary(), parse_transcription_str(), read_string()
 * and select_word_and_transcription().
 *
 * \param file_name The name of text file with words vocabulary which will be
 * loaded (this file describes vocabulary words and their transcriptions).
 *
 * \param phonemes_vocabulary The sorted string array which contains names of
 * recognized phonemes.
 *
 * \param phonemes_number The size of phonemes vocabulary.
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
        char *file_name, char **phonemes_vocabulary, int phonemes_number,
        char **words_vocabulary, int words_number);

/*! \fn int word_exists_in_words_tree(int word_index,
 *         PWordsTreeNode words_tree_root)
 *
 * \brief This function checks existence of the given word in the given words
 * tree.
 *
 * \details It is basic function of this library.
 *
 * \param word_index The index of checked word in the words vocabulary.
 *
 * \param words_tree_root The root of words tree in which the specified word
 * will be found.
 *
 * \return This function returns 1 in case of existence of checked word, and it
 * returns 0 in case of nonexistence of checked word or any error.
 */
int word_exists_in_words_tree(int word_index, PWordsTreeNode words_tree_root);

/*! \fn int create_linear_words_lexicon(
 *         char *file_name, char **phonemes_vocabulary, int phonemes_number,
 *         char **words_vocabulary, int words_number,
 *         TLinearWordsLexicon **linear_words_lexicon)
 *
 * \brief This function loads information about words and their transcriptions
 * from the given text file and creates a new linear words lexicon on basis on
 * this information.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as find_in_vocabulary(),
 * parse_transcription_str(), read_string() and
 * select_word_and_transcription().
 *
 * \param file_name The name of text file with words vocabulary which will be
 * loaded (this file describes vocabulary words and their transcriptions).
 *
 * \param phonemes_vocabulary The sorted string array which contains names of
 * recognized phonemes.
 *
 * \param phonemes_number The size of phonemes vocabulary.
 *
 * \param words_vocabulary The sorted string array which contains names of
 * recognized words.
 *
 * \param words_number The size of words vocabulary.
 *
 * \param linear_words_lexicon Pointer to the TLinearWordsLexicon array
 * defining the linear words lexicon which will be created. Memory for this
 * array will be allocated automatically in this function.
 *
 * \return This function returns size of created linear words lexicon in case
 * of successful creating, or it returns 0 in case of error.
 */
int create_linear_words_lexicon(
        char *file_name, char **phonemes_vocabulary, int phonemes_number,
        char **words_vocabulary, int words_number,
        TLinearWordsLexicon **linear_words_lexicon);

/*! \fn void free_MLF(TMLFFilePart **mlf_data, int number_of_MLF_parts)
 *
 * \brief This function frees array representing the data which was loaded from
 * some MLF file.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param mlf_data Pointer to array of MLF file's parts. This array will be
 * freed and zeroized. Also, all transcriptions included in deletable parts of
 * MLF file will be freed too.
 *
 * \param number_of_MLF_parts The number of MLF file's parts, i.e. size of
 * deletable TMLFFilePart array.
 */
void free_MLF(TMLFFilePart **mlf_data, int number_of_MLF_parts);

/*! \fn void free_words_tree(PWordsTreeNode* root_node)
 *
 * \brief Free memory which was allocated for the given words tree.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param Pointer to root of the deletable words tree.
 */
void free_words_tree(PWordsTreeNode* root_node);

/*! \fn void free_linear_words_lexicon(
 *         TLinearWordsLexicon **words_lexicon, int lexicon_size)
 *
 * \brief Free memory which was allowed for the given linear words lexicon.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param words_lexicon Pointer to the TLinearWordsLexicon array defining the
 * linear words lexicon which will be deleted.
 *
 * \param lexicon_size Length of the deletable TLinearWordsLexicon array.
 */
void free_linear_words_lexicon(TLinearWordsLexicon **words_lexicon,
                               int lexicon_size);

/*! \fn void free_string_array(char ***string_array, int array_size)
 *
 * \brief This function frees memory which was allocated for the given string
 * array.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param string_array Pointer to the deletable string array.
 *
 * \param array_size Size of the deletable string array.
 */
void free_string_array(char ***string_array, int array_size);

/*! \fn void free_language_model(TLanguageModel *language_model)
 *
 * \brief This function frees memory which was allocated for the given language
 * model, in particular, unigrams and bigrams arrays.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param language_model Pointer to the TLanguageModel structure which
 * represents the deletable language model.
 */
void free_language_model(TLanguageModel *language_model);

/*! \fn float get_bigram_probability(
 *         TLanguageModel language_model, int start_word_ind, int end_word_ind)
 *
 * \brief This function gets the bigram probability on basis of vocabulary's
 * indexes of first and second word in this bigram.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param language_model The TLanguageModel structure which represents the
 * language model consisting of the unigrams array and the bigrams array. The
 * bigrams array must be ordered by increase of vocabulary's indexes.
 *
 * \param start_word_ind The vocabulary's index of first word in the bigram.
 *
 * \param end_word_ind The vocabulary's index of second word in the bigram.
 *
 * \return This function returns the bigram probability in case of the
 * corresponding bigram's existence, or it returns 0.0 in case of this bigram's
 * nonexistence.
 */
float get_bigram_probability(TLanguageModel language_model, int start_word_ind,
                             int end_word_ind);

/*! \fn int recognize_words(
 *         TMLFFilePart *source_phonemes_MLF, int number_of_MLF_files,
 *         int phonemes_vocabulary_size, float confusion_weights_matrix[],
 *         int words_vocabulary_size, TLinearWordsLexicon words_lexicon[],
 *         TLanguageModel language_model, float lambda,
 *         TMLFFilePart **result_words_MLF)
 *
 * \brief This function recognizes all words which are represented in source
 * sequences of phonemes. The recognition process is based on the linear words
 * lexicon, the matrix of a priori phonemes probabilities, and the bigram
 * language model.
 *
 * \details It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param source_phonemes_MLF The array of parts of the source MLF file. One
 * part of the MLF file involves name of the some label file and phonemes
 * transcription containing in this label file. Phonemes transcriptions which
 * are represented in corresponding parts of source MLF file are input data for
 * the words recognition system.
 *
 * \param files_number The size of TMLFFilePart array, i.e. number of labels
 * files of which the source MLF file consists.
 *
 * \param phonemes_vocabulary_size The size of phonemes vocabulary.
 *
 * \param confusion_penalties_matrix It is quadratic matrix of phonemes_number on
 * phonemes_number in size. This matrix is unfolded by rows. Each matrix line
 * describes penalties for mixing up the corresponding phoneme with other
 * phonemes.
 *
 * \param words_vocabulary_size The size of words vocabulary.
 *
 * \param words_lexicon The TLinearWordsLexicon array describing the used words
 * lexicon. Each node of words lexicon defines the word's index and word's
 * phonetic transcription.
 *
 * \param language_model The language model which is used for recognition.
 *
 * \param lambda The coefficient of language model smoothing which is used in
 * the deleted interpolation smoothing algorithm. Value of this coefficient
 * must be more or equal 0, and less or equal 1.
 *
 * \param result_words_MLF It is pointer to array of parts of the words MLF
 * file. Each part of the words MLF file involves name of the some label file
 * and words transcription which is contained in this label file. In this
 * context the label file name is equal to the label file name in corresponding
 * part of source MLF file (i.e. phonemes MLF file), and words transcription is
 * recognition result of the phonemes sequence which is represented in the
 * corresponding part of source MLF file. Memory for the array of parts of the
 * words MLF file will be allocated automatically in this function. Numbers of
 * labels files of which the result MLF file and source MLF file consist are
 * equal.
 *
 * \return If the recognition process completes successfully, then this
 * function returns 1. In other cases this function returns 0.
 */
int recognize_words(
        TMLFFilePart *source_phonemes_MLF, int number_of_MLF_files,
        int phonemes_vocabulary_size, float confusion_penalties_matrix[],
        int words_vocabulary_size, TLinearWordsLexicon words_lexicon[],
        TLanguageModel language_model, float lambda,
        TMLFFilePart **result_words_MLF);

/*! \fn float estimate_error_rate(
 *         TMLFFilePart recognized_MLF[], TMLFFilePart correct_MLF[],
 *         int files_number, int *insertions,int *deletions,int *substitutions)
 *
 * \brief This function estimates the word recognition error rate (or phoneme
 * recognition error rate) by means of comparing recognized sequence with
 * correct sequence.
 *
 * \details This function calculates the minimal number of insertions,
 * deletions and substitutions which are executed for adduction of the
 * recognized sequence of words to the correct sequence of words (not only
 * words, but also phonemes may be used as elements of compared sequences).
 *
 * Herein the insertions number is interpreted as number of extra words which
 * were added in the recognized sequence, the deletions number is interpreted
 * as number of correct words which were omitted in the recognized sequence,
 * and the substitutions number is interpreted as number of incorrect words
 * which were substituted for the correct words.
 *
 * After calculating number of insertions, deletions and substitutions this
 * function estimates the word error rate using above mentioned computed data.
 *
 * It is basic function of this library. This function doesn't use any
 * additional function of this library.
 *
 * \param recognized_MLF The TMLFFilePart array representing the recognized
 * sequences of words or phonemes in the form of MLF data. Each item of this
 * array describes the corresponding labels file involving the labels file name
 * and the recognized words transcription (or phonemes transcription).
 *
 * \param correct_MLF The TMLFFilePart array representing the correct sequences
 * of words or phonemes in the form of MLF data. Each item of this array
 * describes the corresponding labels file involving the labels file name and
 * the correct words transcription (or phonemes transcription).
 *
 * \param files_number The size of both TMLFFilePart arrays, i.e. number of
 * labels files of which the each MLF file consists (both MLF files consist of
 * same number of the labels files).
 *
 * \param insertions Pointer to the integer value into which the computed
 * insertions number will be saved. This pointer may be NULL.
 *
 * \param deletions Pointer to the integer value into which the computed
 * deletions number will be saved. This pointer may be NULL.
 *
 * \param substitutions Pointer to the integer value into which the computed
 * substitutions number will be saved. This pointer may be NULL.
 *
 * \return This function returns computed word recognition error rate (or
 * phoneme recognition error rate) in percentage terms. In case of error (for
 * example, values of some arguments are incorrect) this function returns -1.0.
 */
float estimate_error_rate(
        TMLFFilePart recognized_MLF[], TMLFFilePart correct_MLF[],
        int files_number, int *insertions, int *deletions, int *substitutions);

/*! \fn int read_string(FILE *read_file, char *str)
 *
 * \brief This function reads one line from the given text file and trims this
 * line (trimming is deletion of initial and final spaces, tabs and nonprinted
 * characters).
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_phonemes_MLF(),load_phonemes_vocabulary(),
 * load_words_MLF() and load_words_vocabulary().
 *
 * \param read_file Handle of text file.
 *
 * \param str Line which will be read. Maximal length of line is 1000
 * characters. It must not be constant!
 *
 * \return This function returns length of line which has been read. In case of
 * error this function returns zero.
 *
 * \sa load_phonemes_MLF(), load_phonemes_vocabulary(), load_words_MLF(),
 * load_words_vocabulary().
 */
int read_string(FILE *read_file, char *str);

/*! \fn int prepare_filename(char *filename)
 *
 * \brief This function prepares the specified file name for further using.
 * Preparing consists in deletion of initial and final quotes.
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_phonemes_MLF() and load_words_MLF().
 *
 * \param filename The prepared file name which will be rewritten. It must not
 * be constant!
 *
 * \return In case of successful preparing this function returns new length of
 * prepared file name. In case of error this function returns zero.
 *
 * \sa load_phonemes_MLF(), load_words_MLF().
 */
int prepare_filename(char *filename);

/*! \fn int find_in_vocabulary(
 *         char *vocabulary[], int vocabulary_size, char *found_name,
 *         int *is_equal)
 *
 * \brief This function finds the specified node (word or phoneme) in sorted
 * vocabulary. If sought node isn't contained in vocabulary, then function
 * finds the most similar node.
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_words_MLF() and load_words_vocabulary().
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
 *
 * \sa load_words_MLF(), load_words_vocabulary().
 */
int find_in_vocabulary(char *vocabulary[], int vocabulary_size,
                       char *found_name, int *is_equal);

/*! \fn int find_in_unsorted_vocabulary(
 *         char *vocabulary[], int vocabulary_size, char *found_name)
 *
 * \brief This function finds the specified node (word or phoneme) in unsorted
 * vocabulary.
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_phonemes_MLF(), load_phonemes_vocabulary()
 * and string_to_transcription_node().
 *
 * \param vocabulary Unsorted string array which represents the vocabulary in
 * which search will be realized.
 *
 * \param vocabulary_size Size of unsorted string array.
 *
 * \param found_name Vocabulary node which must be found.
 *
 * \result This function returns index of found node in vocabulary in case of
 * successful completion of search, or it returns -1 in case of error (for
 * example, sought node isn't contained in vocabulary).
 *
 * \sa load_phonemes_MLF(), load_phonemes_vocabulary(),
 * string_to_transcription_node().
 */
int find_in_unsorted_vocabulary(char *vocabulary[], int vocabulary_size,
                                char *found_name);

/*! \fn int string_to_transcription_node(
 *         char *str, char *phonemes_vocabulary[], int phonemes_number,
 *         PTranscriptionNode node)
 *
 * \brief This function parses source string and converts it to the
 * TTranscriptionNode structure.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as load_phonemes_MLF().
 *
 * \param str The source string.
 *
 * \param phonemes_vocabulary The vocabulary of phonemes.
 *
 * \param vocabulary_size Size of vocabulary.
 *
 * \param Pointer to the TTranscriptionNode structure in which information
 * about transcription node will be written.
 *
 * \return If source string correctly describes the transcription node, then
 * this function returns 1. Else, this function returns 0.
 *
 * \sa load_phonemes_MLF().
 */
int string_to_transcription_node(char *str, char *phonemes_vocabulary[],
                                 int phonemes_number, PTranscriptionNode node);

/*! \fn int select_word_and_transcription(
 *         char *str, char **word_substr, char **transcription_substr)
 *
 * \brief This function selects substring with word name and substring with
 * word transcription on basis of description which is represented by the
 * source string. In source string the word name and the word transcription are
 * separated by sign of equality.
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as create_words_vocabulary_tree() and
 * load_words_vocabulary().
 *
 * \param str The source string. It must not be constant!
 *
 * \param word_substr Pointer to the substring into which the word name will be
 * written.
 *
 * \param transcription_substr Pointer to the substring into which the word
 * transcription will be written.
 *
 * \return This function returns 1 in case of successful selection, and it
 * returns 0 in case of error.
 *
 * \sa create_words_vocabulary_tree(), load_words_vocabulary().
 */
int select_word_and_transcription(char *str, char **word_substr,
                                  char **transcription_substr);

/*! \fn int parse_transcription_str(
 *         char *transcription_str, char **phonemes_vocabulary,
 *         int phonemes_number, int phonemes_sequence[])
 *
 * \brief This function parses string description of phonetic transcription and
 * creates the sequence of phonemes indexes according to the phonemes
 * vocabulary.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as create_words_vocabulary_tree().
 *
 * \param transcription_str The source string description of phonetic
 * transcription.
 *
 * \param phonemes_vocabulary The unsorted string array which contains names of
 * recognized phonemes.
 *
 * \param phonemes_number The size of phonemes vocabulary.
 *
 * \phonemes_sequence The integer array into which the sequence of phonemes
 * indexes will be written. The memory for this array must be allocated before
 * call of this function.
 *
 * \return In case of success this function returns length of created sequence
 * of phonemes indexes, and in case of error it returns zero.
 *
 * \sa create_words_vocabulary_tree().
 */
int parse_transcription_str(char *transcription_str,char **phonemes_vocabulary,
                            int phonemes_number, int phonemes_sequence[]);

/*! \fn int add_word_to_words_tree(
 *         int word_index, int word_phonemes[], int word_length,
 *         PWordsTreeNode words_tree_root)
 *
 * \brief This function adds new word to the words tree. Added word is
 * specified by its index in the words vocabulary and its phonetic
 * transcription. By-turn, the phonetic transcription is described by sequence
 * of phonemes indexes in the phonemes vocabulary.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as create_words_vocabulary_tree().
 *
 * \param word_index The index of added word in the words vocabulary.
 *
 * \param word_phonemes The sequence of phonemes indexes which describes the
 * phonetic transcription of added word.
 *
 * \param word_length The length of phonetic transcription of added word.
 *
 * \param words_tree_root The root of words tree into which the new word will
 * be added.
 *
 * \return This function returns 1 in case of successful word adding, and it
 * returns 0 in case of error.
 *
 * \sa create_words_vocabulary_tree().
 */
int add_word_to_words_tree(int word_index, int word_phonemes[],int word_length,
                           PWordsTreeNode words_tree_root);

/*! \fn void free_words_tree_node(PWordsTreeNode deleted_node)
 *
 * \brief Free memory which was allocated for the given words tree node.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as free_words_tree().
 *
 * \param The deletable words tree node.
 */
void free_words_tree_node(PWordsTreeNode deleted_node);

#endif // BOND005_LVCSR_LIB_H
