
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "internal.h"


void * _alloc_mem_(size_t size, const char * func, const char * file, int line) {
    void * mem = malloc(size);
    if(mem == NULL) {
        fprintf(stderr,"Failed to allocte memory [bytes %ld / file %s / function %s / line %d]\n", size, file,func,line);
        return NULL;
    }

    return mem;
}