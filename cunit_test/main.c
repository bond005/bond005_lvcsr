#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "parse_transcription_str_test.h"

int main()
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
    if (!create_test_suites_for_parse_transcription_str())
    {
        return CU_get_error();
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
