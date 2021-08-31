#ifndef _READ_FILE_
#define _READ_FILE_

/**
 * @file rdFile.h
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief File manipulation funtions for filscan application
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup application
 * @addtogroup library
 */

#include <stdlib.h>
#include <stdbool.h>

typedef struct sFileDetails tFileDetails;

#define FILE_READ_MAX -2

tFileDetails * openFile(const char * filePath, size_t readSize);
void releaseFile(tFileDetails * fp);
const unsigned char * getNexFileBuffer(tFileDetails * fl);
size_t getLastReadSize(tFileDetails * fl);
size_t getBuffReadSize(tFileDetails * fl);

/**
 * @}
 * @}
 * 
 */
#endif//_READ_FILE_