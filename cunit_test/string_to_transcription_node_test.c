#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "string_to_transcription_node_test.h"

#define STR_SIZE 100
#define VOCABULARY_SIZE 5

static char correct_transcription_str1[STR_SIZE];
static char correct_transcription_str2[STR_SIZE];
static char incorrect_transcription_str1[STR_SIZE];
static char incorrect_transcription_str2[STR_SIZE];
static char incorrect_transcription_str3[STR_SIZE];
static char incorrect_transcription_str4[STR_SIZE];
static char *vocabulary[VOCABULARY_SIZE];
static TTranscriptionNode target_node1, target_node2;

int prepare_for_testing_of_string_to_transcription_node()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for string_to_transcription_node()",
                          init_suite_string_to_transcription_node,
                          clean_suite_string_to_transcription_node);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             string_to_transcription_node_valid_test_1))
            || (NULL == CU_add_test(pSuite, "Valid partition 2",
                                    string_to_transcription_node_valid_test_2))
            || (NULL == CU_add_test(pSuite, "Invalid partition 1",
                                    string_to_transcription_node_invalid_test_1))
            || (NULL == CU_add_test(pSuite, "Invalid partition 2",
                                       string_to_transcription_node_invalid_test_2))
            || (NULL == CU_add_test(pSuite, "Invalid partition 3",
                                       string_to_transcription_node_invalid_test_3))
            || (NULL == CU_add_test(pSuite, "Invalid partition 4",
                                       string_to_transcription_node_invalid_test_4))
            || (NULL == CU_add_test(pSuite, "Invalid partition 5",
                                       string_to_transcription_node_invalid_test_5)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_string_to_transcription_node()
{
    int i;

    memset(correct_transcription_str1, 0, STR_SIZE);
    strcpy(correct_transcription_str1, "10 20 b 0.5");

    memset(correct_transcription_str2, 0, STR_SIZE);
    strcpy(correct_transcription_str2, "10 20 b");

    target_node1.start_time = 10;
    target_node1.end_time = 20;
    target_node1.node_data = 1;
    target_node1.probability = 0.5;

    target_node2.start_time = 10;
    target_node2.end_time = 20;
    target_node2.node_data = 1;
    target_node2.probability = 1.0;

    memset(incorrect_transcription_str1, 0, STR_SIZE);
    strcpy(incorrect_transcription_str1, "-10 20 b 0.5");

    memset(incorrect_transcription_str2, 0, STR_SIZE);
    strcpy(incorrect_transcription_str2, "10 5 b 0.5");

    memset(incorrect_transcription_str3, 0, STR_SIZE);
    strcpy(incorrect_transcription_str3, "10 20 h 0.5");

    memset(incorrect_transcription_str4, 0, STR_SIZE);
    strcpy(incorrect_transcription_str4, "10 20 b -0.5");

    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        vocabulary[i] = malloc(STR_SIZE);
        memset(vocabulary[i], 0, STR_SIZE);
        vocabulary[i][0] = (char)((int)'a' + i);
    }

    return 0;
}

int clean_suite_string_to_transcription_node()
{
    int i;
    for (i = 0; i < VOCABULARY_SIZE; i++)
    {
        free(vocabulary[i]);
    }
    return 0;
}

void string_to_transcription_node_valid_test_1()
{
    TTranscriptionNode node;

    CU_ASSERT_TRUE_FATAL(string_to_transcription_node(
                             correct_transcription_str1, vocabulary,
                             VOCABULARY_SIZE, &node));
    CU_ASSERT_EQUAL_FATAL(target_node1.start_time, node.start_time);
    CU_ASSERT_EQUAL_FATAL(target_node1.end_time, node.end_time);
    CU_ASSERT_EQUAL_FATAL(target_node1.node_data, node.node_data);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(target_node1.probability, node.probability,
                                 FLT_EPSILON);
}

void string_to_transcription_node_valid_test_2()
{
    TTranscriptionNode node;

    CU_ASSERT_TRUE_FATAL(string_to_transcription_node(
                             correct_transcription_str2, vocabulary,
                             VOCABULARY_SIZE, &node));
    CU_ASSERT_EQUAL_FATAL(target_node2.start_time, node.start_time);
    CU_ASSERT_EQUAL_FATAL(target_node2.end_time, node.end_time);
    CU_ASSERT_EQUAL_FATAL(target_node2.node_data, node.node_data);
    CU_ASSERT_DOUBLE_EQUAL_FATAL(target_node2.probability, node.probability,
                                 FLT_EPSILON);
}

void string_to_transcription_node_invalid_test_1()
{
    TTranscriptionNode node;

    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              incorrect_transcription_str1, vocabulary,
                              VOCABULARY_SIZE, &node));
}

void string_to_transcription_node_invalid_test_2()
{
    TTranscriptionNode node;

    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              incorrect_transcription_str2, vocabulary,
                              VOCABULARY_SIZE, &node));
}

void string_to_transcription_node_invalid_test_3()
{
    TTranscriptionNode node;

    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              incorrect_transcription_str3, vocabulary,
                              VOCABULARY_SIZE, &node));
}

void string_to_transcription_node_invalid_test_4()
{
    TTranscriptionNode node;

    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              incorrect_transcription_str4, vocabulary,
                              VOCABULARY_SIZE, &node));
}

void string_to_transcription_node_invalid_test_5()
{
    TTranscriptionNode node;

    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              NULL, vocabulary, VOCABULARY_SIZE, &node));
    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              correct_transcription_str1, NULL,
                              VOCABULARY_SIZE, &node));
    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              correct_transcription_str1, vocabulary, 0,
                              &node));
    CU_ASSERT_FALSE_FATAL(string_to_transcription_node(
                              correct_transcription_str1, vocabulary,
                              VOCABULARY_SIZE, NULL));
}
