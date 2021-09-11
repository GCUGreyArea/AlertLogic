/**
 * @file rdFile.c
 * @author barry Robinson (barry.w.robinson64@dgmail.com)
 * @brief Internal utilities
 * @version 0.1
 * @date 2021-08-31
 *
 * @copyright Copyright (c) 2021
 * @addtogroup library
 * @{
 * @addtogroup rdFiles
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rdFile.h"
#include "internal.h"

struct sFileDetails {
    FILE * filePointer;     //!< Pointer to the file handle
    unsigned char * buffer; //!< Buffer for file reads
    size_t readSize;        //!< Size of buffer for file reads and max read size
    size_t lastReadSz;      //!< Last read size and content of buffer size
    size_t fileSize;        //!< Actual file size in bytes
    size_t readTotal;       //!< Total number of bytes read from the file
};

/**
 * @brief open a text file for reading and return a valid pointer to a sFileDetails structure
 *
 * @param filePath path to the file to open
 * @return tFileDetails* pointer
 */

tFileDetails * openFile(const char * filePath, size_t rdBlock) {

    // First, test we have valid parrameters
    if(filePath == NULL) {
        fprintf(stderr,"ERROR: File path cannot be null\n");
        return NULL;
    }
    else if(access( filePath, F_OK ) != 0) {
        fprintf(stderr,"ERROR: Cannot access file [%s]\n", filePath);
        return NULL;
    }
    else if(rdBlock == 0) {
        fprintf(stderr,"ERROR: read block must be greater than 0 or FILE_READ_MAX\n");
        return NULL;
    }

    tFileDetails * fl = alloc_mem(sizeof(tFileDetails));
    if(fl == NULL) {
        return NULL;
    }

    // Open the file handle
    fl->filePointer = fopen (filePath, "r");
    if(fl->filePointer == NULL) {
        fprintf(stderr, "ERROR: Failed to open [file %s]\n", filePath);
        free(fl);
        return NULL;
    }

    // Get the size of the file now incase wee need to allocate the total file size to the buufer
    fseek(fl->filePointer, 0, SEEK_END);
    fl->fileSize = ftell(fl->filePointer);
    // Reset the file handle to point to the start of the file
    fseek(fl->filePointer, 0, SEEK_SET);


    // Now do the read size for the buffer
    if(rdBlock == FILE_READ_MAX) {
        fl->readSize = fl->fileSize-1;
    }
    else {
        fl->readSize = rdBlock;
        if(fl->readSize > fl->fileSize) {
            fl->readSize = fl->fileSize;
        }
    }

    fl->readTotal = 0;

    // Alocate one read block
    fl->buffer = alloc_mem(fl->readSize);
    if(fl->buffer == NULL) {
        fclose(fl->filePointer);
        free(fl);
        return NULL;
    }

    fl->lastReadSz = 0;
    return fl;
}

void releaseFile(tFileDetails * fl) {
    if(fl && fl->filePointer) {
        fclose(fl->filePointer);
    }

    if(fl && fl->buffer) {
        free(fl->buffer);
    }

    if(fl) {
        free(fl);
    }
}

/**
 * @brief Get the Nex File Buffer object
 *
 * @param fl The handle to the open file
 * @return const char* The buffer read to buffSize
 */
const unsigned char * getNexFileBuffer(tFileDetails * fl) {
    if(fl == NULL) {
        fprintf(stderr,"ERROR: File structure is NULL\n");
        return NULL;
    }

    size_t readSz = 0;
    if(fl->readSize <= (fl->fileSize - fl->readTotal)) {
        readSz = fl->readSize;
    }
    else {
        readSz = fl->fileSize - fl->readTotal;
    }

    if(readSz == 0) return NULL;

    // printf("File read size : %ld\n", readSz);

    int act = fread((void*) fl->buffer, readSz, 1, fl->filePointer);
    if(act == 1) {
        fl->readTotal += readSz;
        fl->lastReadSz = readSz;
        return fl->buffer;
    }

    fprintf(stderr,"ERROR: Nothing read\n");
    return NULL;
}

size_t getLastReadSize(tFileDetails * fl) {
    if(fl) {
        return fl->lastReadSz;
    }

    fprintf(stderr,"ERROR: file structure is uninitialized\n");
    return -1;
}


size_t getBuffReadSize(tFileDetails * fl) {
    if(fl) {
        return fl->readSize;
    }

    fprintf(stderr,"ERROR: file structure is uninitialized\n");
    return -1;
}

/**
 * @}
 * @}
 *
 */
