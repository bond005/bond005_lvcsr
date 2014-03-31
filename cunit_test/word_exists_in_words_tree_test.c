#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "word_exists_in_words_tree_test.h"

static PWordsTreeNode tree_root = NULL;
static int existable_word = 3;
static int nonexistable_word = 7;

static PWordsTreeNode create_tree()
{
    PWordsTreeNode res = malloc(sizeof(TWordsTreeNode));
    PWordsTreeNode cur = NULL;

    res->node_data = -1;
    res->node_type = INIT_NODE;
    res->next_nodes = malloc(3 * sizeof(TWordsTreeNode));
    memset(res->next_nodes, 0, 3 * sizeof(TWordsTreeNode));
    res->number_of_next_nodes = 3;

    // word phonemes: 2->...
    cur = &(res->next_nodes[0]);
    cur->node_data = 2;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(2 * sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, 2 * sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 2;

    // word phonemes: 3->...
    cur = &(res->next_nodes[1]);
    cur->node_data = 3;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(2 * sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, 2 * sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 2;

    // word phonemes: 1->...
    cur = &(res->next_nodes[2]);
    cur->node_data = 1;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word phonemes: 2->5->...
    cur = &(res->next_nodes[0].next_nodes[0]);
    cur->node_data = 5;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word phonemes: 2->5->1...
    cur = &(res->next_nodes[0].next_nodes[0].next_nodes[0]);
    cur->node_data = 1;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word: 1
    // word phonemes: 2->5->1
    cur = &(res->next_nodes[0].next_nodes[0].next_nodes[0].next_nodes[0]);
    cur->node_data = 1;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    // word phonemes: 2->7->...
    cur = &(res->next_nodes[0].next_nodes[1]);
    cur->node_data = 7;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word: 2
    // word phonemes: 2->7
    cur = &(res->next_nodes[0].next_nodes[1].next_nodes[0]);
    cur->node_data = 2;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    // word phonemes: 3->6->...
    cur = &(res->next_nodes[1].next_nodes[0]);
    cur->node_data = 6;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word phonemes: 3->6->2...
    cur = &(res->next_nodes[1].next_nodes[0].next_nodes[0]);
    cur->node_data = 2;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word: 5
    // word phonemes: 3->6->2
    cur = &(res->next_nodes[1].next_nodes[0].next_nodes[0].next_nodes[0]);
    cur->node_data = 5;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    // word phonemes: 3->1->...
    cur = &(res->next_nodes[1].next_nodes[1]);
    cur->node_data = 1;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(2 * sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, 2 * sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 2;

    // word: 4
    // word phonemes: 3->1
    cur = &(res->next_nodes[1].next_nodes[1].next_nodes[0]);
    cur->node_data = 4;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    // word phonemes: 3->1->2->...
    cur = &(res->next_nodes[1].next_nodes[1].next_nodes[1]);
    cur->node_data = 2;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word: 3
    // word phonemes: 3->1->2
    cur = &(res->next_nodes[1].next_nodes[1].next_nodes[1].next_nodes[0]);
    cur->node_data = 3;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    // word phonemes: 1->2...
    cur = &(res->next_nodes[2].next_nodes[0]);
    cur->node_data = 2;
    cur->node_type = PHONEME_NODE;
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word: 6
    // word phonemes: 1->2
    cur = &(res->next_nodes[2].next_nodes[0].next_nodes[0]);
    cur->node_data = 6;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    return res;
}

int prepare_for_testing_of_word_exists_in_words_tree()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for word_exists_in_words_tree()",
                          init_suite_word_exists_in_words_tree,
                          clean_suite_word_exists_in_words_tree);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             word_exists_in_words_tree_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    word_exists_in_words_tree_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    word_exists_in_words_tree_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_word_exists_in_words_tree()
{
    tree_root = create_tree();
    return 0;
}

int clean_suite_word_exists_in_words_tree()
{
    free_words_tree(&tree_root);
    return 0;
}

void word_exists_in_words_tree_valid_test_1()
{
    CU_ASSERT_TRUE_FATAL(word_exists_in_words_tree(existable_word, tree_root));
}

void word_exists_in_words_tree_valid_test_2()
{
    CU_ASSERT_FALSE_FATAL(word_exists_in_words_tree(nonexistable_word,
                                                    tree_root));
}

void word_exists_in_words_tree_invalid_test_1()
{
    CU_ASSERT_FALSE_FATAL(word_exists_in_words_tree(-1, tree_root));
    CU_ASSERT_FALSE_FATAL(word_exists_in_words_tree(existable_word, NULL));
}
