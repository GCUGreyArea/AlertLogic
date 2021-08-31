/**
 * @file testHashMap.cpp
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Test functionality of hash map
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup test
 * @addtogroup library
*/

#include <gtest/gtest.h>
#include <libfilestat.h>

#include "testUtils.h"


/**
 * @brief Unit tests
 *
 */

// Used to rove the number of times delete is called for map items
int delCalled = 0;

inline static uint32_t generate_key(void* data) {
    return hashMap_hashString32((const char *)data);
}

inline static void print_string(void * data) {
    printf("\"%s\"", (const char *)data);
}

inline static bool match_string(void * v1, void* v2) {
    return strcmp((const char*)v1,(const char*)v2) == 0;
}

inline static void delete_data(void * d) {
    printf("Deleting : %s\n", (const char *)d);
    delCalled++;
}

TEST(testHashMap, testThatHashMapCanBeMade) {
    printf("==> testThatHashMapCanBeMade <==\n");

    const char * test1 = "Hello there";
    const char * test2 = "Hello there.";
    const char * test3 = "Test 3";

    hashMap_t * tbl = hashMap_make(NULL);
    EXPECT_TRUE(tbl == NULL);

    // No key generator
    hashMapInit_t mapInit = {"test", generate_key,match_string,delete_data,print_string,25};
    tbl = hashMap_make(&mapInit);
    EXPECT_TRUE(tbl != NULL);


    // Size of the map should have been adjusted to a power of 2 number.
    size_t size = hashMap_getSize(tbl);

    ASSERT_FALSE(size == 25);
    ASSERT_TRUE(size == 32);

    // Now add some values to the map.
    size_t place1 = hashMap_putWithKeyAsData(tbl,(void*) test1);
    size_t place2 = hashMap_putWithKeyAsData(tbl,(void*) test2);
    size_t place3 = hashMap_putWithKeyAsData(tbl,(void*) test3);

    // Test ocupancy
    size_t ocupation = hashMap_getOcupancy(tbl);
    ASSERT_TRUE(ocupation == 3);

    printf("Map ocupancy [metric %f]\n", hashMap_calculateOcupancyMetric(tbl));

    hashMap_debug(tbl);

    // Test get fucntions
    const char * str1 = (const char*) hashMap_getWithPlace(tbl,place1,(void*)test1);
    const char * str2 = (const char*) hashMap_getWithPlace(tbl,place2,(void*)test2);
    const char * str3 = (const char*) hashMap_getWithPlace(tbl,place3,(void*)test3);

    ASSERT_STREQ(str1,test1);
    ASSERT_STREQ(str2,test2);
    ASSERT_STREQ(str3,test3);


    str1 = (const char*) hashMap_getWithKey(tbl,(void*)test1);
    str2 = (const char*) hashMap_getWithKey(tbl,(void*)test2);    // printf("Test 2: %s\n", str2);
    str3 = (const char*) hashMap_getWithKey(tbl,(void*)test3);

    ASSERT_STREQ(str1,test1);
    ASSERT_STREQ(str2,test2);
    ASSERT_STREQ(str3,test3);


    // Test itterator
    hashMap_iterator_t * it = hashMap_getItterator(tbl);

    const char * v1 = (const char*) hashMap_Itterate(it);
    const char * v2 = (const char*) hashMap_Itterate(it);
    const char * v3 = (const char*) hashMap_Itterate(it);

    // These will not be in the order they where inserted, but all strings should be not NULL
    ASSERT_TRUE(v1 != NULL);
    ASSERT_TRUE(v2 != NULL);
    ASSERT_TRUE(v3 != NULL);

    // Make sure we have three distinct strings;
    ASSERT_TRUE(strcmp(v1,v2) != 0);
    ASSERT_TRUE(strcmp(v1,v3) != 0);
    ASSERT_TRUE(strcmp(v2,v3) != 0);

    hashMap_deletItterator(it);

    // Set up the map so that we can remove items without them being deleted.
    hashMap_setDeleteOnRemove(tbl,false);

    bool res = hashMap_removeWithPlace(tbl,place1,(void*) test1);
    ASSERT_TRUE(res);
    ASSERT_EQ(delCalled,0);

    hashMap_setDeleteOnRemove(tbl,true);

    res = hashMap_removeWithKey(tbl,(void*)test2);
    ASSERT_TRUE(res);
    ASSERT_EQ(delCalled,1);
    ASSERT_EQ(hashMap_getOcupancy(tbl), 1);
    hashMap_debug(tbl);

    ASSERT_TRUE(hashMap_removeWithKey(tbl,(void*) test3));
    ASSERT_EQ(hashMap_getOcupancy(tbl), 0);
    ASSERT_EQ(delCalled,2);

    hashMap_debug(tbl);
    hashMap_teardown(tbl);



    printf("==[ testThatHashMapCanBeMade DONE ]==\n");
}

/**
 * @}
 * @}
 */
