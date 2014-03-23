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

int load_phones_transcriptions(char *mlf_name, char **phones_vocabulary,
                               PTranscriptionNode *transcriptions_of_files,
                               char **names_of_files, int files_number);

int load_words_transcriptions(char *mlf_name, char **words_vocabulary,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number);

int save_words_transcriptions(char *mlf_name, char **words_vocabulary,
                              PTranscriptionNode *transcriptions_of_files,
                              char **names_of_files, int files_number);

int load_phones_vocabulary(char *file_name, char ***phones_vocabulary);

int create_words_vocabulary(char *file_name, char **phones_vocabulary,
                            char ***words_vocabulary,
                            PVocabularyTreeNode* root_node);

void free_vocabulary(char ***vocabulary);

#endif // BOND005_LVCSR_LIB_H
