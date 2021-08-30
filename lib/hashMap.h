#ifndef _HASH_MAP_
#define _HASH_MAP_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct hashTable_s hashMap_t;
typedef struct hashNode_s hashNode_t;
typedef struct hashBaseNode_s hashBaseNode_t;
typedef struct hashMap_iterator_s hashMap_iterator_t;

typedef uint32_t (generat_key_CB)(void*);
typedef bool (match_data_CB)(void*,void*);
typedef void (delete_data_CB)(void*);
typedef void (print_data_CB)(void*);

#define HASH_MAPP_ERR -2
#define MAX_HASH_TBL_NAME 35

typedef struct hashMapInit_s {
    char name[MAX_HASH_TBL_NAME];   // !< Name of the table for debug
    generat_key_CB * keyCB;         // !< Callback to generate hash key
    match_data_CB  * matchCB;       // !< Callback to match data
    delete_data_CB * deleteCB;      // !< Callback to delete data
    print_data_CB  * printCB;       // !< Claaback to print data
    size_t           hashSize;      // !< Size of the hash table
} hashMapInit_t;

hashMap_t * hashMap_make(hashMapInit_t * init);
size_t hashMap_putWithKeyAsData(hashMap_t * map, void * data);
size_t hashMap_putDataWithKey(hashMap_t * map, void * data, void * keyData);

void * hashMap_getWithPlace(hashMap_t * map, size_t place,void * keyData);
void * hashMap_getWithKey(hashMap_t * map, void * keyData);
bool hashMap_removeWithPlace(hashMap_t * map, size_t place,void * keyData);
bool hashMap_removeWithKey(hashMap_t * map, void * keyData);

size_t hashMap_getSize(hashMap_t * map);
size_t hashMap_getOcupancy(hashMap_t * map);
void hashMap_debug(hashMap_t * map);

// Fill out a hasMap itterator with valid values for itteration
hashMap_iterator_t * hashMap_getItterator(hashMap_t * map);
void * hashMap_Itterate(hashMap_iterator_t * it);
void hashMap_deletItterator(hashMap_iterator_t * it);

void hashMap_teardown(hashMap_t * map);
void hashMap_setDeleteOnRemove(hashMap_t * map, bool val);

uint32_t hashMap_hashString32(const char * s);
float hashMap_calculateOcupancyMetric(hashMap_t * map);
void hashMap_printStats(hashMap_t *map);


#endif//_HASH_MAP_