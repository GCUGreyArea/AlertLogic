#ifndef __UTILS_LIST__
#define __UTILS_LIST__

/**
 * @file utilsList.h
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Generica linked list functionality for sequention orderd access
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup application
 * @addtogroup library
 */


// Functions and structures for list access
typedef struct utilsListItem_s utilsListItem_t;
typedef struct utilsList_s utilsList_t;

typedef void (list_free_cb)(void*);
typedef void (list_print_cb)(void*);

// Match func takes a key and matches it against the record,
// it should work like strcmp returning positive / negative
typedef int  (list_match_cb)(void*,void*);

#define MAX_LIST_NAME 35

typedef struct listInit_s {
    char list_name[MAX_LIST_NAME];
    list_free_cb  * free_cb;
    list_print_cb * print_cb;
    list_match_cb * match_cb;
} listInit_t;

utilsList_t * utilsListMake(listInit_t * init);
void utilsListDelete(utilsList_t * list);

// Tail - List - Head
bool utilsListAddTail(utilsList_t * list, void * item);
bool utilsListAddHead(utilsList_t * list, void * item);

utilsListItem_t * utilsListGetHead(utilsList_t * list);
utilsListItem_t * utilsListGetTail(utilsList_t * list);

utilsListItem_t * utilsListGetNext(utilsListItem_t * item);
utilsListItem_t * utilsListGetPrev(utilsListItem_t * item);

void * utilsListGetData(utilsListItem_t * item);
void * utilsListRemove(utilsList_t * list, void * data);
void * utilsListRemoveBy(utilsList_t * list, void * data, list_match_cb * match);

bool uitlsListDeleteItem(utilsList_t * list, void *data);

void * utilsListFind(utilsList_t * list, void * data);
void * utilsListFindBy(utilsList_t * list, void * data, list_match_cb * match);

void utilsListPrint(utilsList_t * list);

int utilsListGetSize(utilsList_t * list);

/**
 * @}
 * @}
 * 
 */
#endif//__UTILS_LIST__