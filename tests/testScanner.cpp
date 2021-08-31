
/**
 * @file testScanner.cpp
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup test
 * @{
 * @addtogroup scanner
 * @{
 */
#include <gtest/gtest.h>
#include <libfilestat.h>

#include "testUtils.h"


/**
 * @brief Test that the scanner can find all words in a file
 *
 */
TEST(testScanner,testScannerCanFindWords) {
    printf("==> testScannerCanFindWords <==\n");

    const char * phrase = "one two three four five six\n";
    // Build a valid file to open with invalid arguments
    buildNormalTestFile("./test3.txt",phrase,strlen(phrase),2,false);
    tFileDetails * fp = openFile("./test3.txt", (strlen(phrase) * 2) + 4);
    ASSERT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    ASSERT_TRUE(buff != nullptr);


    size_t lastRead = getLastReadSize(fp);

    fileStats_t * stats = buffScan_init(4096);

    // These need to be set before we scan
    buffScan_setCharFreqStats(stats,true);
    buffScan_setWordStats(stats,true);

    // Scan in one read
    buffScan_scan(stats,buff,lastRead);

    // buff = getNexFileBuffer(fp);
    // lastRead = getLastReadSize(fp);
    // buffScan_scan(stats,buff,lastRead);

    printf("testScannerCanFindWords 1\n");
    printf("--------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");

    // Get the word list and test we have what we think we should have
    utilsList_t * list = getWordList(stats);
    EXPECT_TRUE(list != nullptr);
    // printf("testScannerCanFindWords 2\n");
    // printf("--------------------------\n");
    // utilsListPrint(list);
    // printf("\n--------------------------\n");

    GTEST_ASSERT_EQ(utilsListGetSize(list),6);

    utilsListItem_t * it = utilsListGetTail(list);
    EXPECT_TRUE(it != nullptr);
    wordFreqAn_t * freq = (wordFreqAn_t*) utilsListGetData(it);
    EXPECT_TRUE(strcmp(freq->word,"one") == 0);
    EXPECT_EQ(freq->freq,2);

    it = utilsListGetNext(it);
    EXPECT_TRUE(it != nullptr);
    freq = (wordFreqAn_t*) utilsListGetData(it);
    EXPECT_TRUE(strcmp(freq->word, "two") == 0);
    EXPECT_EQ(freq->freq,2);

    it = utilsListGetNext(it);
    EXPECT_TRUE(it != nullptr);
    freq = (wordFreqAn_t*) utilsListGetData(it);
    EXPECT_TRUE(strcmp(freq->word,"three") == 0);
    EXPECT_EQ(freq->freq,2);

    it = utilsListGetNext(it);
    EXPECT_TRUE(it != nullptr);
    freq = (wordFreqAn_t*) utilsListGetData(it);
    EXPECT_TRUE(strcmp(freq->word,"four") == 0);
    EXPECT_EQ(freq->freq,2);

    it = utilsListGetNext(it);
    EXPECT_TRUE(it != nullptr);
    freq = (wordFreqAn_t*) utilsListGetData(it);
    EXPECT_TRUE(strcmp((char*)freq->word,"five") == 0);
    EXPECT_EQ(freq->freq,2);

    it = utilsListGetNext(it);
    EXPECT_TRUE(it != nullptr);
    freq = (wordFreqAn_t*) utilsListGetData(it);
    EXPECT_TRUE(strcmp(freq->word,"six") == 0);
    EXPECT_EQ(freq->freq,2);

    // Teardown the scanner and remove the file
    buffScan_release(stats);
    releaseFile(fp);
    remove("./test3.txt");
    printf("==[ testScannerCanFindWords END]==\n");
}


TEST(testScanner, testThatTabsAreScanned) {
    printf("==> testThatTabsAreScanned <==\n");

    const char * phrase = "\t\t\t\t\t";
    buildNormalTestFile("./test4.txt", phrase, strlen(phrase), 3, false);

    tFileDetails * fp = openFile("./test4.txt", strlen(phrase) * 4);
    ASSERT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    ASSERT_TRUE(buff != nullptr);

    size_t lastRead = getLastReadSize(fp);
    fileStats_t * stats = buffScan_init(4096);
    buffScan_scan(stats,buff,lastRead);

    ASSERT_EQ(getWhiteSpaceCount(stats),15);

    printf("----------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");


    buffScan_release(stats);
    releaseFile(fp);
    remove("./test4.txt");
    printf("==[ testThatTabsAreScanned END]==\n");

}

TEST(testScanner, testThatSpacesAreScanned) {
    printf("==> testThatSpacesAreScanned <==\n");

    const char * phrase = "     ";
    buildNormalTestFile("./test5.txt", phrase, strlen(phrase), 3, false);

    tFileDetails * fp = openFile("./test5.txt", strlen(phrase) * 4);
    ASSERT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    ASSERT_TRUE(buff != nullptr);

    size_t lastRead = getLastReadSize(fp);
    fileStats_t * stats = buffScan_init(4096);
    buffScan_scan(stats,buff,lastRead);

    ASSERT_EQ(getWhiteSpaceCount(stats),15);

    printf("----------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");


    buffScan_release(stats);
    releaseFile(fp);
    remove("./test5.txt");
    printf("==[ testThatSpacesAreScanned END]==\n");

}

TEST(testScanner, testThatNewLinesAreScanned) {
    printf("==> testThatNewLinesAreScanned <==\n");

    const char * phrase = "\n\n\n\n\n";
    buildNormalTestFile("./test6.txt", phrase, strlen(phrase), 3, false);

    tFileDetails * fp = openFile("./test6.txt", strlen(phrase) * 4);
    ASSERT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    ASSERT_TRUE(buff != nullptr);

    size_t lastRead = getLastReadSize(fp);
    fileStats_t * stats = buffScan_init(4096);
    buffScan_scan(stats,buff,lastRead);

    ASSERT_EQ(getWhiteSpaceCount(stats),15);

    printf("----------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");


    buffScan_release(stats);
    releaseFile(fp);
    remove("./test6.txt");
    printf("==[ testThatNewLinesAreScanned END]==\n");

}

TEST(testScanner, testThatCarageRetursAndLineFeedsAreScanned) {
    printf("==> testThatCarageRetursAndLineFeedsAreScanned <==\n");

    const char * fileName = "./test7.txt";

    const char * phrase = "\r\f\r\f\r";
    buildNormalTestFile(fileName, phrase, strlen(phrase), 3, false);

    tFileDetails * fp = openFile(fileName, strlen(phrase) * 4);
    ASSERT_TRUE(fp != nullptr);

    const unsigned char * buff = getNexFileBuffer(fp);
    ASSERT_TRUE(buff != nullptr);

    size_t lastRead = getLastReadSize(fp);
    fileStats_t * stats = buffScan_init(4096);
    buffScan_scan(stats,buff,lastRead);

    ASSERT_EQ(getWhiteSpaceCount(stats),15);

    printf("----------------------------\n");
    printFrequencyAnalysis(stats);
    printf("\n--------------------------\n");


    buffScan_release(stats);
    releaseFile(fp);
    remove(fileName);
    printf("==[ testThatCarageRetursAndLineFeedsAreScanned END]==\n");

}

TEST(testScanner,testScannerReconisesNumberSign) {
    const char * fileName = "./test8.txt";
    const char * phrase = "##########";
    // Build a valid file to open with invalid arguments
    buildNormalTestFile(fileName,phrase,strlen(phrase),2,false);
    tFileDetails * fp = openFile(fileName, (strlen(phrase) * 2) + 4);
    ASSERT_TRUE(fp != nullptr);

    fileStats_t * stats = buffScan_init(4096);
    buffScan_setCharFreqStats(stats,true);

    const unsigned char * buff = getNexFileBuffer(fp);
    ASSERT_TRUE(buff != nullptr);

    size_t lastRead = getLastReadSize(fp);

    buffScan_scan(stats,buff,lastRead);

    printFrequencyAnalysis(stats);

    buffScan_release(stats);
    releaseFile(fp);
    remove(fileName);
}

/**
 * @}
 * @}
 */
