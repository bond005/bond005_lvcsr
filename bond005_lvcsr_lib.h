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

/*! \struct TVocabularyTreeNode
 * \brief Structure for representation of a words tree node. */
typedef struct {
    int node_data; /**< Data of node: index of word of phone. */
    int node_type; /**< Type of node: 0 - initial node, 1 - phone node,
                     * 2 - word node (final node). */
    int number_of_next_nodes;        /**< Number of next nodes. */
    TVocabularyTreeNode *next_nodes; /**< Array of pointers to next nodes. */
} TDVocabularyTreeNode;

/*! \typedef PVocabularyTreeNode
 * \brief Pointer to TVocabularyTreeNode data.
 * \sa TVocabularyTreeNode
 */
typedef TVocabularyTreeNode* PVocabularyTreeNode;

/*! \struct TTranscriptionNode
 * \brief Structure for representation of one transcription node.
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

/*! \fn load_phones_transcriptions(char *mlf_name, char **phones_vocabulary,
 *                                 PTranscriptionNode *transcriptions_of_files,
 *                                 char **names_of_files, int files_number);
 *
 * \brief This function loads list of phones transcriptions and names of their
 * files from the given MLF file. This MLF file must contain not only labels of
 * acoustical events (i.e. names of phones) but also start and end times of these
 * events. Besides, probabilities of these acoustical events can be present at
 * the MLF file.
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param transcriptions_of_files The array of transcriptions into which phones
 * transcriptions will be written. Memory for this array must be allocated
 * before call of this function. If this array is NULL then it will be ignored.
 *
 * \param names_of_files The string array into which names of transcriptions
 * files will be written. Memory for this array must be allocated before call
 * of this function. If this array is NULL then it will be ignored.
 *
 * \param files_number The maximum number of phones transcriptions and
 * corresponding files names which can be loaded from MLF file.
 *
 * \return If the loading has been completed successfully then this function
 * will return number of loaded phones transcriptions and corresponding files
 * names, else this function will return zero.
 */
int load_phones_transcriptions(char *mlf_name, char **phones_vocabulary,
                               PTranscriptionNode *transcriptions_of_files,
                               char **names_of_files, int files_number);

/*! \fn int load_words_transcriptions(
 *         char *mlf_name, char **words_vocabulary,
 *         PTranscriptionNode *transcriptions_of_files, char **names_of_files,
 *         int files_number);
 *
 * \brief This function loads list of words transcriptions and names of their
 * files from the given MLF file. This MLF file must contain only labels of
 * acoustical events (i.e. names of words).
 *
 * \param mlf_name The name of source MLF file.
 *
 * \param transcriptions_of_files The array of transcriptions into which words
 * transcriptions will be written. Memory for this array must be allocated
 * before call of this function. If this array is NULL then it will be ignored.
 *
 * \param names_of_files The string array into which names of transcriptions
 * files will be written. Memory for this array must be allocated before call
 * of this function. If this array is NULL then it will be ignored.
 *
 * \param files_number The maximum number of words transcriptions and
 * corresponding files names which can be loaded from MLF file.
 *
 * \return If the loading has been completed successfully then this functiont
 * will return number of loaded words transcriptions and corresponding files
 * names, else this function will return zero.
 */
int load_words_transcriptions(char *mlf_name, char **words_vocabulary,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number);

/*! \fn int save_words_transcriptions(
 *         char *mlf_name, char **words_vocabulary,
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
int save_words_transcriptions(char *mlf_name, char **words_vocabulary,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number);

/*! \fn int load_phones_vocabulary(char *file_name, char ***phones_vocabulary)
 *
 * \brief This function loads phones vocabulary (i.e. list of phones) from the
 * given text file into the string array.
 *
 * \param file_name The name of text file with phones vocabulary which will be
 * loaded.
 *
 * \param phones_vocabulary String array in which the phones vocabulary will be
 * written. Memory for this string array must be allocated before call of this
 * function. If this array is NULL then it will be ignored.
 *
 * \param phones_number Maximum number of phones which can be loaded.
 *
 * \return This function returns number of phones in case of successful
 * loading, and it returns zero in case of loading error.
 */
int load_phones_vocabulary(char *file_name, char **phones_vocabulary,
                           int phones_number);

/*! \fn int load_words_vocabulary(char *file_name, char ***words_vocabulary)
 *
 * \brief This function loads words vocabulary (i.e. list of words) from the
 * given text file into the string array.
 *
 * \param file_name The name of text file with words vocabulary which will be
 * loaded (this file describes vocabulary words and their transcriptions).
 *
 * \param words_vocabulary String array in which the words vocabulary will be
 * written. Memory for this string array must be allocated before call of this
 * function. If this array is NULL then it will be ignored.
 *
 * \return This function returns number of words in case of successful loading,
 * and it returns zero in case of loading error.
 */
int load_words_vocabulary(char *file_name, char **words_vocabulary,
                          int words_number);

/*! \fn int create_words_vocabulary_tree(
 *         char *file_name, char **phones_vocabulary, char **words_vocabulary,
 *         PVocabularyTreeNode* root_node)
 *
 * \brief This function loads information about words and their transcriptions
 * from the given text file and creates a new words tree on basis on this
 * information.
 *
 * \param file_name The name of text file with words vocabulary which will be
 * loaded (this file describes vocabulary words and their transcriptions).
 *
 * \param phones_vocabulary String array which contains names of recognized
 * phones.
 *
 * \param words_vocabulary String array which contains names of recognized
 * words.
 *
 * \return This function returns root of created words tree (pointer to the
 * TVocabularyTreeNode structure) in case of successful creating, or it returns
 * NULL in case of error.
 */
PVocabularyTreeNode create_words_vocabulary_tree(
        char *file_name, char **phones_vocabulary, char **words_vocabulary);

/*! \fn void free_vocabulary_tree(PVocabularyTreeNode* root_node)
 *
 * \brief Free memory which was allocated for the given words tree.
 *
 * \param Pointer to root of the deletable words tree.
 */
void free_vocabulary_tree(PVocabularyTreeNode* root_node);

/*! \fn void free_vocabulary(char ***vocabulary, int vocabulary_size)
 *
 * \brief Free memory which was allocated for the given vocabulary.
 *
 * \param Pointer to string array which is represented the deletable vocabulary.
 *
 * \param Size of string array which is represented the deletable vocabulary.
 */
void free_vocabulary(char ***vocabulary, int vocabulary_size);

#endif // BOND005_LVCSR_LIB_H
