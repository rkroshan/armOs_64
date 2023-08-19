#include "list.h"
#include <stddef.h>

void append_list_tail(struct HeadList *list, struct List *item)
{
    item->next = NULL; /*set next to null since it is the last element*/

    if (is_list_empty(list)) { /*if list is empty it should point to head and tail*/
        list->next = item;
        list->tail = item;
    }
    else {
        list->tail->next = item; /*else add it to the tail*/
        list->tail = item;
    }
}

struct List* remove_list_head(struct HeadList *list)
{
    struct List *item;

    if (is_list_empty(list)) {
        return NULL;
    }

    item = list->next;
    list->next = item->next;
    
    if (list->next == NULL) {
        list->tail = NULL; /*if head is null the tail should also be null*/
    }
    
    return item;
}

bool is_list_empty(struct HeadList *list)
{
    return (list->next == NULL);
}