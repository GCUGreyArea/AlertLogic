#ifndef _READ_FILE_
#define _READ_FILE_


#include <stdlib.h>
#include <stdbool.h>

typedef struct sFileDetails tFileDetails;

#define FILE_READ_MAX -2

tFileDetails * openFile(const char * filePath, size_t readSize);
void releaseFile(tFileDetails * fp);
const unsigned char * getNexFileBuffer(tFileDetails * fl);
size_t getLastReadSize(tFileDetails * fl);
size_t getBuffReadSize(tFileDetails * fl);


#endif//_READ_FILE_