#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "create_words_vocabulary_tree_test.h"

static char *correct_words_vocabulary_name = "correct_words_vocabulary.txt";
static char *incorrect_words_vocabulary_name ="incorrect_words_vocabulary.txt";
static PWordsTreeNode target_tree_root = NULL;
static int phonemes_number = 8;
static char *phonemes_vocabulary[8];
static int words_number = 6;
static char *words_vocabulary[6];
static char *words_transcriptions[6];

static PWordsTreeNode create_target_tree()
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

    // word: 0
    // word phonemes: 2->5->1
    cur = &(res->next_nodes[0].next_nodes[0].next_nodes[0].next_nodes[0]);
    cur->node_data = 0;
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

    // word: 1
    // word phonemes: 2->7
    cur = &(res->next_nodes[0].next_nodes[1].next_nodes[0]);
    cur->node_data = 1;
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

    // word: 4
    // word phonemes: 3->6->2
    cur = &(res->next_nodes[1].next_nodes[0].next_nodes[0].next_nodes[0]);
    cur->node_data = 4;
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

    // word: 3
    // word phonemes: 3->1
    cur = &(res->next_nodes[1].next_nodes[1].next_nodes[0]);
    cur->node_data = 3;
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

    // word: 2
    // word phonemes: 3->1->2
    cur = &(res->next_nodes[1].next_nodes[1].next_nodes[1].next_nodes[0]);
    cur->node_data = 2;
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

    // word: 5
    // word phonemes: 1->2
    cur = &(res->next_nodes[2].next_nodes[0].next_nodes[0]);
    cur->node_data = 5;
    cur->node_type = WORD_NODE;
    cur->next_nodes = NULL;
    cur->number_of_next_nodes = 0;

    return res;
}

static void create_phonemes_vocabulary()
{
    int i;
    for (i = 0; i < phonemes_number; i++)
    {
        phonemes_vocabulary[i] = malloc(2 * sizeof(char));
        phonemes_vocabulary[i][0] = (char)((int)'a' + i);
        phonemes_vocabulary[i][1] = 0;
    }
}

static void create_words_vocabulary()
{
    int i;
    for (i = 0; i < words_number; i++)
    {
        words_vocabulary[i] = malloc(4 * sizeof(char));
        words_vocabulary[i][0] = (char)((int)'a' + i);
        words_vocabulary[i][1] = words_vocabulary[i][0];
        words_vocabulary[i][2] = words_vocabulary[i][1];
        words_vocabulary[i][3] = 0;
    }
}

static void create_words_transcriptions()
{
    // word: 0
    // word phonemes: 2->5->1
    words_transcriptions[0] = malloc(3 * 2 * sizeof(char));
    words_transcriptions[0][0] = phonemes_vocabulary[2][0];
    words_transcriptions[0][1] = ' ';
    words_transcriptions[0][2] = phonemes_vocabulary[5][0];
    words_transcriptions[0][3] = ' ';
    words_transcriptions[0][4] = phonemes_vocabulary[1][0];
    words_transcriptions[0][5] = 0;

    // word: 1
    // word phonemes: 2->7
    words_transcriptions[1] = malloc(2 * 2 * sizeof(char));
    words_transcriptions[1][0] = phonemes_vocabulary[2][0];
    words_transcriptions[1][1] = ' ';
    words_transcriptions[1][2] = phonemes_vocabulary[7][0];
    words_transcriptions[1][3] = 0;

    // word: 4
    // word phonemes: 3->6->2
    words_transcriptions[4] = malloc(3 * 2 * sizeof(char));
    words_transcriptions[4][0] = phonemes_vocabulary[3][0];
    words_transcriptions[4][1] = ' ';
    words_transcriptions[4][2] = phonemes_vocabulary[6][0];
    words_transcriptions[4][3] = ' ';
    words_transcriptions[4][4] = phonemes_vocabulary[2][0];
    words_transcriptions[4][5] = 0;

    // word: 3
    // word phonemes: 3->1
    words_transcriptions[3] = malloc(2 * 2 * sizeof(char));
    words_transcriptions[3][0] = phonemes_vocabulary[3][0];
    words_transcriptions[3][1] = ' ';
    words_transcriptions[3][2] = phonemes_vocabulary[1][0];
    words_transcriptions[3][3] = 0;

    // word: 2
    // word phonemes: 3->1->2
    words_transcriptions[2] = malloc(3 * 2 * sizeof(char));
    words_transcriptions[2][0] = phonemes_vocabulary[3][0];
    words_transcriptions[2][1] = ' ';
    words_transcriptions[2][2] = phonemes_vocabulary[1][0];
    words_transcriptions[2][3] = ' ';
    words_transcriptions[2][4] = phonemes_vocabulary[2][0];
    words_transcriptions[2][5] = 0;

    // word: 5
    // word phonemes: 1->2
    words_transcriptions[5] = malloc(2 * 2 * sizeof(char));
    words_transcriptions[5][0] = phonemes_vocabulary[1][0];
    words_transcriptions[5][1] = ' ';
    words_transcriptions[5][2] = phonemes_vocabulary[2][0];
    words_transcriptions[5][3] = 0;
}

static int save_correct_words_vocabulary_for_testing()
{
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(correct_words_vocabulary_name, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }

    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[0],
                words_transcriptions[0]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[1],
                words_transcriptions[1]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[4],
                words_transcriptions[4]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[3],
                words_transcriptions[3]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[2],
                words_transcriptions[2]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[5],
                words_transcriptions[5]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }

    fclose(vocabulary_file);
    return 1;
}

static int save_incorrect_words_vocabulary_for_testing()
{
    FILE *vocabulary_file = NULL;

    vocabulary_file = fopen(incorrect_words_vocabulary_name, "w");
    if (vocabulary_file == NULL)
    {
        return 0;
    }

    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[0],
                words_transcriptions[0]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[1], "c o r") <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[4],
                words_transcriptions[4]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[3],
                words_transcriptions[3]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[2],
                words_transcriptions[2]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }
    if (fprintf(vocabulary_file, "%s=%s\n", words_vocabulary[5],
                words_transcriptions[5]) <= 0)
    {
        fclose(vocabulary_file);
        return 0;
    }

    fclose(vocabulary_file);
    return 1;
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

int prepare_for_testing_of_create_words_vocabulary_tree()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for create_words_vocabulary_tree()",
                          init_suite_create_words_vocabulary_tree,
                          clean_suite_create_words_vocabulary_tree);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             create_words_vocabulary_tree_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    create_words_vocabulary_tree_valid_test_2))
            || (NULL == CU_add_test(
                    pSuite, "Invalid partitions",
                    create_words_vocabulary_tree_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_create_words_vocabulary_tree()
{
    target_tree_root = create_target_tree();
    create_phonemes_vocabulary();
    create_words_vocabulary();
    create_words_transcriptions();
    if (!save_correct_words_vocabulary_for_testing())
    {
        return -1;
    }
    if (!save_incorrect_words_vocabulary_for_testing())
    {
        return -1;
    }
    return 0;
}

int clean_suite_create_words_vocabulary_tree()
{
    int i;
    free_words_tree(&target_tree_root);
    for (i = 0; i < phonemes_number; i++)
    {
        if (phonemes_vocabulary[i] != NULL)
        {
            free(phonemes_vocabulary[i]);
            phonemes_vocabulary[i] = NULL;
        }
    }
    for (i = 0; i < words_number; i++)
    {
        if (words_vocabulary[i] != NULL)
        {
            free(words_vocabulary[i]);
            words_vocabulary[i] = NULL;
        }
        if (words_transcriptions[i] != NULL)
        {
            free(words_transcriptions[i]);
            words_transcriptions[i] = NULL;
        }
    }
    remove(correct_words_vocabulary_name);
    remove(incorrect_words_vocabulary_name);
    return 0;
}

void create_words_vocabulary_tree_valid_test_1()
{
    PWordsTreeNode created_tree_root = NULL;
    int tree_is_null = 1, tree_is_correct = 0;

    created_tree_root = create_words_vocabulary_tree(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, words_number);
    if (created_tree_root != NULL)
    {
        tree_is_null = 0;
        if (same_words_trees(target_tree_root, created_tree_root))
        {
            tree_is_correct = 1;
        }
    }
    free_words_tree(&created_tree_root);

    CU_ASSERT_FALSE_FATAL(tree_is_null);
    CU_ASSERT_TRUE_FATAL(tree_is_correct);
}

void create_words_vocabulary_tree_valid_test_2()
{
    PWordsTreeNode created_tree_root = NULL;
    int tree_is_null = 1;

    created_tree_root = create_words_vocabulary_tree(
                incorrect_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, words_number);
    tree_is_null = (created_tree_root == NULL);
    free_words_tree(&created_tree_root);
    CU_ASSERT_TRUE_FATAL(tree_is_null);
}

void create_words_vocabulary_tree_invalid_test_1()
{
    PWordsTreeNode created_tree_root = NULL;
    int tree_is_null = 1;

    created_tree_root = create_words_vocabulary_tree(
                NULL, phonemes_vocabulary, phonemes_number,
                words_vocabulary, words_number);
    tree_is_null = (created_tree_root == NULL);
    free_words_tree(&created_tree_root);
    CU_ASSERT_TRUE_FATAL(tree_is_null);

    created_tree_root = create_words_vocabulary_tree(
                correct_words_vocabulary_name, NULL, phonemes_number,
                words_vocabulary, words_number);
    tree_is_null = (created_tree_root == NULL);
    free_words_tree(&created_tree_root);
    CU_ASSERT_TRUE_FATAL(tree_is_null);

    created_tree_root = create_words_vocabulary_tree(
                correct_words_vocabulary_name, phonemes_vocabulary, 0,
                words_vocabulary, words_number);
    tree_is_null = (created_tree_root == NULL);
    free_words_tree(&created_tree_root);
    CU_ASSERT_TRUE_FATAL(tree_is_null);

    created_tree_root = create_words_vocabulary_tree(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, NULL, words_number);
    tree_is_null = (created_tree_root == NULL);
    free_words_tree(&created_tree_root);
    CU_ASSERT_TRUE_FATAL(tree_is_null);

    created_tree_root = create_words_vocabulary_tree(
                correct_words_vocabulary_name, phonemes_vocabulary,
                phonemes_number, words_vocabulary, 0);
    tree_is_null = (created_tree_root == NULL);
    free_words_tree(&created_tree_root);
    CU_ASSERT_TRUE_FATAL(tree_is_null);
}
