/**
 * @file utilsList.c
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Generica linked list functionality for sequention orderd access
 * @version 0.1
 * @date 2021-08-31
 *
 * @copyright Copyright (c) 2021
 * @addtogroup library
 * @{
 * @addtogroup utilsList
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "internal.h"
#include "utilsList.h"


// List functions
// Defined here to enforce proper access
struct utilsListItem_s {
    void * data;
    struct utilsListItem_s * next;
    struct utilsListItem_s * prev;
};

struct utilsList_s {
    listInit_t cbs;
    utilsListItem_t * head;
    utilsListItem_t * tail;
    int items;
};

utilsList_t * utilsListMake(listInit_t * cbs) {
    utilsList_t * list = (utilsList_t *) alloc_mem(sizeof(utilsList_t));
    if(list == NULL) {
		fprintf(stderr,"Memory allocation failure");
		return NULL;
	}

    memcpy(&list->cbs, cbs, sizeof(listInit_t));
    strcpy(list->cbs.list_name, cbs->list_name);

    list->head = NULL;
    list->tail = NULL;
    list->items = 0;

	debug(LEVEL_DEBUG,"Created list [name \"%s\" / addr %p]", list->cbs.list_name, list);

    return list;
}

void utilsListDelete(utilsList_t * list) {
    if(list) {
        utilsListItem_t * next = list->tail;
        int count=0;
        while(next) {
            utilsListItem_t * last = next;
            if(list->cbs.free_cb) {list->cbs.free_cb(next->data);}

            next = next->next;
            free(last);
            count++;
        }

        debug(LEVEL_INFO,"UtilsList : Freed [nodes %d / elements %d]\n", count, list->items);
    }

    debug(LEVEL_DEBUG,"Deleting list [name \"%s\" / addr %p]\n", list->cbs.list_name, list);

    free(list);
}


void utilsListPrint(utilsList_t * list) {
   if(list && list->cbs.print_cb) {
		utilsListItem_t * next = list->tail;

        while(next) {
            list->cbs.print_cb(next->data);

            next = next->next;
        }
    }
}

// Tail - List - Head
bool utilsListAddTail(utilsList_t * list, void * data) {
    bool ret=false;

    if(list) {
        utilsListItem_t * prev = (utilsListItem_t *) alloc_mem(sizeof(utilsListItem_t));
        if(prev == NULL) {
            fprintf(stdout,"Memory allocation failure [list \"%s\"]\n", list->cbs.list_name);
            return ret;
        }

        // Add data and make this the new head.
        prev->data = data;

        if(list->items > 1) {
            prev->next = list->tail;
            list->tail->prev = prev;
            prev->prev = NULL;
            list->tail = prev;
        }
        else if(list->items == 1) {
            prev->prev = NULL;
            list->head->prev = prev;
            prev->next = list->tail;
            list->tail = prev;
        }
        else {
            // This is the first item
            prev->next = NULL;
            prev->prev = NULL;

            list->head = prev;
            list->tail = prev;
        }

        ret=true;
        list->items++;

		debug(LEVEL_DEBUG, "List [addr %p / items %d / head %p / Tail %p / name \"%s\"]", list, list->items, list->head, list->tail, list->cbs.list_name);
    }

    return ret;
}

bool utilsListAddHead(utilsList_t * list, void * data) {
    bool ret=false;

    if(list) {
        utilsListItem_t * next = (utilsListItem_t *) alloc_mem(sizeof(utilsListItem_t));
        if(next == NULL) {
            return ret;
        }

        // Add data and make this the new head.
        next->data = data;

        if(list->items > 1) {
            next->prev = list->head;
            list->head->next = next;
            next->next = NULL;
            list->head = next;
        }
        else if(list->items == 1) {
            next->next = NULL;
            list->tail->next = next;
            next->prev = list->head;
            list->head = next;
        }
        else {
            // This is the first item
            next->next = NULL;
            next->prev = NULL;

            list->head = next;
            list->tail = next;
        }

        ret=true;
        list->items++;

	    debug(LEVEL_DEBUG, "List [addr %p / items %d / head %p / Tail %p / name \"%s\"]", list, list->items, list->head, list->tail, list->cbs.list_name);
    }

    return ret;
}

void * utilsListRemoveBy(utilsList_t * list, void * data, list_match_cb * match_cb) {
    void * ret = NULL;

    if(list && match_cb && data) {
        utilsListItem_t * next = list->tail;

        while(next) {
            if(match_cb(next->data, data) == 0 ) {
                ret = next->data;
                if(list->items == 1) {
                    list->head = NULL;
                    list->tail = NULL;
                }
                else {
                    utilsListItem_t * prev = next->prev;
                    utilsListItem_t * nx = next->next;
                    if(nx) {
                        nx->prev = prev;
                    }

                    if(prev) {
                        prev->next = nx;
                    }
                }

				list->items--;

                free(next);
                break;
            }

            next = next->next;
        }
    }

    return ret;
}

// Remove ietm from list but don't delete.
void * utilsListRemove(utilsList_t * list, void * data) {
    void * ret = NULL;
    if(list && list->cbs.match_cb && data) {
        ret = utilsListRemoveBy(list, data, list->cbs.match_cb);
    }

    return ret;
}


bool uitlsListDeleteItem(utilsList_t * list, void *data) {
	if(list && data) {
		void * item = utilsListRemove(list, data);
		if(list->cbs.free_cb) {
			list->cbs.free_cb(item);
			return true;
		}
	}

	return false;
}

void * utilsListFind(utilsList_t * list, void * data) {
    void * ret = NULL;
    if(list && list->cbs.match_cb && data) {
        ret = utilsListFindBy(list, data, list->cbs.match_cb);
    }

    return ret;
}

void * utilsListFindBy(utilsList_t * list, void * data, list_match_cb * match_cb) {
    void * ret = NULL;

    if(list && match_cb && data) {
        utilsListItem_t * next = list->tail;

        while(next) {
            if(match_cb(next->data, data) == 0) {
                ret = next->data;
                break;
            }

            next = next->next;
        }
    }

    return ret;

}

int utilsListGetSize(utilsList_t * list) {
    if(list) {
        return list->items;
    }

    return -1;
}

utilsListItem_t * utilsListGetHead(utilsList_t * list) {
    return list->head;
}

utilsListItem_t * utilsListGetTail(utilsList_t * list) {
    return list->tail;
}

utilsListItem_t * utilsListGetNext(utilsListItem_t * item) {
    return item->next;
}

utilsListItem_t * utilsListGetPrev(utilsListItem_t * item) {
    return item->prev;
}

void * utilsListGetData(utilsListItem_t * item) {
    return item->data;
}

