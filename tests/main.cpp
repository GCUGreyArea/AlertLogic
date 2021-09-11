/**
 * @file main.cpp
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Test harness for filescan application library
 * @version 0.1
 * @date 2021-08-31
 *
 * @copyright Copyright (c) 2021
 * @addtogroup test
 * @{
 * @addtogroup main
 * @{
 *
 */


#include <gtest/gtest.h>
#include "libfilestat.h"

/**
 * @brief Main function for test harness
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, const char ** argv) {
    testing::InitGoogleTest(&argc, (char**) argv);
    return RUN_ALL_TESTS();
}

/**
 * @}
 * @}
 */