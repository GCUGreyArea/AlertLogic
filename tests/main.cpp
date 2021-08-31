/**
 * @file main.cpp
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Test harness for filescan application library
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup test
 * @addtogroup library
 */


#include <gtest/gtest.h>


int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @}
 * @}
 * 
 */