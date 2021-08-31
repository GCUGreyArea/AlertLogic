/**
 * @file buffScan.h
 * @author Barry Robinson (barry.w.robinson64@google.com)
 * @brief Build a file scanner to profile a text file and reture basic statistics on character use and freequency.
 * @version 0.1
 * @date 2021-08-28
 *
 * @copyright Copyright (c) Alert Logic 2021
 *
 */
#ifndef _BUFF_SCAN_
#define _BUFF_SCAN_

#include "utilsList.h"
#include "hashMap.h"

typedef struct fileStats_s fileStats_t;

/**
 * @brief Strutcure for word frequency analysis
 *
 */
typedef struct wordFreqAn_s {
    char * word;
    int freq;
    int len;
    FILE * outfile;
} wordFreqAn_t;


/**
 * @brief Create a buffer for the file stats
 *
 * @return fileStats_t*
 */
fileStats_t * buffScan_init(size_t mapSize);
size_t buffScan_scan(fileStats_t* stats,const unsigned char * buffer, size_t buffLen);
void buffScan_printWords(fileStats_t* stats);
void buffScan_release(fileStats_t* stats);
utilsList_t * getWordList(fileStats_t* stats);
void printFrequencyAnalysis(fileStats_t* stats);
int getWhiteSpaceCount(fileStats_t* stats);


void buffScan_setWordStats(fileStats_t * stats, bool val);
void buffScan_setCharFreqStats(fileStats_t * stats, bool val);
void buffScan_setHighestFreqWordsStats(fileStats_t * stats, bool val);
void buffScan_setWordMatch(fileStats_t * stats, const char * funcName);
void buffScan_setOutputFile(fileStats_t * stats, FILE * file);
void buffScan_addCharToMatchFunc(unsigned char c);

// Helper
bool isWordChar(unsigned char c);
hashMap_t * getHashMap(fileStats_t* stats);

#endif//_BUFF_SCAN_