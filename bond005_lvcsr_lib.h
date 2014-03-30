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
    INIT_NODE,  /**< The initial node (root node) of the words tree. */
    PHONE_NODE, /**< The phone node of the words tree. */
    WORD_NODE   /**< The word node (final node) of the words tree. */
} TWordsTreeNodeType;

/*! \struct TWordsTreeNode
 * \brief Structure for representation of a words tree node. */
typedef struct _TWordsTreeNode {
    int node_data;                /**< Data of node: index of word of phone. */
    TWordsTreeNodeType node_type; /**< Type of node^ initial node, phone node
                                       or word node (final node). */
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
typedef struct _TTranscriptionNode {
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

/*! \fn int load_phones_MLF(
 *         char *mlf_name, char **phones_vocabulary, int phones_number,
 *         TMLFFilePart **mlf_data);
 *
 * \brief This function loads MLF file describing phones transcriptions of some
 * speech signals. This MLF file must contain not only labels of acoustical
 * events (i.e. names of phones) but also start and end times of these events.
 * Besides, probabilities of these acoustical events can be present at the MLF
 * file.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as prepare_filename(), read_string() and
 * string_to_transcription_node().
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param phones_vocabulary The string array which represents phones vocabulary.
 *
 * \param phones_number The size of phones vocabulary.
 *
 * \param mlf_data Pointer to array of MLF file's parts (one part of MLF file
 * involves name of the some label file and phones transcription containing in
 * this label file). Memory for this array will be allocated automatically in
 * this function.
 *
 * \return If the loading has been completed successfully then this function
 * will return number of loaded parts of MLF file (i.e. size of loaded MLF data
 * array), else this function will return zero.
 *
 * \sa prepare_filename(), read_string(), string_to_transcription_node().
 */
int load_phones_MLF(char *mlf_name, char **phones_vocabulary,int phones_number,
                    TMLFFilePart **mlf_data);

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

/*! \fn int load_phones_vocabulary(char *file_name, char ***phones_vocabulary)
 *
 * \brief This function loads phones vocabulary (i.e. list of phones) from the
 * given text file into the string array. The phones vocabulary is being sorted
 * automatically at loading.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as find_in_vocabulary() and read_string().
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
 *
 * \sa find_in_vocabulary(), read_string().
 */
int load_phones_vocabulary(char *file_name, char ***phones_vocabulary);

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

/*! \fn int load_words_bigrams(
 *         char *file_name, char **words_vocabulary, int words_number,
 *         TWordBigram bigrams[], int bigrams_number)
 *
 * \brief This function loads a bigrams list from the given text file into
 * the TWordBigram array. The bigrams list is being sorted automatically at
 * loading.
 *
 * \details It is basic function of this library. This function uses such
 * additional functions of library as find_in_bigrams_list(), read_string() and
 * string_to_bigram().
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
 *
 * \sa find_in_bigrams_list(), read_string(), string_to_bigram().
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
 * \details It is basic function of this library. This function uses such
 * additional functions of library as add_word_to_words_tree(),
 * find_in_vocabulary(), parse_transcription_str(), read_string()
 * and select_word_and_transcription().
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

/*! \fn int recognize_words(
 *         PTranscriptionNode source_phones_transcription,
 *         PWordsTreeNode words_tree, TWordBigram bigrams[],int bigrams_number,
 *         PTranscriptionNode *recognized_words)
 *
 * \brief This function recognizes all words in the source phones sequence
 * using the words tree and the bigrams list.
 *
 * \param source_phones_transcription The source phones transcription which will
 * be recognized. As result of this recognition the words transcription will be
 * generated.
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
        PWordsTreeNode words_tree, TWordBigram bigrams[], int bigrams_number,
        PTranscriptionNode *recognized_words);

/*! \fn int read_string(FILE *read_file, char *str)
 *
 * \brief This function reads one line from the given text file and trims this
 * line (trimming is deletion of initial and final spaces, tabs and nonprinted
 * characters).
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_phones_MLF(), load_phones_vocabulary(),
 * load_words_bigrams(), load_words_MLF() and load_words_vocabulary().
 *
 * \param read_file Handle of text file.
 *
 * \param str Line which will be read. Maximal length of line is 1000
 * characters. It must not be constant!
 *
 * \return This function returns length of line which has been read. In case of
 * error this function returns zero.
 *
 * \sa load_phones_MLF(), load_phones_vocabulary(), load_words_bigrams(),
 * load_words_MLF(), load_words_vocabulary().
 */
int read_string(FILE *read_file, char *str);

/*! \fn int prepare_filename(char *filename)
 *
 * \brief This function prepares the specified file name for further using.
 * Preparing consists in deletion of initial and final quotes.
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_phones_MLF() and load_words_MLF().
 *
 * \param filename The prepared file name which will be rewritten. It must not
 * be constant!
 *
 * \return In case of successful preparing this function returns new length of
 * prepared file name. In case of error this function returns zero.
 *
 * \sa load_phones_MLF(), load_words_MLF().
 */
int prepare_filename(char *filename);

/*! \fn int find_in_vocabulary(
 *         char *vocabulary[], int vocabulary_size, char *found_name,
 *         int *is_equal)
 *
 * \brief This function finds the specified node (word or phone) in sorted
 * vocabulary. If sought node isn't contained in vocabulary, then function
 * finds the most similar node.
 *
 * \details It is additional function of this library. This function is used in
 * such functions of library as load_phones_vocabulary(), load_words_MLF(),
 * load_words_vocabulary(), string_to_bigram() and
 * string_to_transcription_node().
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
 * \sa load_phones_vocabulary(), load_words_MLF(), load_words_vocabulary(),
 * string_to_bigram(), string_to_transcription_node().
 */
int find_in_vocabulary(char *vocabulary[], int vocabulary_size,
                       char *found_name, int *is_equal);

/*! \fn int string_to_transcription_node(
 *         char *str, char *vocabulary[], int vocabulary_size,
 *         PTranscriptionNode node)
 *
 * \brief This function parses source string and converts it to the
 * TTranscriptionNode structure.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as load_phones_MLF().
 *
 * \param str The source string.
 *
 * \param vocabulary The vocabulary of words or phones which is sorted by items
 * names.
 *
 * \param vocabulary_size Size of vocabulary.
 *
 * \param Pointer to the TTranscriptionNode structure in which information
 * about transcription node will be written.
 *
 * \return If source string correctly describes the transcription node, then
 * this function returns 1. Else, this function returns 0.
 *
 * \sa load_phones_MLF().
 */
int string_to_transcription_node(char *str,
                                 char *vocabulary[], int vocabulary_size,
                                 PTranscriptionNode node);

/*! \fn int string_to_bigram(
 *         char *str, char *words_vocabulary[], int words_number,
 *         TWordBigram *bigram)
 *
 * \brief This function parses source string and converts it to the TWordBigram
 * structure.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as load_words_bigrams().
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
 *
 * \sa load_words_bigrams().
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
 * \details It is additional function of this library. This function is used in
 * such function of library as load_words_bigrams().
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
 *
 * \sa load_words_bigrams().
 */
int find_in_bigrams_list(TWordBigram bigrams[], int bigrams_number,
                         TWordBigram found_bigram, int *is_equal);

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
 *         char *transcription_str, char **phones_vocabulary,int phones_number,
 *         int phones_sequence[])
 *
 * \brief This function parses string description of phonetic transcription and
 * creates the sequence of phones indexes according to the phones vocabulary.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as create_words_vocabulary_tree().
 *
 * \param transcription_str The source string description of phonetic
 * transcription.
 *
 * \param phones_vocabulary The sorted string array which contains names of
 * recognized phones.
 *
 * \param phones_number The size of phones vocabulary.
 *
 * \phones_sequence The integer array into which the sequence of phones indexes
 * will be written. The memory for this array must be allocated before call of
 * this function.
 *
 * \return In case of success this function returns length of created sequence
 * of phones indexes, and in case of error it returns zero.
 *
 * \sa create_words_vocabulary_tree().
 */
int parse_transcription_str(char *transcription_str, char **phones_vocabulary,
                            int phones_number, int phones_sequence[]);

/*! \fn int add_word_to_words_tree(
 *         int word_index, int word_phones[], int word_length,
 *         PWordsTreeNode words_tree_root)
 *
 * \brief This function adds new word to the words tree. Added word is
 * specified by its index in the words vocabulary and its phonetic
 * transcription. By-turn, the phonetic transcription is described by sequence
 * of phones indexes in the phones vocabulary.
 *
 * \details It is additional function of this library. This function is used in
 * such function of library as create_words_vocabulary_tree().
 *
 * \param word_index The index of added word in the words vocabulary.
 *
 * \param word_phones The sequence of phones indexes which describes the
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
int add_word_to_words_tree(int word_index, int word_phones[], int word_length,
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
