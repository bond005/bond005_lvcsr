#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "add_word_to_words_tree_test.h"

static PWordsTreeNode root_of_empty_tree = NULL;
static PWordsTreeNode root_of_uncompleted_tree = NULL;
static PWordsTreeNode root_of_completed_tree = NULL;
static int word_index = 3;
static int word_phonemes[] = { 3, 1, 2 };
static int word_length = 3;

static PWordsTreeNode create_empty_tree()
{
    PWordsTreeNode res = malloc(sizeof(TWordsTreeNode));

    res->node_data = -1;
    res->node_type = INIT_NODE;
    res->next_nodes = NULL;
    res->number_of_next_nodes = 0;

    return res;
}

static PWordsTreeNode create_uncompleted_tree()
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
    cur->next_nodes = malloc(sizeof(TWordsTreeNode));
    memset(cur->next_nodes, 0, sizeof(TWordsTreeNode));
    cur->number_of_next_nodes = 1;

    // word: 4
    // word phonemes: 3->1
    cur = &(res->next_nodes[1].next_nodes[1].next_nodes[0]);
    cur->node_data = 4;
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

static PWordsTreeNode create_completed_tree()
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

static int same_words_trees(PWordsTreeNode words_tree1,
                            PWordsTreeNode words_tree2)
{
    int i, res = 1;
    PWordsTreeNode cur1, cur2;
    if (words_tree1 == words_tree2)
    {
        return 1;
    }
    if ((words_tree1 == NULL) || (words_tree2 == NULL))
    {
        return 0;
    }
    if (words_tree1->node_data != words_tree2->node_data)
    {
        return 0;
    }
    if (words_tree1->node_type != words_tree2->node_type)
    {
        return 0;
    }
    if (words_tree1->number_of_next_nodes != words_tree2->number_of_next_nodes)
    {
        return 0;
    }
    if (words_tree1->number_of_next_nodes < 0)
    {
        return 0;
    }
    if (words_tree1->number_of_next_nodes == 0)
    {
        return 1;
    }
    cur1 = words_tree1->next_nodes;
    cur2 = words_tree2->next_nodes;
    for (i = 0; i < words_tree1->number_of_next_nodes; i++)
    {
        if (!same_words_trees(cur1++, cur2++))
        {
            res = 0;
            break;
        }
    }
    return res;
}

int prepare_for_testing_of_add_word_to_words_tree()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for add_word_to_words_tree()",
                          init_suite_add_word_to_words_tree,
                          clean_suite_add_word_to_words_tree);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             add_word_to_words_tree_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    add_word_to_words_tree_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Valid partition 3",
                                    add_word_to_words_tree_valid_test_3))
            || (NULL == CU_add_test(pSuite, "Invalid partitions",
                                    add_word_to_words_tree_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_add_word_to_words_tree()
{
    root_of_empty_tree = create_empty_tree();
    root_of_uncompleted_tree = create_uncompleted_tree();
    root_of_completed_tree = create_completed_tree();
    return 0;
}

int clean_suite_add_word_to_words_tree()
{
    free_words_tree(&root_of_empty_tree);
    free_words_tree(&root_of_uncompleted_tree);
    free_words_tree(&root_of_completed_tree);
    return 0;
}

void add_word_to_words_tree_valid_test_1()
{
    CU_ASSERT_TRUE_FATAL(add_word_to_words_tree(
                             word_index, word_phonemes, word_length,
                             root_of_empty_tree));
    CU_ASSERT_TRUE_FATAL(word_exists_in_words_tree(
                             word_index, root_of_empty_tree));
}

void add_word_to_words_tree_valid_test_2()
{
    CU_ASSERT_TRUE_FATAL(add_word_to_words_tree(
                             word_index, word_phonemes, word_length,
                             root_of_uncompleted_tree));
    CU_ASSERT_TRUE_FATAL(word_exists_in_words_tree(
                             word_index, root_of_uncompleted_tree));
    CU_ASSERT_TRUE_FATAL(same_words_trees(root_of_completed_tree,
                                          root_of_uncompleted_tree))
}

void add_word_to_words_tree_valid_test_3()
{
    CU_ASSERT_TRUE_FATAL(add_word_to_words_tree(
                             word_index, word_phonemes, word_length,
                             root_of_completed_tree));
    CU_ASSERT_TRUE_FATAL(word_exists_in_words_tree(
                             word_index, root_of_completed_tree));
}

void add_word_to_words_tree_invalid_test_1()
{
    CU_ASSERT_FALSE_FATAL(add_word_to_words_tree(
                              -1, word_phonemes, word_length,
                              root_of_completed_tree));
    CU_ASSERT_FALSE_FATAL(add_word_to_words_tree(
                              word_index, NULL, word_length,
                              root_of_completed_tree));
    CU_ASSERT_FALSE_FATAL(add_word_to_words_tree(
                              word_index, word_phonemes, 0,
                              root_of_completed_tree));
    CU_ASSERT_FALSE_FATAL(add_word_to_words_tree(
                              word_index, word_phonemes, word_length, NULL));
}
