#ifndef _TEST_UTILS_
#define _TEST_UIILS_


void randomiseString(char * str);
void buildRandomTestFile(const char * fillPathAndName, const char * testFrase, int size);
void buildNormalTestFile(const char * fillPathAndName, const char * testFrase, size_t buffSize, int writeTimes, bool addIdx);

#endif//_TEST_UTILS_