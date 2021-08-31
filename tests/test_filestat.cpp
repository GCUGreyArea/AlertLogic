/**
 * @file test_filestat.cpp
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Test file read and write functions
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup application
 * @addtogroup test
 * @addtogroup library
 */

#include <gtest/gtest.h>
#include <libfilestat.h>


#include "testUtils.h"


TEST(testFiles, testThatTestFileCanBeCreated) {
    printf("==> testThatTestFileCanBeCreated <==\n");

    char str[34] = {'\0'};

    str[0] = (char)0;
    str[1] = (char)1;
    str[2] = (char)2;
    str[3] = (char)3;
    str[4] = (char)4;
    str[5] = (char)5;
    str[6] = (char)6;
    str[7] = (char)7;
    str[8] = (char)8;
    str[9] = (char)9;
    str[10] = (char)10;
    str[11] = (char)11;
    str[12] = (char)12;
    str[13] = (char)13;
    str[14] = (char)14;
    str[15] = (char)15;
    str[16] = (char)16;
    str[17] = (char)17;
    str[18] = (char)18;
    str[19] = (char)19;
    str[20] = (char)20;
    str[21] = (char)21;
    str[22] = (char)22;
    str[23] = (char)23;
    str[24] = (char)24;
    str[25] = (char)25;
    str[26] = (char)26;
    str[27] = (char)27;
    str[28] = (char)28;
    str[29] = (char)29;
    str[30] = (char)30;
    str[31] = (char)31;
    str[32] = (char)32;
    str[33] = '\0';

    buildNormalTestFile("./test0.txt", (const char*) str, 34, 1, false);

     tFileDetails * fp = openFile("./test0.txt", 34);
     EXPECT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    EXPECT_TRUE(buff != nullptr);

    EXPECT_TRUE(buff[0]  == (char)0);
    EXPECT_TRUE(buff[1]  == (char)1);
    EXPECT_TRUE(buff[2]  == (char)2);
    EXPECT_TRUE(buff[3]  == (char)3);
    EXPECT_TRUE(buff[4]  == (char)4);
    EXPECT_TRUE(buff[5]  == (char)5);
    EXPECT_TRUE(buff[6]  == (char)6);
    EXPECT_TRUE(buff[7]  == (char)7);
    EXPECT_TRUE(buff[8]  == (char)8);
    EXPECT_TRUE(buff[9]  == (char)9);
    EXPECT_TRUE(buff[10] == (char)10);
    EXPECT_TRUE(buff[11] == (char)11);
    EXPECT_TRUE(buff[12] == (char)12);
    EXPECT_TRUE(buff[13] == (char)13);
    EXPECT_TRUE(buff[14] == (char)14);
    EXPECT_TRUE(buff[15] == (char)15);
    EXPECT_TRUE(buff[16] == (char)16);
    EXPECT_TRUE(buff[17] == (char)17);
    EXPECT_TRUE(buff[18] == (char)18);
    EXPECT_TRUE(buff[19] == (char)19);
    EXPECT_TRUE(buff[20] == (char)20);
    EXPECT_TRUE(buff[21] == (char)21);
    EXPECT_TRUE(buff[22] == (char)22);
    EXPECT_TRUE(buff[23] == (char)23);
    EXPECT_TRUE(buff[24] == (char)24);
    EXPECT_TRUE(buff[25] == (char)25);
    EXPECT_TRUE(buff[26] == (char)26);
    EXPECT_TRUE(buff[27] == (char)27);
    EXPECT_TRUE(buff[28] == (char)28);
    EXPECT_TRUE(buff[29] == (char)29);
    EXPECT_TRUE(buff[30] == (char)30);
    EXPECT_TRUE(buff[31] == (char)31);
    EXPECT_TRUE(buff[32] == (char)32);

    // Test that it can be parsed
    fileStats_t * stats = buffScan_init(4096);
    int scanned = buffScan_scan(stats,buff,32);

    EXPECT_EQ(scanned,32);

    printf("----------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");


    // Teardown
    releaseFile(fp);
    remove("./test0.txt");
}

/**
 * @brief Test that a file can be read repeatedly and return valid results
 *
 */
TEST(testFiles, testGoodFileCanBeRead) {
    printf("==> testGoodFileCanBeRead <==\n");
    // Set up a reapeating test phrase and create a test file
    const char * phrase = "Hello test ";
    buildNormalTestFile("./test1.txt", phrase, strlen(phrase), 3, true);

    // Now test the file library againt the file
    tFileDetails * fp = openFile("./test1.txt", strlen(phrase)+1);
    char buffer[strlen(phrase)+1] = {'\0'};

    EXPECT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    EXPECT_TRUE(buff != nullptr);

    strncpy(buffer,(char*)buff,strlen(phrase)+1);
    buffer[strlen(phrase)+1] = '\0';

    printf("Buffer : %s\n", buffer);

    EXPECT_TRUE(strcmp(buffer,"Hello test 0") == 0);

    buff = getNexFileBuffer(fp);
    EXPECT_TRUE(buff != nullptr);

    strncpy(buffer,(char*)buff,strlen(phrase)+1);
    buffer[strlen(phrase)+1] = '\0';

    // printf("Buffer %s\n", buffer);
    EXPECT_TRUE(strcmp(buffer,"Hello test 1") == 0);

    buff = getNexFileBuffer(fp);
    EXPECT_TRUE(buff != nullptr);

    strncpy(buffer,(char*)buff,strlen(phrase)+1);
    buffer[strlen(phrase)+1] = '\0';

    // printf("Buffer %s\n", buffer);
    EXPECT_TRUE(strcmp(buffer,"Hello test 2") == 0);

    releaseFile(fp);
    remove("./test1.txt");


    printf("==[ testGoodFileCanBeRead END ]==\n");
}

/**
 * @brief Test that a bad file fails in a consitant and predictable way
 *
 */
TEST(testFiles, testBadFileCantBeRead) {
    printf("==> testBadFileCantBeRead <==\n");
    tFileDetails * fp = openFile("no_file.file", 45);
    EXPECT_TRUE(fp == nullptr);

    // Build a valid file to open with invalid arguments
    buildNormalTestFile("./test2.txt", "test",strlen("test"), 1,false);

    // Zero buffer size should fail
    fp = openFile("./test2.txt", 0);
    EXPECT_TRUE(fp == nullptr);

    // This should now work
    fp = openFile("./test2.txt", 10);
    EXPECT_TRUE(fp != nullptr);
    releaseFile(fp);

    remove("./test2.txt");
    printf("==[ testBadFileCantBeRead END]==\n");
}

TEST(testFiles,testFileReadOnResorceFile) {
    printf("==> testFileReadOnResorceFile <==\n");
    tFileDetails * fp = openFile("./resouces/small.txt", FILE_READ_MAX);
    EXPECT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    size_t readSz = getLastReadSize(fp);

    fileStats_t * stats = buffScan_init(4096);
    size_t scanned = buffScan_scan(stats,buff,readSz);

    EXPECT_EQ(scanned,readSz);

    printf("----------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");

    // Teardown the scanner and remove the file
    buffScan_release(stats);
    releaseFile(fp);

    printf("==[ testOnResorceFile END]==\n");
}

TEST(testFiles,testWithLargeFile) {
    printf("==> testWithLargeFile <==\n");
    // const char * inFile = "./resouces/ascii.txt";
    const char * inFile = "./main.cpp";
    const char * outFile = "./output.xml";
    tFileDetails * fp = openFile(inFile, FILE_READ_MAX);
    EXPECT_TRUE(fp != nullptr);

    // Test that we can write to file
    FILE * write = fopen(outFile, "w");

    const unsigned char * buff = getNexFileBuffer(fp);
    size_t readSz = getLastReadSize(fp);

    printf("Input file %s\n", (char*) buff);


    fileStats_t * stats = buffScan_init(4096);

    // These need to be set before we scan
    buffScan_setCharFreqStats(stats,true);
    buffScan_setWordStats(stats,true);
    buffScan_setOutputFile(stats,write);


    size_t scanned = buffScan_scan(stats,buff,readSz);

    EXPECT_EQ(scanned,readSz);

    hashMap_t * map = getHashMap(stats);

    hashMap_printStats(map);
    float metric = hashMap_calculateOcupancyMetric(map);

    // Make sure the map is not running out of space..
    EXPECT_TRUE(metric > 80.0);

    // Output to the file
    printFrequencyAnalysis(stats);

    // Close the file
    fclose(write);
    ASSERT_TRUE(access(outFile, F_OK ) == 0);
    releaseFile(fp);

    fp = openFile(outFile, FILE_READ_MAX);
    EXPECT_TRUE(fp != NULL);

    buff = getNexFileBuffer(fp);
    EXPECT_TRUE(buff != NULL);

    printf("Output file %s\n", (char*) buff);

    releaseFile(fp);


    // Teardown the scanner and remove the file
    buffScan_release(stats);

    printf("==[ testWithLargeFile END ]==\n");
}

/**
 * @}
 * @}
 * @}
 */

