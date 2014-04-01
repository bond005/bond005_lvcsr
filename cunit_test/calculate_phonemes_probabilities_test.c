#include <float.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../bond005_lvcsr_lib.h"
#include "calculate_phonemes_probabilities_test.h"

#define PHONEMES_NUMBER 3

static char *confusion_matrix_name = "confusion_matrix_for_testing.txt";
static int target_confusion_matrix[(PHONEMES_NUMBER+1)*(PHONEMES_NUMBER+1)] =
{ 10, 3, 2, 3, 1, 7, 3, 4, 2, 0, 12, 3, 1, 2, 1, 1 };
static float target_probabilities_matrx[PHONEMES_NUMBER*PHONEMES_NUMBER] =
{
    10.0 / (10.0+3.0+2.0), 3.0 / (10.0+3.0+2.0), 2.0 / (10.0+3.0+2.0),
    1.0 / (1.0+7.0+3.0), 7.0 / (1.0+7.0+3.0), 3.0 / (1.0+7.0+3.0),
    2.0 / (2.0+0.0+12.0), 0.0 / (2.0+0.0+12.0), 12.0 / (2.0+0.0+12.0)
};

static int save_confusion_matrix_for_testing()
{
    int i, j, res = 1;
    FILE *matrix_file = NULL;

    matrix_file = fopen(confusion_matrix_name, "w");
    if (matrix_file == NULL)
    {
        return 0;
    }

    for (i = 0; i <= PHONEMES_NUMBER; i++)
    {
        for (j = 0; j <= PHONEMES_NUMBER; j++)
        {
            if (fprintf(matrix_file, "%d ",
                        target_confusion_matrix[i*(PHONEMES_NUMBER+1)+j]) <= 0)
            {
                res = 0;
                break;
            }
        }
        if (!res)
        {
            break;
        }
        if (fprintf(matrix_file, "\n") <= 0)
        {
            res = 0;
            break;
        }
    }
    fclose(matrix_file);

    return res;
}

static int compare_float_arrays(float a1[], float a2[], int n)
{
    int i, res = 1;

    for (i = 0; i < n; i++)
    {
        if (fabs(a1[i]-a2[i]) > FLT_EPSILON)
        {
            res = 0;
            break;
        }
    }

    return res;
}

int prepare_for_testing_of_calculate_phonemes_probabilities()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("Test suite for calculate_phonemes_probabilities()",
                          init_suite_calculate_phonemes_probabilities,
                          clean_suite_calculate_phonemes_probabilities);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return 0;
    }

    if ((NULL == CU_add_test(pSuite, "Valid partition 1",
                             calculate_phonemes_probabilities_valid_test_1))
            || (NULL == CU_add_test(
                    pSuite, "Invalid partitions",
                    calculate_phonemes_probabilities_invalid_test_1)))
    {
        CU_cleanup_registry();
        return 0;
    }

    return 1;
}

int init_suite_calculate_phonemes_probabilities()
{
    return (save_confusion_matrix_for_testing() ? 0 : -1);
}

int clean_suite_calculate_phonemes_probabilities()
{
    return 0;
}

void calculate_phonemes_probabilities_valid_test_1()
{
    float probabilities_matrix[PHONEMES_NUMBER * PHONEMES_NUMBER];

    CU_ASSERT_TRUE_FATAL(calculate_phonemes_probabilities(
                             confusion_matrix_name, PHONEMES_NUMBER,
                             probabilities_matrix));
    CU_ASSERT_TRUE_FATAL(compare_float_arrays(probabilities_matrix,
                                              target_probabilities_matrx,
                                              PHONEMES_NUMBER * PHONEMES_NUMBER));
}

void calculate_phonemes_probabilities_invalid_test_1()
{
    float probabilities_matrix[PHONEMES_NUMBER * PHONEMES_NUMBER];

    CU_ASSERT_FALSE_FATAL(calculate_phonemes_probabilities(
                              NULL, PHONEMES_NUMBER,
                              probabilities_matrix));
    CU_ASSERT_FALSE_FATAL(calculate_phonemes_probabilities(
                              confusion_matrix_name, 0,
                              probabilities_matrix));
    CU_ASSERT_FALSE_FATAL(calculate_phonemes_probabilities(
                              confusion_matrix_name, PHONEMES_NUMBER,
                              NULL));
}
